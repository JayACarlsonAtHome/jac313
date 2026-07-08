// store_bench — one generic, fully-parameterized Store throughput benchmark.
//
// The instrument, not the answer: instead of hardcoding "the fast settings"
// (which are machine-specific), this lets you sweep a parameter on YOUR hardware
// and read the peak off the curve. Mirrors the project's ethos — Setup senses the
// toolchain; store_bench senses the performance envelope.
//
// Single config:
//   store_bench --threads 50 --events-per-thread 20000 --runs 30 --persist none
// Sweep a parameter (find the per-machine peak):
//   store_bench --sweep events=100..2000:100 --threads 16 --runs 20 --persist none
//   store_bench --sweep batch=1000..32000:1000 --persist sql --runs 8
//   store_bench --sweep threads=1..32:1 --events-per-thread 50000 --runs 10
//
// Axes: events (events-per-thread), threads, batch (durable batch size).
// Every config is a measurement SET: --runs N runs -> {median, high, low, stddev}
// via bench_stats.hpp. Median is the headline (a tight median over many runs reads
// as "consistently fast"); persist=none is the in-memory ceiling.
//
// PROCESS-LEVEL TIMING: each timed run is the FULL lifetime of a fresh child process
// (the binary re-execs itself with --single-run). The clock spans fork/exec -> static
// init -> store construction -> work -> finalize -> destructor teardown -> exit — i.e.
// "time ./prog", what a user actually sees, setup and teardown included, not just the
// hot loop. This is why the durable numbers count the real flush, not buffered bytes.
//
// Metric counts are compile-time (ts_store_config) — fixed here to the pure
// payload+flags shape; metric variants would be separate compiled binaries.

#include <algorithm>
#include <array>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <thread>
#include <vector>

#include <unistd.h>      // fork, execv, readlink, _exit, dup2  (POSIX; project is Linux-only)
#include <sys/wait.h>    // waitpid, WIFEXITED, WEXITSTATUS
#include <fcntl.h>       // open, O_WRONLY  (silence the worker's stdout)

#include "test_common.hpp"
#include "bench_stats.hpp"

#ifdef JAC313_STORE_HAS_SQL_PERSIST
#include <unistd.h>     // gethostname
#include "jac313/Qlite/v002/Sqlite.hpp"   // jac313::Qlite::v002::Sqlite — the results DB receiver
#include "jac313_results_db.hpp"           // jac313::results — shared schema + dimension helpers
#include "jac313_harness_version.hpp"
#endif

using namespace jac313::Store::v002;
using namespace std::chrono;

using LogConfig = ts_store_config<true, 6, 20, 43, 0, 0, false>;
using LogxStore = ts_store<LogConfig>;

namespace {

// ---- parameters (runtime) ----
struct Params {
    std::size_t threads = 16;
    std::size_t events_per_thread = 20'000;
    std::size_t runs = 20;
    std::size_t batch = 10'000;          // durable double-buffer batch
    std::string persist = "none";        // none | binary | jtext | html | json | sql
    std::string base_name = "bench";
    std::uint64_t flags = 0;             // per-event user flag mask
    std::string db_path;                 // --db: append one row per run to this SQLite DB (via Qlite)
    std::string label;                   // --label: human label for the config (e.g. "2 flags, non-durable")
    std::int64_t run_id = 0;             // runID: one per --suite run (0 = allocate one per single record)
};

struct Sweep {
    std::string axis;                    // "events" | "threads" | "batch"
    std::size_t lo = 0, hi = 0, step = 0;
};

std::uint64_t default_flags() {
    std::uint64_t f = 0;
    f = set_user_flag(f, StoreFlags::UserFlag::LogConsole);
    f = set_user_flag(f, StoreFlags::UserFlag::KeeperRecord);
    f = set_user_flag(f, StoreFlags::UserFlag::HotCacheHint);
    return f;
}

std::uint64_t parse_flags(const std::string& csv) {
    std::uint64_t f = 0;
    std::string tok;
    auto apply = [&](const std::string& name) {
        if (name == "console")  f = set_user_flag(f, StoreFlags::UserFlag::LogConsole);
        else if (name == "keeper")   f = set_user_flag(f, StoreFlags::UserFlag::KeeperRecord);
        else if (name == "database") f = set_user_flag(f, StoreFlags::UserFlag::DatabaseEntry);
        else if (name == "hotcache") f = set_user_flag(f, StoreFlags::UserFlag::HotCacheHint);
    };
    for (char c : csv) {
        if (c == ',') { apply(tok); tok.clear(); }
        else tok.push_back(c);
    }
    if (!tok.empty()) apply(tok);
    return f;
}

// Set the first N of the 7 user flags (bits 0-6) — for the flag-overhead sweep
// (0 / 2 / 4 / 6). Non-durable has no routing sink, so these are pure bit-set cost:
// exactly the flag overhead we want to isolate.
std::uint64_t flags_by_count(std::size_t n) {
    std::uint64_t f = 0;
    for (std::size_t i = 0; i < n && i < 7; ++i) {
        f = set_user_flag(f, static_cast<StoreFlags::UserFlag>(i));
    }
    return f;
}

std::optional<Sweep> parse_sweep(const std::string& spec) {
    // AXIS=LO..HI:STEP
    const auto eq = spec.find('=');
    const auto dots = spec.find("..");
    const auto colon = spec.find(':', dots == std::string::npos ? 0 : dots);
    if (eq == std::string::npos || dots == std::string::npos || colon == std::string::npos)
        return std::nullopt;
    Sweep s;
    s.axis = spec.substr(0, eq);
    try {
        s.lo = std::stoull(spec.substr(eq + 1, dots - eq - 1));
        s.hi = std::stoull(spec.substr(dots + 2, colon - dots - 2));
        s.step = std::stoull(spec.substr(colon + 1));
    } catch (...) { return std::nullopt; }
    if (s.step == 0 || s.hi < s.lo) return std::nullopt;
    return s;
}

std::string arg_value(const std::string& a, const char* key) {
    const std::string pfx = std::string(key) + "=";
    if (a.rfind(pfx, 0) == 0) return a.substr(pfx.size());
    return {};
}

// minimal, self-contained host tag for the "peak on this machine" line
std::string host_tag() {
    std::string cpu = "cpu?";
    int cores = 0;
    std::ifstream in("/proc/cpuinfo");
    std::string line;
    while (std::getline(in, line)) {
        if (line.rfind("model name", 0) == 0) {
            const auto c = line.find(':');
            if (c != std::string::npos) cpu = line.substr(c + 2);
        } else if (line.rfind("processor", 0) == 0) {
            ++cores;
        }
    }
    return cpu + ", " + std::to_string(cores) + "c";
}

std::unique_ptr<IEventSink> make_sink(const Params& p) {
    const std::size_t im = LogConfig::the_IntMetrics;
    const std::size_t dm = LogConfig::the_DblMetrics;
    if (p.persist == "none") return nullptr;
    return make_persistence_sink(p.persist, p.base_name, im, dm, PersistMode::All);
}

// Remove prior persisted output (base_name*) so each run's footprint is measured alone
// (and durable output doesn't accumulate across configs/compilers).
void clean_output(const std::string& base) {
    namespace fs = std::filesystem;
    try {
        fs::path bp(base);
        fs::path dir = bp.has_parent_path() ? bp.parent_path() : fs::path(".");
        const std::string stem = bp.filename().string();
        if (!fs::exists(dir)) return;
        for (const auto& e : fs::directory_iterator(dir))
            if (e.is_regular_file() && e.path().filename().string().rfind(stem, 0) == 0)
                fs::remove(e.path());
    } catch (...) {}
}

// The pure, user-visible workload: construct the store, attach persistence (durable),
// drive every thread to completion, finalize the writer, then let the store fall out of
// scope (its destructor — munmap / close / final sink teardown — runs before we return).
// NO timing and NO output cleanup here: this is the body a freshly-exec'd child process
// runs end to end, so the parent's process-level clock captures construction + work +
// finalize + teardown exactly as a real program start->exit would. Returns false on
// any failure (the child turns that into a non-zero exit).
bool do_workload(const Params& p, bool verify) {
    LogxStore store(p.threads, p.events_per_thread);

    if (p.persist != "none") {
        if (auto sink = make_sink(p)) {
            store.attach_persistence(std::make_unique<DoubleBufferedWriter>(std::move(sink), p.batch));
        } else {
            std::cerr << "ERROR: persist='" << p.persist << "' unavailable (SQL needs JAC313_STORE_HAS_SQL_PERSIST)\n";
            return false;
        }
    }

    std::array<std::string, 8> pre_payloads;
    for (std::size_t i = 0; i < 8; ++i) pre_payloads[i] = std::string(LogxStore::test_messages[i]);
    std::array<std::string, 5> pre_cats;
    for (std::size_t i = 0; i < 5; ++i) pre_cats[i] = std::string(LogxStore::categories[i]);

    std::vector<std::thread> ths;
    ths.reserve(p.threads);
    for (std::size_t t = 0; t < p.threads; ++t) {
        ths.emplace_back([&, t]() {
            std::array<std::int64_t, LogConfig::the_IntMetrics> ints{};
            std::array<double, LogConfig::the_DblMetrics> dbls{};
            for (std::size_t i = 0; i < p.events_per_thread; ++i) {
                std::string payload = pre_payloads[i % pre_payloads.size()];
                std::string cat = pre_cats[t % pre_cats.size()];
                std::uint64_t f = set_severity(p.flags, static_cast<StoreFlags::Severity>(i % 8));
                auto [ok, id] = store.save_event(t, i, std::move(payload), f, std::move(cat), true, ints, dbls);
                if (!ok) { std::cerr << "CLAIM FAILED t=" << t << " i=" << i << "\n"; std::abort(); }
            }
        });
    }
    for (auto& th : ths) th.join();
    // Drain + flush the persistence writer (join its worker, finalize the sink). No-op when
    // non-durable. The store destructor that follows at scope exit does the rest of teardown;
    // both land inside the child's process lifetime, so both are counted.
    store.finalize_persistence();

    // Verification is correctness, not throughput. It runs only when asked (the parent asks
    // on the last run as a sanity net) and, because the child exits right after, its cost
    // never lands in a recorded timing — only failing/non-failing matters.
    if (verify && !store.verify_level01()) {
        std::cerr << "STRUCTURAL VERIFICATION FAILED\n";
        store.diagnose_failures();
        return false;
    }
    return true;
}

// Absolute path to the running benchmark binary (Linux). Empty on failure.
std::string self_exe() {
    char buf[4096];
    const ssize_t n = ::readlink("/proc/self/exe", buf, sizeof(buf) - 1);
    if (n <= 0) return {};
    return std::string(buf, static_cast<std::size_t>(n));
}

// argv that re-invokes THIS binary as a single-shot worker for config p. Workers never
// touch the results DB and never print stats — the parent owns measurement. --flags-raw
// carries the exact flag mask so the child reproduces the parent's flags bit-for-bit.
std::vector<std::string> worker_argv(const std::string& exe, const Params& p, bool verify) {
    std::vector<std::string> a = {
        exe, "--single-run",
        "--threads",           std::to_string(p.threads),
        "--events-per-thread", std::to_string(p.events_per_thread),
        "--persist",           p.persist,
        "--base-name",         p.base_name,
        "--batch",             std::to_string(p.batch),
        "--flags-raw",         std::to_string(p.flags),
    };
    if (verify) a.push_back("--verify");
    return a;
}

// One timed run = the FULL lifetime of a fresh child process doing exactly one workload:
// fork+exec -> dynamic link -> static init -> construct -> work -> finalize -> destructor
// teardown -> exit. The parent's monotonic clock spans that whole arc, so the recorded
// number is what a user actually sees when they launch the program ("time ./prog"), not
// just the hot loop. Returns microseconds, or nullopt if the worker did not exit clean.
std::optional<std::size_t> run_one(const Params& p, bool verify) {
    static const std::string exe = self_exe();
    if (exe.empty()) { std::cerr << "ERROR: cannot resolve /proc/self/exe for worker spawn\n"; return std::nullopt; }

    // Clean prior output in the PARENT so the child's measured lifetime is pure workload
    // (a real program doesn't delete leftovers first). No-op when non-durable.
    if (p.persist != "none") clean_output(p.base_name);

    const std::vector<std::string> sargs = worker_argv(exe, p, verify);
    std::vector<char*> argv;
    argv.reserve(sargs.size() + 1);
    for (const auto& s : sargs) argv.push_back(const_cast<char*>(s.c_str()));
    argv.push_back(nullptr);

    const auto start = steady_clock::now();
    const pid_t pid = ::fork();
    if (pid < 0) { std::cerr << "ERROR: fork failed for worker spawn\n"; return std::nullopt; }
    if (pid == 0) {
        // Worker is silent on stdout (the parent owns all reporting); stderr stays for real errors.
        const int devnull = ::open("/dev/null", O_WRONLY);
        if (devnull >= 0) { ::dup2(devnull, STDOUT_FILENO); ::close(devnull); }
        ::execv(exe.c_str(), argv.data());
        std::perror("execv");          // only reached if exec itself failed
        _exit(127);
    }
    int status = 0;
    while (::waitpid(pid, &status, 0) < 0) { /* retry on EINTR */ }
    const auto end = steady_clock::now();

    if (!(WIFEXITED(status) && WEXITSTATUS(status) == 0)) {
        std::cerr << "worker FAILED (config '" << p.label << "', persist=" << p.persist << ")\n";
        return std::nullopt;
    }
    return static_cast<std::size_t>(duration_cast<microseconds>(end - start).count());
}

// One measurement set: RUNS timed runs -> durations. Returns empty on any failure.
std::vector<std::size_t> measure_set(const Params& p) {
    std::vector<std::size_t> durations;
    durations.reserve(p.runs);
    for (std::size_t r = 0; r < p.runs; ++r) {
        auto us = run_one(p, /*verify=*/ (r + 1 == p.runs));   // verify the last run only
        if (!us) return {};
        durations.push_back(*us);
    }
    return durations;
}

void apply_axis(Params& p, const std::string& axis, std::size_t v) {
    if (axis == "events")       p.events_per_thread = v;
    else if (axis == "threads") p.threads = v;
    else if (axis == "batch")   p.batch = v;
}

#ifdef JAC313_STORE_HAS_SQL_PERSIST
// ---- the results DB (receiver): one append-only row per run, written via Qlite ----
int set_flag_count(std::uint64_t f) { int n = 0; for (int i = 0; i < 7; ++i) if (f & (1ull << i)) ++n; return n; }

std::string compiler_id() {
#if defined(__clang__)
    return "clang " + std::to_string(__clang_major__) + "." + std::to_string(__clang_minor__);
#elif defined(__GNUC__)
    return "gcc " + std::to_string(__GNUC__) + "." + std::to_string(__GNUC_MINOR__);
#else
    return "unknown";
#endif
}

// Full compiler identity for the results.db compiler dimension: name + EXACT version (with patch, to
// tie a result to its precise toolchain) + major (for clean grouping even when version strings differ).
// CompilerInfo now lives in jac313_results_db.hpp (jac313::results::CompilerInfo).
jac313::results::CompilerInfo this_compiler() {
#if defined(__clang__)
    return {"clang", std::to_string(__clang_major__) + "." + std::to_string(__clang_minor__) + "." +
            std::to_string(__clang_patchlevel__), __clang_major__};
#elif defined(__GNUC__)
    return {"gcc", std::to_string(__GNUC__) + "." + std::to_string(__GNUC_MINOR__) + "." +
            std::to_string(__GNUC_PATCHLEVEL__), __GNUC__};
#else
    return {"unknown", "0", 0};
#endif
}

// total bytes of the persisted output (every file starting with base_name) — the on-disk
// footprint of jtext / binary / sql for this config.
std::uint64_t measure_output_bytes(const std::string& base) {
    namespace fs = std::filesystem;
    std::uint64_t total = 0;
    try {
        fs::path bp(base);
        fs::path dir = bp.has_parent_path() ? bp.parent_path() : fs::path(".");
        const std::string stem = bp.filename().string();
        for (const auto& e : fs::directory_iterator(dir))
            if (e.is_regular_file() && e.path().filename().string().rfind(stem, 0) == 0)
                total += static_cast<std::uint64_t>(e.file_size());
    } catch (...) {}
    return total;
}

struct HostInfo { std::string host, cpu, os; std::int64_t cores = 0, ram_gb = 0; };

// Public-safety override: prefer a gitignored host_label.local (then $JAC313_HOST_LABEL)
// over the machine's real hostname, so a committed DB carries a stable label (e.g.
// jac313-002) from the start — no scrub needed, and grouping stays stable across re-runs.
// store_bench runs from the build dir, so the file is searched from CWD UP the tree (it
// lives at the v002/ root). Mirrors tools/jac313_test_cli/host_hardware.cpp::hostname().
std::string host_label_override() {
    namespace fs = std::filesystem;
    auto trim = [](std::string s) {
        const auto ws = [](char c) { return c == ' ' || c == '\t' || c == '\r' || c == '\n'; };
        while (!s.empty() && ws(s.front())) s.erase(s.begin());
        while (!s.empty() && ws(s.back()))  s.pop_back();
        return s;
    };
    std::error_code ec;
    for (fs::path d = fs::current_path(ec); !ec && !d.empty(); d = d.parent_path()) {
        std::ifstream in(d / "host_label.local");
        if (in) { std::string s; std::getline(in, s); s = trim(std::move(s)); if (!s.empty()) return s; }
        if (d == d.parent_path()) break;
    }
    if (const char* env = std::getenv("JAC313_HOST_LABEL")) { std::string s = trim(env); if (!s.empty()) return s; }
    return {};
}

HostInfo sense_host() {
    HostInfo h;
    h.host = host_label_override();
    if (h.host.empty()) {
        char hn[256] = {0};
        if (::gethostname(hn, sizeof(hn) - 1) == 0) h.host = hn;
    }
    std::ifstream ci("/proc/cpuinfo"); std::string line;
    while (std::getline(ci, line)) {
        if (h.cpu.empty() && line.rfind("model name", 0) == 0) {
            auto c = line.find(':');
            if (c != std::string::npos) {
                h.cpu = line.substr(c + 1);
                while (!h.cpu.empty() && h.cpu.front() == ' ') h.cpu.erase(h.cpu.begin());
                while (!h.cpu.empty() && h.cpu.back() == ' ')  h.cpu.pop_back();   // trim BOTH ends to
            }                                                                       // match the CLI's host sensing (shared group_id)
        }
        if (line.rfind("processor", 0) == 0) ++h.cores;
    }
    std::ifstream mi("/proc/meminfo");
    while (std::getline(mi, line)) {
        if (line.rfind("MemTotal", 0) == 0) {
            std::int64_t kb = 0; for (char c : line) if (c >= '0' && c <= '9') kb = kb * 10 + (c - '0');
            h.ram_gb = (kb + 512 * 1024) / (1024 * 1024); break;
        }
    }
    std::ifstream osr("/etc/os-release");
    while (std::getline(osr, line)) {
        if (line.rfind("PRETTY_NAME=", 0) == 0) {
            h.os = line.substr(12);
            if (!h.os.empty() && h.os.front() == '"') h.os.erase(h.os.begin());
            if (!h.os.empty() && h.os.back() == '"') h.os.pop_back();
            break;
        }
    }
    return h;
}

// ============================================================================
// results.db — the unified, normalized results database. The schema and the compiler/lookup/run
// helpers live in jac313_results_db.hpp (jac313::results — shared with the test CLI, single source
// of truth). The wrappers below just adapt store_bench's HostInfo to that shared API — results.db
// is the only thing store_bench writes (the legacy bench_run table is retired).
// ============================================================================
std::string host_label_for(std::int64_t gid) { return jac313::results::host_label(gid); }

std::filesystem::path results_db_path(const std::string& bench_db_path) {
    namespace fs = std::filesystem;
    const fs::path p(bench_db_path);
    return (p.has_parent_path() ? p.parent_path() : fs::path(".")) / "results.db";
}

const std::string& bench_harness_version() {
    static const std::string ver = jac313::harness_version_from_cwd();
    return ver;
}

// Shared schema + the 7 bench-suite testList names (store_bench owns those; the CLI seeds its own).
void ensure_results_schema(jac313::Qlite::v002::Sqlite& db) {
    jac313::results::ensure_schema(db);
    db.exec("INSERT OR IGNORE INTO testList(name) VALUES"
            " ('0 flags, non-durable'),('2 flags, non-durable'),('4 flags, non-durable'),"
            " ('6 flags, non-durable'),('durable jtext'),('durable sql'),('durable binary')");
}

std::int64_t results_group_id(jac313::Qlite::v002::Sqlite& db, const HostInfo& /*h*/) {
    return jac313::results::current_host(db, bench_harness_version());
}

std::int64_t results_next_run_id(jac313::Qlite::v002::Sqlite& db) {
    return jac313::results::next_run_id(db);
}

// Allocate the next results.db run_id up front (one per --suite run, shared by all its configs).
std::int64_t next_results_run_id(const std::string& bench_db_path) {
    try {
        jac313::Qlite::v002::Sqlite db(results_db_path(bench_db_path).string());
        ensure_results_schema(db);
        return results_next_run_id(db);
    } catch (...) { return 0; }
}

std::int64_t results_lookup_id(jac313::Qlite::v002::Sqlite& db, const char* sql, const std::string& name) {
    return jac313::results::lookup_id(db, sql, name);
}

std::int64_t compiler_id_for(jac313::Qlite::v002::Sqlite& db, const jac313::results::CompilerInfo& c) {
    return jac313::results::compiler_id(db, c);
}

// Insert-or-find a bench parameter combo (build_type Release, modules off; size/output_mode/batch
// N/A => NULL). SELECT-then-INSERT, because a UNIQUE over NULL-bearing columns won't dedupe in SQLite.
std::int64_t parameter_id_bench(jac313::Qlite::v002::Sqlite& db, std::int64_t compiler_id,
                                const std::string& persist, std::int64_t threads, std::int64_t events,
                                std::int64_t runs, std::int64_t flags) {
    const char* where =
        "SELECT id FROM parameter WHERE compiler_id=? AND build_type='Release' AND modules='off' AND import_std='off' "
        "AND size IS NULL AND persist=? AND output_mode IS NULL AND threads=? AND events_per_thread=? AND runs=? "
        "AND batch IS NULL AND flag_count=? AND valgrind_tool IS NULL";
    auto find = [&]() -> std::int64_t {
        std::int64_t id = 0; auto st = db.prepare(where);
        st.bind(compiler_id, persist, threads, events, runs, flags); if (st.step()) st.get(id); return id;
    };
    std::int64_t id = find();
    if (id == 0) {
        db.exec("INSERT INTO parameter(compiler_id,build_type,modules,import_std,size,persist,output_mode,threads,"
                "events_per_thread,runs,batch,flag_count) VALUES(?, 'Release','off','off', NULL, ?, NULL, ?, ?, ?, NULL, ?)",
                compiler_id, persist, threads, events, runs, flags);
        id = find();
    }
    return id;
}

// Write one normalized testRun fact (resolving run / parameter / testList / testType) into results.db.
void record_to_results_db(const Params& p, const BenchSummary& s) {
    if (p.db_path.empty()) return;
    try {
        jac313::Qlite::v002::Sqlite db(results_db_path(p.db_path).string());
        ensure_results_schema(db);
        // Read the pinned machine (set by the CLI/bootstrap). store_bench no longer self-senses the
        // host, so it can't disagree on disk/cores and split the machine into a separate group.
        const std::int64_t group_id = jac313::results::current_host(db, bench_harness_version());
        if (group_id == 0) {
            std::cerr << "[results] no current_host pinned (run a CLI gate or ./bootstrap.sh first) — "
                         "not recording bench rows.\n";
            return;
        }
        const std::int64_t run_id = (p.run_id != 0) ? p.run_id : results_next_run_id(db);
        // All three component versions come from the package_version table (pinned from the code
        // version() literals by the CLI at host-pin time) — the single source store_bench can reach for
        // jText, whose header isn't visible in this TU. Store/Qlite fall back to their in-process literal
        // if the table is somehow unpinned (same value; belt-and-suspenders), jText to empty.
        namespace R = jac313::results;
        const std::string& jver = bench_harness_version();   // 'v001'|'v002' — the world scope for package_version
        jac313::results::insert_run(db, run_id, group_id, jac313::results::host_label(group_id),
                                    jver,
                                    R::package_version_get(db, group_id, jver, "Store", std::string(jac313::Store::v002::version())),
                                    R::package_version_get(db, group_id, jver, "Qlite", std::string(jac313::Qlite::v002::version())),
                                    R::package_version_get(db, group_id, jver, "jText"));
        std::string base = p.label;
        if (const auto pos = base.find(" @10M"); pos != std::string::npos) base.erase(pos, 5);
        const std::int64_t test_list_id = results_lookup_id(db, "SELECT id FROM testList WHERE name=?", base);
        const std::int64_t test_type_id = results_lookup_id(db, "SELECT id FROM testType WHERE name=?", std::string("bench"));
        const std::int64_t flags = set_flag_count(p.flags);
        const std::int64_t comp_id = compiler_id_for(db, this_compiler());
        const std::int64_t param_id = parameter_id_bench(db, comp_id, p.persist,
            static_cast<std::int64_t>(p.threads), static_cast<std::int64_t>(p.events_per_thread),
            static_cast<std::int64_t>(p.runs), flags);
        const std::int64_t bytes = (p.persist == "none") ? 0
                                 : static_cast<std::int64_t>(measure_output_bytes(p.base_name));
        db.exec("INSERT INTO testRun(run_id, test_type_id, test_list_id, parameter_id, status, duration_ms, "
                "median_ops, low_ops, high_ops, avg_ops, stddev_ops, bytes) VALUES(?,?,?,?, NULL, NULL, ?,?,?,?,?, ?)",
                run_id, test_type_id, test_list_id, param_id,
                static_cast<std::int64_t>(s.median), static_cast<std::int64_t>(s.low),
                static_cast<std::int64_t>(s.high), static_cast<std::int64_t>(s.avg),
                static_cast<std::int64_t>(s.stddev), bytes);
        jac313::results::rebuild_safeness(db);   // refresh the safeness summary (read by report + verdict)
    } catch (const std::exception& e) {
        std::cerr << "[results] record failed: " << e.what() << "\n";
    }
}

// bench_results.db / bench_run is retired — store_bench writes only the unified results.db.
void record_to_db(const Params& p, const BenchSummary& s) {
    record_to_results_db(p, s);
}
#else
void record_to_db(const Params&, const BenchSummary&) {}   // built without SQL persist -> no DB
#endif

// Two-phase batch schedule: fine resolution where the curve actually moves (small batches), coarse
// in the flat tail — fine_step from lo up to knee, then coarse_step from knee up to hi. e.g.
// (250,1000,250,5000,500) -> 250,500,750,1000, 1500,2000,...,5000. coarse continues from the last
// fine point + coarse_step, so it stays clean even if fine_step doesn't land exactly on the knee.
inline std::vector<std::size_t> build_batch_schedule(std::size_t lo, std::size_t knee,
                                                     std::size_t fine_step, std::size_t hi,
                                                     std::size_t coarse_step) {
    std::vector<std::size_t> v;
    if (lo == 0 || fine_step == 0 || coarse_step == 0 || hi < lo) return v;
    std::size_t b = lo;
    for (; b <= knee && b <= hi; b += fine_step) v.push_back(b);
    if (!v.empty())
        for (b = v.back() + coarse_step; b <= hi; b += coarse_step) v.push_back(b);
    return v;
}

#ifdef JAC313_STORE_HAS_SQL_PERSIST
// Per-machine durable-backend calibration. For each of binary/jtext/sql, sweep the double-buffer
// record size (batch) over a schedule under the same process-level timing as everything else, keeping
// two running numbers: the best median ops/sec seen and the batch that produced it. useThis is that
// PEAK batch — the record size at the max ops/sec — NOT an average. (The old "middle of the plateau
// band" averaged the peak toward its slower neighbours, which measurably hurt the pick.) We still
// report the near-peak band low_size/high_size (every batch within tol% of the peak) for reference;
// best_size/best_ops (the raw peak) and useThis land in io_best_fit for THIS machine. The recorded
// suite then runs each durable config at useThis.
// Needs --db and a pinned current_host (./bootstrap.sh or a CLI gate).
struct CalResult { std::string type; std::size_t low, high, best, use; std::uint64_t best_ops; };

int run_calibration(Params base, const std::vector<std::size_t>& schedule, std::size_t tol_pct,
                    const std::string& sched_desc) {
    if (base.db_path.empty()) { std::cerr << "ERROR: --calibrate needs --db <results.db>\n"; return 2; }
    if (schedule.empty()) { std::cerr << "ERROR: empty calibrate schedule (check --cal-* range)\n"; return 2; }

    std::int64_t gid = 0;
    try {
        jac313::Qlite::v002::Sqlite db(results_db_path(base.db_path).string());
        ensure_results_schema(db);
        gid = jac313::results::current_host(db, bench_harness_version());
    } catch (const std::exception& e) { std::cerr << "[calibrate] DB error: " << e.what() << "\n"; return 1; }
    if (gid == 0) {
        std::cerr << "[calibrate] no current_host pinned (run ./bootstrap.sh or a CLI gate first)\n";
        return 1;
    }

    std::cout << "\n===============================================================\n";
    std::cout <<   "--- calibrating double buffer best size for this machine ---\n";
    std::cout <<   "===============================================================\n";
    std::cout << "host " << jac313::results::host_label(gid) << " (group " << gid << ")"
              << "   threads=" << base.threads
              << "  events/thread=" << bench_group_thousands(base.events_per_thread)
              << "  runs=" << base.runs
              << "  batch " << sched_desc << " (" << schedule.size() << " pts)"
              << "  (plateau = within " << tol_pct << "% of peak)\n";

    std::vector<CalResult> results;
    for (const char* ty : {"binary", "jtext", "sql"}) {
        Params p = base;
        p.persist = ty;
        p.flags = 0;
        std::cout << "\n--- " << ty << " ---\n";
        std::vector<std::uint64_t> medians;
        std::uint64_t best_median = 0; std::size_t best_batch = schedule.front();
        for (std::size_t b : schedule) {
            p.batch = b;
            auto durations = measure_set(p);
            if (durations.empty()) { std::cerr << "  batch=" << b << "  FAILED\n"; return 1; }
            const BenchSummary s = compute_bench_summary(durations,
                static_cast<std::uint64_t>(p.threads) * p.events_per_thread);
            std::cout << "  batch=" << bench_group_thousands(b) << "\t-> median "
                      << bench_group_thousands(s.median) << " ops/sec\n";
            medians.push_back(s.median);
            if (s.median > best_median) { best_median = s.median; best_batch = b; }
        }
        // Plateau = every swept batch within tol% of the peak; band edges = its lowest/highest batch.
        const std::uint64_t thresh = best_median - (best_median * tol_pct) / 100;
        std::size_t low_b = best_batch, high_b = best_batch;
        for (std::size_t i = 0; i < schedule.size(); ++i)
            if (medians[i] >= thresh) { low_b = std::min(low_b, schedule[i]); high_b = std::max(high_b, schedule[i]); }
        // useThis = the batch at the PEAK median (the max records @ max ops/sec tracked above) — the
        // honest measured best, not a band average. low_b/high_b stay as the near-peak band for reference.
        const std::size_t use = best_batch;
        std::cout << "  band [" << bench_group_thousands(low_b) << ".." << bench_group_thousands(high_b)
                  << "]  peak " << bench_group_thousands(best_median) << " @ batch "
                  << bench_group_thousands(best_batch) << "  -> useThis " << bench_group_thousands(use) << "\n";
        results.push_back({ty, low_b, high_b, best_batch, use, best_median});
        try {
            jac313::Qlite::v002::Sqlite db(results_db_path(base.db_path).string());
            ensure_results_schema(db);
            jac313::results::upsert_io_best_fit(db, gid, ty,
                static_cast<std::int64_t>(low_b), static_cast<std::int64_t>(high_b),
                static_cast<std::int64_t>(best_batch), static_cast<std::int64_t>(best_median),
                static_cast<std::int64_t>(use));
        } catch (const std::exception& e) { std::cerr << "[calibrate] record failed: " << e.what() << "\n"; return 1; }
    }

    std::cout << "\n===============================================================\n";
    std::cout <<   "--- calibration summary: " << jac313::results::host_label(gid)
              << " (group " << gid << ") ---\n";
    std::cout <<   "===============================================================\n";
    // Right-justify every number to its column's widest value so the columns line up.
    auto rjust = [](const std::string& s, std::size_t w) {
        return s.size() < w ? std::string(w - s.size(), ' ') + s : s;
    };
    std::size_t w_type = 0, w_use = 0, w_low = 0, w_high = 0, w_ops = 0, w_best = 0;
    for (const auto& r : results) {
        w_type = std::max(w_type, r.type.size());
        w_use  = std::max(w_use,  bench_group_thousands(r.use).size());
        w_low  = std::max(w_low,  bench_group_thousands(r.low).size());
        w_high = std::max(w_high, bench_group_thousands(r.high).size());
        w_ops  = std::max(w_ops,  bench_group_thousands(r.best_ops).size());
        w_best = std::max(w_best, bench_group_thousands(r.best).size());
    }
    for (const auto& r : results)
        std::cout << "  " << r.type << std::string(w_type - r.type.size(), ' ')
                  << "   useThis= " << rjust(bench_group_thousands(r.use), w_use)
                  << "   band[ "    << rjust(bench_group_thousands(r.low), w_low)
                  << " .. "         << rjust(bench_group_thousands(r.high), w_high) << " ]"
                  << "   peak "     << rjust(bench_group_thousands(r.best_ops), w_ops)
                  << " @ "          << rjust(bench_group_thousands(r.best), w_best) << "\n";
    std::cout << "io_best_fit updated.\n";
    return 0;
}

// The calibrated batch (useThis) for a durable backend on this machine, or fallback if not calibrated.
std::int64_t calibrated_use(const std::string& db_path, const std::string& type, std::int64_t fallback) {
    if (db_path.empty()) return fallback;
    try {
        jac313::Qlite::v002::Sqlite db(results_db_path(db_path).string());
        ensure_results_schema(db);
        const std::int64_t gid = jac313::results::current_host(db, bench_harness_version());
        if (gid == 0) return fallback;
        return jac313::results::io_best_fit_use(db, gid, type, fallback);
    } catch (...) { return fallback; }
}
#else
int run_calibration(Params, const std::vector<std::size_t>&, std::size_t, const std::string&) {
    std::cerr << "ERROR: --calibrate needs the SQL-persist build (JAC313_STORE_HAS_SQL_PERSIST)\n"; return 2;
}
std::int64_t calibrated_use(const std::string&, const std::string&, std::int64_t fb) { return fb; }
#endif

// Two scales of the SAME 10 configs:
//   full  (--suite)         — the big numbers; the recorded throughput benchmark (Release).
//   smoke (--suite --smoke) — the same 10 capped to <=10k events, run as a CORRECTNESS GATE:
//                             pass iff ALL configs run clean (incl. structural verify); the
//                             numbers are meaningless at this scale, so nothing is recorded.
int run_suite(Params base, bool dry, bool smoke) {
    base.threads = 50;                                 // curated suite is opinionated about threads
    struct Cfg { std::string persist; int flags; std::string label; std::size_t evt, runs; };
    std::vector<Cfg> cfgs;
    for (int n : {0, 2, 4, 6})
        cfgs.push_back({"none", n, std::to_string(n) + " flags, non-durable", 200000, 10});
    for (const char* pn : {"jtext", "sql", "binary"})
        cfgs.push_back({std::string(pn), 0, std::string("durable ") + pn, 20000, 3});          // 1M × 3
    for (const char* pn : {"jtext", "sql", "binary"})
        cfgs.push_back({std::string(pn), 0, std::string("durable ") + pn + " @10M", 200000, 3}); // 10M × 3 (scaling)

    if (smoke) {
        const std::size_t cap = base.threads ? 10000 / base.threads : 10000;  // 200 -> 10k total / config
        for (auto& cf : cfgs) {
            cf.evt = std::min<std::size_t>(cf.evt, cap == 0 ? 1 : cap);
            cf.runs = std::min<std::size_t>(cf.runs, 2);
            cf.label += " (smoke)";
        }
    }

    if (dry) {
        std::cout << "# Store benchmark suite — " << cfgs.size() << " configs"
                  << (smoke ? " [SMOKE gate: <=10k events, pass/fail, not recorded]" : "")
                  << " (run from the build dir)\n\n";
        for (const auto& cf : cfgs) {
            std::cout << "./jac313_store_bench --threads " << base.threads << " --events-per-thread " << cf.evt
                      << " --runs " << cf.runs << " --persist " << cf.persist;
            if (cf.persist == "none") std::cout << " --flag-count " << cf.flags;
            std::cout << "   # " << cf.label << "\n";
        }
        return 0;
    }

    if (smoke) {
        // Correctness gate: run every config, fail the whole gate if ANY config fails.
        std::size_t passed = 0;
        std::vector<std::string> failed;
        for (const auto& cf : cfgs) {
            Params p = base;
            p.persist = cf.persist; p.flags = flags_by_count(cf.flags); p.label = cf.label;
            p.events_per_thread = cf.evt; p.runs = cf.runs;
            std::cout << "\n=== " << cf.label << " ===\n";
            if (measure_set(p).empty()) { std::cerr << "FAIL: " << cf.label << "\n"; failed.push_back(cf.label); }
            else                        { ++passed;  std::cout << "PASS: " << cf.label << "\n"; }
        }
        std::cout << "\n=== smoke summary ===\nPassed: " << passed << "/" << cfgs.size() << "\n";
        if (!failed.empty()) {
            std::cout << "Failed: " << failed.size() << "\n";
            for (const auto& f : failed) std::cout << "  - " << f << "\n";
            std::cout << "smoke FAIL\n";
            return 1;
        }
        std::cout << "smoke PASS (all " << cfgs.size() << " configs clean; correctness only, not recorded)\n";
        return 0;
    }

    // full: the recorded throughput benchmark. Allocate ONE results.db run_id up front so every
    // config in this suite shares it (that's what "ran together" means).
    if (!base.db_path.empty()) base.run_id = next_results_run_id(base.db_path);
    // Announce the calibrated double-buffer sizes (io_best_fit.useThis) this run will use per durable
    // backend — so the recorded numbers are tied to a visible batch (falls back to the default if a
    // backend isn't calibrated yet; run --calibrate to populate io_best_fit).
    std::cout << "durable batch (io_best_fit.useThis):";
    for (const char* ty : {"jtext", "sql", "binary"})
        std::cout << "  " << ty << "=" << bench_group_thousands(
            static_cast<std::uint64_t>(calibrated_use(base.db_path, ty, static_cast<std::int64_t>(base.batch))));
    std::cout << std::endl;   // flush: surface the chosen batches promptly (before the long runs)
    for (const auto& cf : cfgs) {
        Params p = base;
        p.persist = cf.persist; p.flags = flags_by_count(cf.flags); p.label = cf.label;
        p.events_per_thread = cf.evt; p.runs = cf.runs;
        // Durable configs run at this machine's calibrated double-buffer size (io_best_fit.useThis);
        // falls back to the default batch when uncalibrated. Non-durable has no writer, so batch is moot.
        if (cf.persist != "none") p.batch = static_cast<std::size_t>(calibrated_use(base.db_path, cf.persist, static_cast<std::int64_t>(base.batch)));
        std::cout << "\n=== " << cf.label << " ===";
        if (cf.persist != "none") std::cout << "  (batch " << bench_group_thousands(p.batch) << ")";
        std::cout << "\n";
        auto durations = measure_set(p);
        if (durations.empty()) { std::cerr << "FAILED: " << cf.label << "\n"; return 1; }
        const std::uint64_t total = static_cast<std::uint64_t>(p.threads) * p.events_per_thread;
        emit_bench_stats(durations, total);
        record_to_db(p, compute_bench_summary(durations, total));
    }
    std::cout << "\n\n";   // two blank lines after the bench suite, for visual separation
    return 0;
}


} // namespace

int main(int argc, char** argv) {
    Params p;
    p.flags = default_flags();
    std::optional<Sweep> sweep;
    bool do_suite = false, dry = false, smoke = false;
    bool single_run = false, verify_flag = false;   // worker mode (internal: re-exec by run_one)
    bool do_calibrate = false;                       // --calibrate: per-machine batch best-fit -> io_best_fit
    // Two-phase sweep: fine (cal_step) from cal_lo up to cal_knee, then coarse (cal_step2) up to cal_hi.
    // Default: 250,500,750,1000, then 1500..5000:500 — fine where the curve moves, coarse in the flat tail.
    std::size_t cal_lo = 250, cal_knee = 1000, cal_step = 250, cal_step2 = 500, cal_hi = 5000;
    std::size_t cal_tol = 3;                          // plateau = within cal_tol% of peak (band -> middle = useThis)
    bool threads_set = false, runs_set = false;      // so --calibrate can apply its own defaults

    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        auto next = [&]() -> std::string { return (i + 1 < argc) ? argv[++i] : std::string{}; };
        if (a == "--threads") { p.threads = std::stoull(next()); threads_set = true; }
        else if (a == "--events-per-thread") p.events_per_thread = std::stoull(next());
        else if (a == "--runs") { p.runs = std::stoull(next()); runs_set = true; }
        else if (a == "--batch") p.batch = std::stoull(next());
        else if (a == "--persist") p.persist = next();
        else if (a == "--base-name") p.base_name = next();
        else if (a == "--flags") p.flags = parse_flags(next());
        else if (a == "--flags-raw") p.flags = std::stoull(next());   // exact mask (worker re-exec)
        else if (a == "--flag-count") p.flags = flags_by_count(std::stoull(next()));
        else if (a == "--single-run") single_run = true;              // worker mode (one workload, no stats/DB)
        else if (a == "--verify") verify_flag = true;                 // worker: structural-verify this run
        else if (a == "--db") p.db_path = next();
        else if (a == "--label") p.label = next();
        else if (a == "--suite") do_suite = true;
        else if (a == "--smoke") { do_suite = true; smoke = true; }   // smoke = the suite as a pass/fail gate
        else if (a == "--calibrate") do_calibrate = true;            // sweep batch per durable backend -> io_best_fit
        else if (a == "--cal-lo")    cal_lo = std::stoull(next());
        else if (a == "--cal-knee")  cal_knee = std::stoull(next());     // fine->coarse transition
        else if (a == "--cal-hi")    cal_hi = std::stoull(next());
        else if (a == "--cal-step")  cal_step = std::stoull(next());     // fine step (lo..knee)
        else if (a == "--cal-step2") cal_step2 = std::stoull(next());    // coarse step (knee..hi)
        else if (a == "--cal-tol")   cal_tol = std::stoull(next());
        else if (a == "--dry-run") dry = true;
        else if (a == "--sweep") { sweep = parse_sweep(next()); if (!sweep) { std::cerr << "bad --sweep (want AXIS=LO..HI:STEP)\n"; return 2; } }
        else if (!arg_value(a, "--sweep").empty()) { sweep = parse_sweep(arg_value(a, "--sweep")); }
        else if (a.rfind("--", 0) == 0) {
            // Reject unknown flags instead of silently falling through to the default
            // single run (which would record a junk row to --db, e.g. on a typo or a
            // retired flag like the old --clear-all).
            std::cerr << "unknown option: " << a << "\n"; return 2;
        }
    }

    // Worker mode: do exactly one workload and exit. The parent (run_one) times this whole
    // process from fork to exit, so nothing is printed and nothing is recorded here.
    if (single_run) return do_workload(p, verify_flag) ? 0 : 1;

    if (do_calibrate) {
        // Calibration defaults to the durable 1M config's shape (50 threads, 1M events, 3 runs)
        // unless the user overrode them — so the best-fit batch is found at the scale it's used.
        if (!threads_set) p.threads = 50;
        if (!runs_set)    p.runs = 3;
        const auto schedule = build_batch_schedule(cal_lo, cal_knee, cal_step, cal_hi, cal_step2);
        std::string desc = bench_group_thousands(cal_lo) + ".." + bench_group_thousands(cal_knee)
                         + ":" + bench_group_thousands(cal_step) + ", then .." + bench_group_thousands(cal_hi)
                         + ":" + bench_group_thousands(cal_step2);
        return run_calibration(p, schedule, cal_tol, desc);
    }

    if (do_suite)  return run_suite(p, dry, smoke);

    if (!sweep) {
        // single config — one measurement set
        std::cout << "store_bench: threads=" << p.threads << " events/thread=" << p.events_per_thread
                  << " runs=" << p.runs << " persist=" << p.persist << " batch=" << p.batch << "\n";
        auto durations = measure_set(p);
        if (durations.empty()) { std::cerr << "benchmark failed\n"; return 1; }
        emit_bench_stats(durations, static_cast<std::uint64_t>(p.threads * p.events_per_thread));
        record_to_db(p, compute_bench_summary(durations, static_cast<std::uint64_t>(p.threads * p.events_per_thread)));
        return 0;
    }

    // sweep — find the per-machine peak along the chosen axis
    std::cout << "store_bench sweep: axis=" << sweep->axis
              << " range=" << sweep->lo << ".." << sweep->hi << ":" << sweep->step
              << "  (threads=" << p.threads << " events/thread=" << p.events_per_thread
              << " runs=" << p.runs << " persist=" << p.persist << " batch=" << p.batch << ")\n";
    std::cout << "----------------------------------------------------------------\n";
    std::uint64_t best_median = 0, best_high = 0, best_stddev = 0;
    std::size_t best_val = 0;
    for (std::size_t v = sweep->lo; v <= sweep->hi; v += sweep->step) {
        Params pv = p;
        apply_axis(pv, sweep->axis, v);
        auto durations = measure_set(pv);
        if (durations.empty()) { std::cerr << sweep->axis << "=" << v << "  FAILED\n"; return 1; }
        const BenchSummary s = compute_bench_summary(durations, static_cast<std::uint64_t>(pv.threads * pv.events_per_thread));
        std::cout << "  " << sweep->axis << "=" << bench_group_thousands(v)
                  << "  -> median " << bench_group_thousands(s.median) << " ops/sec"
                  << "  (high " << bench_group_thousands(s.high)
                  << ", σ " << bench_group_thousands(s.stddev) << ", N=" << s.runs << ")\n";
        if (s.median > best_median) { best_median = s.median; best_high = s.high; best_stddev = s.stddev; best_val = v; }
    }
    std::cout << "----------------------------------------------------------------\n";
    std::cout << "PEAK (this machine): " << sweep->axis << "=" << bench_group_thousands(best_val)
              << "  -> median " << bench_group_thousands(best_median) << " ops/sec"
              << "  (high " << bench_group_thousands(best_high)
              << ", σ " << bench_group_thousands(best_stddev) << ")"
              << "   [" << host_tag() << ", persist=" << p.persist << "]\n";
    // machine-parseable peak line for the runner / DB
    std::cout << "BENCHSWEEP axis=" << sweep->axis << " best_val=" << best_val
              << " median=" << best_median << " high=" << best_high << " stddev=" << best_stddev
              << " persist=" << p.persist << "\n";
    return 0;
}
