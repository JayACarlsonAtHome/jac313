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

#include "test_common.hpp"
#include "bench_stats.hpp"

#ifdef JAC313_STORE_HAS_SQL_PERSIST
#include <unistd.h>     // gethostname
#include "jac313/Qlite/v002/Sqlite.hpp"   // jac313::Qlite::v002::Sqlite — the results DB receiver
#include "jac313_results_db.hpp"           // jac313::results — shared schema + dimension helpers
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
    std::string persist = "none";        // none | binary | jtext | sql
    std::string base_name = "bench";
    std::uint64_t flags = 0;             // per-event user flag mask
    std::string db_path;                 // --db: append one row per run to this SQLite DB (via Qlite)
    std::string label;                   // --label: human label for the config (e.g. "2 flags, non-durable")
    std::string jtext_ver;               // --jtext-ver: passed in (jText's version header isn't trivially reachable here)
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
    if (p.persist == "binary") return std::make_unique<BinaryEventSink>(p.base_name, im, dm, PersistMode::All);
    if (p.persist == "jtext")  return std::make_unique<JTextEventSink>(p.base_name, im, dm, PersistMode::All);
#ifdef JAC313_STORE_HAS_SQL_PERSIST
    if (p.persist == "sql")    return std::make_unique<SqlEventSink>(p.base_name, im, dm, PersistMode::All, false);
#endif
    return nullptr;
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

// One timed run of the full workload. Attaches persistence for THIS run when durable.
std::optional<std::size_t> run_one(const Params& p, bool verify) {
    if (p.persist != "none") clean_output(p.base_name);
    // Time the FULL front edge: store construction (the threads×events buffer allocation),
    // sink setup, and payload prep are all counted now — not trimmed. Honest end-to-end.
    const auto start = high_resolution_clock::now();
    LogxStore store(p.threads, p.events_per_thread);

    if (p.persist != "none") {
        if (auto sink = make_sink(p)) {
            store.attach_persistence(std::make_unique<DoubleBufferedWriter>(std::move(sink), p.batch));
        } else {
            std::cerr << "ERROR: persist='" << p.persist << "' unavailable (SQL needs JAC313_STORE_HAS_SQL_PERSIST)\n";
            return std::nullopt;
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
    // Durable honesty: drain + flush the persistence writer (and join its worker thread)
    // INSIDE the timed region, so the durable rate counts bytes that actually reached the
    // sink — not just bytes buffered. No-op when non-durable (no writer attached).
    store.finalize_persistence();
    const auto end = high_resolution_clock::now();

    // Verification is correctness, not throughput — kept OUT of the timed region and run
    // only when asked. The benchmark verifies the LAST run as a sanity net (O(N), so we
    // don't pay it every run); full correctness lives in the small correctness tests.
    if (verify && !store.verify_level01()) {
        std::cerr << "STRUCTURAL VERIFICATION FAILED\n";
        store.diagnose_failures();
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

// Shared schema + the 7 bench-suite testList names (store_bench owns those; the CLI seeds its own).
void ensure_results_schema(jac313::Qlite::v002::Sqlite& db) {
    jac313::results::ensure_schema(db);
    db.exec("INSERT OR IGNORE INTO testList(name) VALUES"
            " ('0 flags, non-durable'),('2 flags, non-durable'),('4 flags, non-durable'),"
            " ('6 flags, non-durable'),('durable jtext'),('durable sql'),('durable binary')");
}

std::int64_t results_group_id(jac313::Qlite::v002::Sqlite& db, const HostInfo& h) {
    return jac313::results::group_id(db, {h.cpu, h.cores, h.ram_gb, h.os});
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
        HostInfo h = sense_host();
        const std::int64_t group_id = results_group_id(db, h);
        if (host_label_override().empty()) h.host = host_label_for(group_id);
        const std::int64_t run_id = (p.run_id != 0) ? p.run_id : results_next_run_id(db);
        jac313::results::insert_run(db, run_id, group_id, h.host, {h.cpu, h.cores, h.ram_gb, h.os},
                                    std::string(jac313::Store::v002::version()),
                                    std::string(jac313::Qlite::v002::version()), p.jtext_ver);
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
    for (const auto& cf : cfgs) {
        Params p = base;
        p.persist = cf.persist; p.flags = flags_by_count(cf.flags); p.label = cf.label;
        p.events_per_thread = cf.evt; p.runs = cf.runs;
        std::cout << "\n=== " << cf.label << " ===\n";
        auto durations = measure_set(p);
        if (durations.empty()) { std::cerr << "FAILED: " << cf.label << "\n"; return 1; }
        const std::uint64_t total = static_cast<std::uint64_t>(p.threads) * p.events_per_thread;
        emit_bench_stats(durations, total);
        record_to_db(p, compute_bench_summary(durations, total));
    }
    return 0;
}


} // namespace

int main(int argc, char** argv) {
    Params p;
    p.flags = default_flags();
    std::optional<Sweep> sweep;
    bool do_suite = false, dry = false, smoke = false;

    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        auto next = [&]() -> std::string { return (i + 1 < argc) ? argv[++i] : std::string{}; };
        if (a == "--threads") p.threads = std::stoull(next());
        else if (a == "--events-per-thread") p.events_per_thread = std::stoull(next());
        else if (a == "--runs") p.runs = std::stoull(next());
        else if (a == "--batch") p.batch = std::stoull(next());
        else if (a == "--persist") p.persist = next();
        else if (a == "--base-name") p.base_name = next();
        else if (a == "--flags") p.flags = parse_flags(next());
        else if (a == "--flag-count") p.flags = flags_by_count(std::stoull(next()));
        else if (a == "--db") p.db_path = next();
        else if (a == "--label") p.label = next();
        else if (a == "--jtext-ver") p.jtext_ver = next();
        else if (a == "--suite") do_suite = true;
        else if (a == "--smoke") { do_suite = true; smoke = true; }   // smoke = the suite as a pass/fail gate
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
