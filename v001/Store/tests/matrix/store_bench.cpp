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
//
// THE PRODUCTION RUN (--persist production, or --production SPEC): what a real program does —
// three sinks at once, one worker each: the binary replay log (every event, PersistMode::All;
// binary=off drops it), a keeper file (jtext | html | json, KeeperOnly: only events flagged
// KeeperRecord) and a database (sql | pg-local | pg-remote, DatabaseOnly: only DatabaseEntry).
// Event k (= thread * events_per_thread + i) gets KeeperRecord when k % 100 < keeper_pct and
// DatabaseEntry when (k * 37 + 11) % 100 < database_pct: deterministic, so the read-back knows
// each sink's exact subset (and the two subsets overlap like independent picks would).
//   store_bench --production keeper=jtext,database=sql,keeper_pct=10,database_pct=10,binary=on
// Three numbers per run: the hot path (producers' wall time until they join, before finalize),
// each sink's drain time (Store::drain_time_us: attach -> that sink's finalize done) and the
// durable total (the whole process, as for every config). Each sink attaches at its own batch: this
// machine's calibrated io_best_fit.useThis for its type (binary, the keeper's, the database's), the
// default batch where a type is not calibrated; --batch N gives all three N.

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
#include "jac313_summary_part.hpp"   // the summary parts line (--summary-parts)

#ifdef JAC313_STORE_HAS_SQL_PERSIST
#include <unistd.h>     // gethostname
#include "jac313/Qlite/v001/Sqlite.hpp"   // jac313::Qlite::v001::Sqlite — the results DB receiver
#include "jac313_results_db.hpp"           // jac313::results — shared schema + dimension helpers
#include "jac313_harness_version.hpp"

#include <string_view>

// The contract evaluation mode this binary was built under (JAC313_CONTRACTS, defined by the CMake
// tree; a build outside it is enforce, the default). Recorded with every bench row.
#ifdef JAC313_CONTRACTS_MODE
inline constexpr std::string_view contracts_mode = JAC313_CONTRACTS_MODE;
#else
inline constexpr std::string_view contracts_mode = "enforce";
#endif

// How this binary was built, from the CMake tree (CMakeLists.txt, beside JAC313_CONTRACTS_MODE):
// CMAKE_BUILD_TYPE, JAC313_BUILD_MODULES, JAC313_STRICT, JAC313_HARDENED. Recorded with every
// bench row; a build outside the tree records '' (unknown).
#ifdef JAC313_BUILD_TYPE
inline constexpr std::string_view kBuildType = JAC313_BUILD_TYPE;
#else
inline constexpr std::string_view kBuildType = "";
#endif
#ifdef JAC313_BUILD_MODULES
inline constexpr std::string_view kBuildModules = JAC313_BUILD_MODULES;
#else
inline constexpr std::string_view kBuildModules = "";
#endif
#ifdef JAC313_STRICT
inline constexpr std::string_view kBuildStrict = JAC313_STRICT;
#else
inline constexpr std::string_view kBuildStrict = "";
#endif
#ifdef JAC313_HARDENED
inline constexpr std::string_view kBuildHardened = JAC313_HARDENED;
#else
inline constexpr std::string_view kBuildHardened = "";
#endif
#endif

using namespace jac313::Store::v001;
using namespace std::chrono;

using LogConfig = ts_store_config<true, 6, 20, 43, 0, 0, false>;
using LogxStore = ts_store<LogConfig>;

namespace {

// ---- parameters (runtime) ----
struct Params {
    std::size_t threads = 16;
    std::size_t events_per_thread = 20'000;
    std::size_t runs = 20;
    std::size_t batch = 10'000;          // durable persistence batch: rows per sink call
    std::string persist = "none";        // none | binary | jtext | html | json | sql | pg-local | pg-remote | production
    std::string base_name = "bench";
    std::uint64_t flags = 0;             // per-event user flag mask
    std::string db_path;                 // --db: append one row per run to this SQLite DB (via Qlite)
    std::string label;                   // --label: human label for the config (e.g. "2 flags, non-durable")
    std::int64_t run_id = 0;             // runID: one per --suite run (0 = allocate one per single record)
    bool keep_logs = false;              // --keep-logs: retain durable artifacts (skip clean_output)
    std::string keep_logs_dir;           // --keep-logs-dir: output root (default: <version>/kept-logs/bench)
    std::size_t max_persist_rows = 0;    // --max-persist-rows: persist only event ids [0, N); 0 = all
    std::string readback = "off";        // --readback off|count|sample|all: one extra UNTIMED run reads back
    std::string pg_cleanup = "keep-last";// --pg-cleanup keep-last|delete (PostgreSQL test tables)
    // Suite controls forwarded by jac313_test_cli (Jac313Runner):
    std::string persist_list;            // --persist-list a,b: durable backends (empty = the default five)
    std::size_t max_runs = 0;            // --max-runs: cap each config's runs
    std::size_t max_records = 0;         // --max-records: cap events per run (threads x events/thread)
    std::string summary_parts;           // --summary-parts PATH: append one key=value line per config
    std::string last_readback;           // filled by the read-back worker run (its last READBACK line)
    std::vector<std::string> readbacks;  // every READBACK line of that run (three for production)
    std::string target_lines;            // the TARGET lines of the last timed run: what the sinks wrote where
    // --production SPEC: the production run's sinks and mix (persist = "production").
    struct Production {
        std::string keeper = "jtext";    // jtext | html | json         (KeeperOnly)
        std::string database = "sql";    // sql | pg-local | pg-remote  (DatabaseOnly)
        std::size_t keeper_pct = 10;     // % of events flagged KeeperRecord
        std::size_t database_pct = 10;   // % of events flagged DatabaseEntry
        bool binary = true;              // the binary replay log (All)
        // The batch the replay log and the keeper file attach with; the database sink's is Params::batch
        // (what parameter.batch records). 0 = Params::batch. Set by apply_production_batches.
        std::size_t batch_binary = 0, batch_keeper = 0;
        // The canonical spec (worker re-exec, dry-run line).
        std::string spec() const {
            return "keeper=" + keeper + ",database=" + database + ",keeper_pct=" + std::to_string(keeper_pct)
                 + ",database_pct=" + std::to_string(database_pct) + ",binary=" + (binary ? "on" : "off");
        }
        // results.db parameter.persist: the pair ("jtext+sql"; the binary log is parameter.binary_log).
        std::string persist_label() const { return keeper + "+" + database; }
        // The suite / testList name: "production jtext+sql 10/10".
        std::string config_name() const {
            return "production " + keeper + "+" + database + " " + std::to_string(keeper_pct) + "/"
                 + std::to_string(database_pct) + (binary ? "" : " no binary");
        }
    } prod;
};

// Parse "keeper=jtext,database=sql,keeper_pct=10,database_pct=10,binary=on" (any subset, any order;
// the rest keep their defaults) into `out`. Returns "" or what is wrong.
std::string parse_production(const std::string& spec, Params::Production& out) {
    std::string item;
    std::string err;
    auto pct = [&](const std::string& k, const std::string& v, std::size_t& dst) {
        try {
            std::size_t used = 0;
            const unsigned long n = std::stoul(v, &used);
            if (used == v.size() && n <= 100) { dst = n; return; }
        } catch (...) {}
        err = k + " must be 0..100, not '" + v + "'";
    };
    auto apply = [&](const std::string& kv) {
        if (kv.empty() || !err.empty()) return;
        const auto eq = kv.find('=');
        const std::string k = kv.substr(0, eq), v = eq == std::string::npos ? std::string() : kv.substr(eq + 1);
        if (k == "keeper") {
            if (v == "jtext" || v == "html" || v == "json") out.keeper = v;
            else err = "keeper must be jtext | html | json, not '" + v + "'";
        } else if (k == "database") {
            if (v == "sql" || v == "pg-local" || v == "pg-remote") out.database = v;
            else err = "database must be sql | pg-local | pg-remote, not '" + v + "'";
        } else if (k == "keeper_pct") {
            pct(k, v, out.keeper_pct);
        } else if (k == "database_pct") {
            pct(k, v, out.database_pct);
        } else if (k == "binary") {
            if (v == "on" || v == "true" || v == "1") out.binary = true;
            else if (v == "off" || v == "false" || v == "0") out.binary = false;
            else err = "binary must be on | off, not '" + v + "'";
        } else {
            err = "unknown production key '" + k + "' (keeper, database, keeper_pct, database_pct, binary)";
        }
    };
    for (char c : spec + ",") {
        if (c == ',') { apply(item); item.clear(); }
        else if (c != ' ') item += c;
    }
    return err;
}

// One sink of a run: the backend, its output base, and the events it takes.
struct SinkPlan {
    std::string backend;
    std::string base;
    PersistMode mode = PersistMode::All;
    std::string role;    // "" (a one-sink config) | binary | keeper | database
    std::size_t batch = 0;   // the persistence batch it attaches with (rows per sink call)
};

// The sinks a config attaches, in attach order: none, one (mode All), or the production three.
std::vector<SinkPlan> sink_plan(const Params& p) {
    if (p.persist == "none") return {};
    if (p.persist != "production") return {{p.persist, p.base_name, PersistMode::All, "", p.batch}};
    auto or_db = [&](std::size_t b) { return b ? b : p.batch; };
    std::vector<SinkPlan> v;
    if (p.prod.binary)
        v.push_back({"binary", p.base_name + "_replay", PersistMode::All, "binary", or_db(p.prod.batch_binary)});
    v.push_back({p.prod.keeper, p.base_name + "_keeper", PersistMode::KeeperOnly, "keeper", or_db(p.prod.batch_keeper)});
    v.push_back({p.prod.database, p.base_name + "_database", PersistMode::DatabaseOnly, "database", p.batch});
    return v;
}

// The production run's batch per sink, machine-readable: "batch_binary=4000 batch_keeper=3000
// batch_database=2000" (0 for a sink not attached: binary=off). The PRODTIMES, PRODUCTION and start
// lines all carry it, so what the worker attached with and what testRun records can be compared.
std::string production_batch_fields(const Params& p) {
    std::size_t b[3] = {0, 0, 0};
    for (const auto& sp : sink_plan(p))
        b[sp.role == "binary" ? 0 : sp.role == "keeper" ? 1 : 2] = sp.batch;
    return "batch_binary=" + std::to_string(b[0]) + " batch_keeper=" + std::to_string(b[1])
         + " batch_database=" + std::to_string(b[2]);
}

// The production run's per-run timings, reported by the worker on its PRODTIMES line.
struct ProdTimes {
    std::uint64_t hot_us = 0;        // producers start -> all joined (before finalize)
    std::uint64_t binary_us = 0;     // drain_time_us of each sink (0 when not attached)
    std::uint64_t keeper_us = 0;
    std::uint64_t database_us = 0;
};

// Medians over a config's runs (what results.db and the report hold).
struct ProdSummary {
    std::uint64_t hot_ops = 0;
    std::uint64_t drain_binary_ms = 0, drain_keeper_ms = 0, drain_database_ms = 0;
};

bool keep_logs_enabled(const Params& p) {
    if (p.keep_logs) return true;
    if (const char* e = std::getenv("JAC313_KEEP_LOGS")) {
        if (!e[0]) return false;
        return e[0] == '1' || e[0] == 'y' || e[0] == 'Y' || e[0] == 't' || e[0] == 'T';
    }
    return false;
}

std::filesystem::path find_version_root() {
    namespace fs = std::filesystem;
    std::error_code ec;
    for (fs::path d = fs::current_path(ec); !ec && !d.empty(); d = d.parent_path()) {
        if (fs::exists(d / "Store") && fs::exists(d / "CMakeLists.txt")) return d;
        if (d == d.parent_path()) break;
    }
    return fs::current_path(ec);
}

std::string resolved_keep_logs_dir(const Params& p) {
    if (!p.keep_logs_dir.empty()) return p.keep_logs_dir;
    return (find_version_root() / "kept-logs" / "bench" / "local").string();
}

// Match jac313_test_cli kept_logs.hpp: <persist>_1M|10M/capture under --keep-logs-dir.
std::string bench_option_dir(const std::string& persist, const std::string& label) {
    return persist + "_" + (label.find("@10M") != std::string::npos ? "10M" : "1M");
}

std::string suite_persist_base(const Params& base, const std::string& persist, const std::string& label) {
    return resolved_keep_logs_dir(base) + "/" + bench_option_dir(persist, label) + "/capture";
}

void ensure_capture_parent_dir(const std::string& base_name) {
    namespace fs = std::filesystem;
    std::error_code ec;
    fs::path bp(base_name);
    if (bp.has_parent_path()) fs::create_directories(bp.parent_path(), ec);
}

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
bool do_workload(const Params& p, bool verify, bool readback = false) {
    LogxStore store(p.threads, p.events_per_thread);
    store.set_max_persist_rows(p.max_persist_rows);

    const std::vector<SinkPlan> plan = sink_plan(p);
    for (const auto& sp : plan) {
        ensure_capture_parent_dir(sp.base);
        if (auto sink = make_persistence_sink(sp.backend, sp.base, LogConfig::the_IntMetrics,
                                              LogConfig::the_DblMetrics, sp.mode,
                                              jtext_row_limits_for<LogConfig>())) {
            store.attach_persistence(std::move(sink), sp.batch);
        } else {
            std::cerr << "ERROR: persist='" << sp.backend << "' unavailable (SQL needs JAC313_STORE_HAS_SQL_PERSIST)\n";
            return false;
        }
    }
    // The production mix: KeeperRecord / DatabaseEntry by event index (see the file header).
    const bool production = p.persist == "production";
    const std::uint64_t keeper_bit = set_user_flag(std::uint64_t{0}, StoreFlags::UserFlag::KeeperRecord);
    const std::uint64_t database_bit = set_user_flag(std::uint64_t{0}, StoreFlags::UserFlag::DatabaseEntry);
    const std::uint64_t base_flags = production ? (p.flags & ~(keeper_bit | database_bit)) : p.flags;

    std::array<std::string, 8> pre_payloads;
    for (std::size_t i = 0; i < 8; ++i) pre_payloads[i] = std::string(LogxStore::test_messages[i]);
    std::array<std::string, 5> pre_cats;
    for (std::size_t i = 0; i < 5; ++i) pre_cats[i] = std::string(LogxStore::categories[i]);

    const auto hot_start = steady_clock::now();
    std::vector<std::thread> ths;
    ths.reserve(p.threads);
    for (std::size_t t = 0; t < p.threads; ++t) {
        ths.emplace_back([&, t]() {
            std::array<std::int64_t, LogConfig::the_IntMetrics> ints{};
            std::array<double, LogConfig::the_DblMetrics> dbls{};
            for (std::size_t i = 0; i < p.events_per_thread; ++i) {
                std::string payload = pre_payloads[i % pre_payloads.size()];
                std::string cat = pre_cats[t % pre_cats.size()];
                std::uint64_t f = base_flags;
                if (production) {
                    const std::size_t k = t * p.events_per_thread + i;
                    if (k % 100 < p.prod.keeper_pct) f |= keeper_bit;
                    if ((k * 37 + 11) % 100 < p.prod.database_pct) f |= database_bit;
                }
                f = set_severity(f, static_cast<StoreFlags::Severity>(i % 8));
                auto [ok, id] = store.save_event(t, i, std::move(payload), f, std::move(cat), true, ints, dbls);
                if (!ok) { std::cerr << "CLAIM FAILED t=" << t << " i=" << i << "\n"; std::abort(); }
            }
        });
    }
    for (auto& th : ths) th.join();
    const auto hot_end = steady_clock::now();
    // Drain + flush the persistence writers (join each worker, finalize its sink). No-op when
    // non-durable. The store destructor that follows at scope exit does the rest of teardown;
    // both land inside the child's process lifetime, so both are counted.
    store.finalize_persistence();
    // What each sink wrote where (one TARGET line per table / file), for results.db run_target.
    for (std::size_t w = 0; w < plan.size(); ++w)
        print_targets(store, w, plan[w].backend, plan[w].role.empty() ? std::string("persist") : plan[w].role);
    if (production) {
        // The worker's timings for the parent (run_one reads this line from the worker's stdout).
        ProdTimes pt;
        pt.hot_us = static_cast<std::uint64_t>(duration_cast<microseconds>(hot_end - hot_start).count());
        for (std::size_t w = 0; w < plan.size(); ++w) {
            const std::uint64_t us = store.drain_time_us(w);
            if (plan[w].role == "binary") pt.binary_us = us;
            else if (plan[w].role == "keeper") pt.keeper_us = us;
            else pt.database_us = us;
        }
        std::cout << "PRODTIMES hot_us=" << pt.hot_us << " binary_us=" << pt.binary_us
                  << " keeper_us=" << pt.keeper_us << " database_us=" << pt.database_us
                  << " " << production_batch_fields(p) << std::endl;
    }

    // Verification is correctness, not throughput. It runs only when asked (the parent asks
    // on the last run as a sanity net) and, because the child exits right after, its cost
    // never lands in a recorded timing — only failing/non-failing matters.
    if (verify && !store.verify_level01()) {
        std::cerr << "STRUCTURAL VERIFICATION FAILED\n";
        store.diagnose_failures();
        return false;
    }
    // Read-back (the untimed --readback-run worker only): compare each sink's dataset with the
    // in-memory rows it was given (the production keeper / database sinks: only their flagged
    // subset, and nothing else) and print one READBACK line per sink (tests/matrix/readback.hpp).
    bool all_ok = true;
    if (readback && p.readback != "off") for (const auto& sp : plan) {
        const auto rep = run_readback(store, sp.backend, sp.base, LogConfig::the_IntMetrics,
                                      LogConfig::the_DblMetrics, p.readback, p.pg_cleanup, sp.mode);
        std::string extra;
#ifdef JAC313_STORE_HAS_PG_PERSIST
        if (is_pg_backend(sp.backend) && g_pg_timings) {
            auto b = g_pg_timings->batch_ms;
            std::sort(b.begin(), b.end());
            extra = std::format(" connect_ms={:.3f} ping_ms={:.3f} batches={} batch_med_ms={:.3f} write_ms={:.3f}",
                                g_pg_timings->connect_ms, g_pg_timings->ping_median_ms, b.size(),
                                b.empty() ? 0.0 : b[b.size() / 2], g_pg_timings->write_total_ms);
        }
#endif
        print_readback(rep, extra);
        all_ok = all_ok && rep.ok;
    }
    return all_ok;
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
        "--max-persist-rows",  std::to_string(p.max_persist_rows),
        "--readback",          p.readback,
        "--pg-cleanup",        p.pg_cleanup,
    };
    if (p.persist == "production") {
        a.push_back("--production"); a.push_back(p.prod.spec());
        // the replay log's and the keeper's own batches (--batch above is the database sink's)
        a.push_back("--batch-binary"); a.push_back(std::to_string(p.prod.batch_binary));
        a.push_back("--batch-keeper"); a.push_back(std::to_string(p.prod.batch_keeper));
    }
    if (verify) a.push_back("--verify");
    if (keep_logs_enabled(p)) {
        a.push_back("--keep-logs");
        if (!p.keep_logs_dir.empty()) {
            a.push_back("--keep-logs-dir");
            a.push_back(p.keep_logs_dir);
        }
    }
    return a;
}

// One timed run = the FULL lifetime of a fresh child process doing exactly one workload:
// fork+exec -> dynamic link -> static init -> construct -> work -> finalize -> destructor
// teardown -> exit. The parent's monotonic clock spans that whole arc, so the recorded
// number is what a user actually sees when they launch the program ("time ./prog"), not
// just the hot loop. Returns microseconds, or nullopt if the worker did not exit clean.
// A production worker also reports its hot-path and drain times (PRODTIMES line) into `pt`;
// its stdout is a pipe instead of /dev/null for that one line. The last (verify) run's stdout is a
// pipe too: its TARGET lines (what the sinks wrote where) go to `targets`.
std::optional<std::size_t> run_one(const Params& p, bool verify, ProdTimes* pt = nullptr,
                                   std::string* targets = nullptr) {
    static const std::string exe = self_exe();
    if (exe.empty()) { std::cerr << "ERROR: cannot resolve /proc/self/exe for worker spawn\n"; return std::nullopt; }

    // Clean prior output in the PARENT so the child's measured lifetime is pure workload
    // (a real program doesn't delete leftovers first). Skipped when --keep-logs retains artifacts.
    if (p.persist != "none" && !keep_logs_enabled(p)) clean_output(p.base_name);

    const std::vector<std::string> sargs = worker_argv(exe, p, verify);
    std::vector<char*> argv;
    argv.reserve(sargs.size() + 1);
    for (const auto& s : sargs) argv.push_back(const_cast<char*>(s.c_str()));
    argv.push_back(nullptr);

    const bool want_times = pt != nullptr && p.persist == "production";
    const bool want_out = want_times || (targets != nullptr && p.persist != "none");
    int fds[2] = {-1, -1};
    if (want_out && ::pipe(fds) != 0) { std::cerr << "ERROR: pipe failed for worker spawn\n"; return std::nullopt; }
    std::cout.flush();
    const auto start = steady_clock::now();
    const pid_t pid = ::fork();
    if (pid < 0) { std::cerr << "ERROR: fork failed for worker spawn\n"; return std::nullopt; }
    if (pid == 0) {
        // Worker is silent on stdout (the parent owns all reporting); stderr stays for real errors.
        // A production worker's stdout is the pipe: its one PRODTIMES line comes back that way.
        if (want_out) {
            ::dup2(fds[1], STDOUT_FILENO);
            ::close(fds[0]); ::close(fds[1]);
        } else {
            const int devnull = ::open("/dev/null", O_WRONLY);
            if (devnull >= 0) { ::dup2(devnull, STDOUT_FILENO); ::close(devnull); }
        }
        ::execv(exe.c_str(), argv.data());
        std::perror("execv");          // only reached if exec itself failed
        _exit(127);
    }
    std::string out;
    if (want_out) {
        ::close(fds[1]);
        char buf[4096];
        for (ssize_t n; (n = ::read(fds[0], buf, sizeof buf)) > 0;) out.append(buf, static_cast<std::size_t>(n));
        ::close(fds[0]);
    }
    int status = 0;
    while (::waitpid(pid, &status, 0) < 0) { /* retry on EINTR */ }
    const auto end = steady_clock::now();

    if (!(WIFEXITED(status) && WEXITSTATUS(status) == 0)) {
        std::cerr << "worker FAILED (config '" << p.label << "', persist=" << p.persist << ")\n";
        return std::nullopt;
    }
    if (targets != nullptr) {
        targets->clear();
        for (std::size_t at = 0; (at = out.find("TARGET ", at)) != std::string::npos;) {
            const auto nl = out.find('\n', at);
            if (at == 0 || out[at - 1] == '\n') targets->append(out, at, nl == std::string::npos ? std::string::npos : nl + 1 - at);
            at = nl == std::string::npos ? out.size() : nl + 1;
        }
    }
    if (want_times) {
        const auto at = out.rfind("PRODTIMES ");
        if (at == std::string::npos) { std::cerr << "worker gave no PRODTIMES line (config '" << p.label << "')\n"; return std::nullopt; }
        auto field = [&](const char* key) -> std::uint64_t {
            const std::string k = std::string(" ") + key + "=";
            const auto f = out.find(k, at);
            return f == std::string::npos ? 0 : std::stoull(out.substr(f + k.size()));
        };
        pt->hot_us = field("hot_us");
        pt->binary_us = field("binary_us");
        pt->keeper_us = field("keeper_us");
        pt->database_us = field("database_us");
        // The worker attached each sink at the batch the parent planned (and testRun records).
        const std::string want = production_batch_fields(p);
        if (out.find(want, at) == std::string::npos) {
            std::cerr << "worker batches differ from the plan (" << want << ", config '" << p.label << "')\n";
            return std::nullopt;
        }
    }
    return static_cast<std::size_t>(duration_cast<microseconds>(end - start).count());
}

// The untimed read-back run: a worker that does the same workload, then reads the dataset
// back and prints its READBACK line (stdout passes through). Returns false on failure.
bool run_readback_worker(Params& p) {
    static const std::string exe = self_exe();
    if (exe.empty()) return false;
    if (!keep_logs_enabled(p)) clean_output(p.base_name);
    std::vector<std::string> sargs = worker_argv(exe, p, /*verify=*/ false);
    sargs.push_back("--readback-run");
    std::vector<char*> argv;
    for (const auto& s : sargs) argv.push_back(const_cast<char*>(s.c_str()));
    argv.push_back(nullptr);
    std::cout.flush();
    int fds[2];
    if (::pipe(fds) != 0) return false;
    const pid_t pid = ::fork();
    if (pid < 0) return false;
    if (pid == 0) {
        ::dup2(fds[1], STDOUT_FILENO);
        ::close(fds[0]); ::close(fds[1]);
        ::execv(exe.c_str(), argv.data());
        std::perror("execv");
        _exit(127);
    }
    ::close(fds[1]);
    std::string out;
    char buf[4096];
    for (ssize_t n; (n = ::read(fds[0], buf, sizeof buf)) > 0;) out.append(buf, static_cast<std::size_t>(n));
    ::close(fds[0]);
    // Pass the READBACK lines through (one per sink); the PRODTIMES line stays internal.
    p.readbacks.clear();
    for (std::size_t at = 0; at < out.size();) {
        const auto nl = out.find('\n', at);
        const std::string line = out.substr(at, nl == std::string::npos ? std::string::npos : nl - at);
        at = nl == std::string::npos ? out.size() : nl + 1;
        if (line.rfind("PRODTIMES ", 0) == 0 || line.rfind("TARGET ", 0) == 0) continue;
        std::cout << line << "\n";
        if (line.rfind("READBACK ", 0) == 0) p.readbacks.push_back(line.substr(9));
    }
    if (!p.readbacks.empty()) p.last_readback = p.readbacks.back();
    int status = 0;
    while (::waitpid(pid, &status, 0) < 0) { /* retry on EINTR */ }
    if (!(WIFEXITED(status) && WEXITSTATUS(status) == 0)) {
        std::cerr << "READBACK FAILED (config '" << p.label << "', persist=" << p.persist << ")\n";
        return false;
    }
    return true;
}

// One measurement set: RUNS timed runs -> durations. Returns empty on any failure. A production
// config also fills `prod` with each run's hot-path and drain times.
std::vector<std::size_t> measure_set(Params& p, std::vector<ProdTimes>* prod = nullptr) {
    std::vector<std::size_t> durations;
    durations.reserve(p.runs);
    if (prod) prod->clear();
    for (std::size_t r = 0; r < p.runs; ++r) {
        ProdTimes pt;
        const bool last = r + 1 == p.runs;
        auto us = run_one(p, /*verify=*/ last, prod ? &pt : nullptr, last ? &p.target_lines : nullptr);   // verify the last run only
        if (!us) return {};
        durations.push_back(*us);
        if (prod) prod->push_back(pt);
    }
    // One extra UNTIMED run that reads the persisted dataset back (never in the durations).
    p.last_readback.clear();
    if (p.persist != "none" && p.readback != "off" && !run_readback_worker(p)) return {};
    return durations;
}

// The medians of a production config's runs; prints the hot-path and drain lines (and the
// machine-parseable PRODUCTION line) after the durable total's statistics box.
ProdSummary summarize_production(const Params& p, const std::vector<ProdTimes>& runs, std::uint64_t events) {
    auto median = [&](std::uint64_t ProdTimes::*m) -> std::uint64_t {
        std::vector<std::uint64_t> v;
        for (const auto& r : runs) v.push_back(r.*m);
        if (v.empty()) return 0;
        std::sort(v.begin(), v.end());
        return v.size() % 2 ? v[v.size() / 2] : (v[v.size() / 2 - 1] + v[v.size() / 2]) / 2;
    };
    ProdSummary s;
    std::vector<std::size_t> hot;
    for (const auto& r : runs) hot.push_back(static_cast<std::size_t>(r.hot_us));
    s.hot_ops = compute_bench_summary(hot, events).median;
    auto ms = [](std::uint64_t us) { return (us + 500) / 1000; };
    s.drain_binary_ms = ms(median(&ProdTimes::binary_us));
    s.drain_keeper_ms = ms(median(&ProdTimes::keeper_us));
    s.drain_database_ms = ms(median(&ProdTimes::database_us));
    std::cout << "  Hot path           : " << bench_group_thousands(s.hot_ops)
              << " ops/sec (median; producers until they join, before finalize)\n"
              << "  Drain (median)     : binary " << (p.prod.binary ? bench_group_thousands(s.drain_binary_ms) + " ms" : std::string("off"))
              << " · keeper " << p.prod.keeper << " " << bench_group_thousands(s.drain_keeper_ms) << " ms"
              << " · database " << p.prod.database << " " << bench_group_thousands(s.drain_database_ms) << " ms"
              << "  (attach -> that sink's finalize done)\n"
              << "  Durable total      : the statistics box above (whole process, start -> exit)\n";
    std::cout << "PRODUCTION hot_ops=" << s.hot_ops << " drain_binary_ms=" << s.drain_binary_ms
              << " drain_keeper_ms=" << s.drain_keeper_ms << " drain_database_ms=" << s.drain_database_ms
              << " keeper=" << p.prod.keeper << " database=" << p.prod.database
              << " keeper_pct=" << p.prod.keeper_pct << " database_pct=" << p.prod.database_pct
              << " binary=" << (p.prod.binary ? "on" : "off") << " " << production_batch_fields(p) << "\n";
    return s;
}

// The name of a single run given no --label, built like the suite's: what persisted (the production
// pair, "durable <backend>", or "<n> flags, non-durable") and, unless it is the suite's 1M events,
// the scale ("@10M", "@10", "@20K").
std::string generated_label(const Params& p) {
    int flags = 0;
    for (int i = 0; i < 7; ++i) if (p.flags & (1ull << i)) ++flags;
    std::string name = p.persist == "production" ? p.prod.config_name()
                     : p.persist == "none"       ? std::to_string(flags) + " flags, non-durable"
                                                 : "durable " + p.persist;
    const std::uint64_t events = static_cast<std::uint64_t>(p.threads) * p.events_per_thread;
    if (events == 1'000'000) return name;
    std::string scale = std::to_string(events);
    for (const auto& [div, suf] : {std::pair<std::uint64_t, const char*>{1'000'000, "M"}, {1'000, "K"}})
        if (events >= div && events % div == 0) { scale = std::to_string(events / div) + suf; break; }
    return name + " @" + scale;
}

void apply_axis(Params& p, const std::string& axis, std::size_t v) {
    if (axis == "events")       p.events_per_thread = v;
    else if (axis == "threads") p.threads = v;
    else if (axis == "batch")   p.batch = v;
}

// Every reason a measured config was not written to results.db (no --db, no host pin, a DB error).
// main() prints one "NOT RECORDED: ..." line per distinct reason on stdout and exits non-zero, so a
// run that recorded nothing never looks successful.
std::vector<std::string> g_not_recorded;
void not_recorded(const std::string& why) {
    std::cerr << "[results] not recorded: " << why << "\n";
    if (std::find(g_not_recorded.begin(), g_not_recorded.end(), why) == g_not_recorded.end())
        g_not_recorded.push_back(why);
}

// The repository root: the parent of the version dir (the one holding CMakeLists.txt, Store and jText)
// whose test-summary/README.md marks the shared results store. Searched up from `start`.
std::filesystem::path find_repo_root(const std::filesystem::path& start) {
    namespace fs = std::filesystem;
    std::error_code ec;
    for (fs::path d = fs::absolute(start, ec).lexically_normal(); !d.empty(); d = d.parent_path()) {
        if (fs::exists(d / "CMakeLists.txt", ec) && fs::exists(d / "Store", ec) && fs::exists(d / "jText", ec)
            && fs::exists(d.parent_path() / "test-summary" / "README.md", ec))
            return d.parent_path();
        if (d == d.parent_path()) break;
    }
    return {};
}

// --db, made safe: a relative path resolves against the repository root (found up from the working
// directory, else from this binary), never against the build dir the bench runs in. The bench writes
// <dir of --db>/results.db, so that dir must be a test-summary/ directory, and a results.db that does
// not exist yet may only be the shared one (<root>/test-summary/results.db). Returns the absolute
// results.db path, or "" with `err` set.
std::string resolve_bench_db(const std::string& given, std::string& err) {
    namespace fs = std::filesystem;
    std::error_code ec;
    fs::path root = find_repo_root(fs::current_path(ec));
    if (root.empty()) root = find_repo_root(fs::read_symlink("/proc/self/exe", ec).parent_path());
    fs::path p(given);
    if (p.is_relative()) {
        if (root.empty()) {
            err = "--db " + given + ": a relative path needs the repository root (test-summary/README.md "
                  "above the working directory or this binary); pass an absolute path";
            return {};
        }
        p = root / p;
    }
    p = fs::weakly_canonical(p, ec);
    const fs::path db = p.parent_path() / "results.db";   // what store_bench writes (results_db_path)
    if (p.parent_path().filename() != "test-summary") {
        err = "--db " + p.string() + " is not under a test-summary/ directory (the shared results.db is "
              + (root.empty() ? std::string("<root>/test-summary/results.db") : (root / "test-summary" / "results.db").string()) + ")";
        return {};
    }
    const fs::path shared = root.empty() ? fs::path() : fs::weakly_canonical(root / "test-summary" / "results.db", ec);
    if (!fs::exists(db, ec) && db != shared) {
        err = "--db " + p.string() + " would create a new results.db outside the shared one"
              + (shared.empty() ? std::string() : " (" + shared.string() + ")");
        return {};
    }
    return db.string();
}

#ifdef JAC313_STORE_HAS_SQL_PERSIST
// ---- the results DB (receiver): one append-only row per run, written via Qlite ----
int set_flag_count(std::uint64_t f) { int n = 0; for (int i = 0; i < 7; ++i) if (f & (1ull << i)) ++n; return n; }

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
// footprint of jtext / binary / sql / html / json for this config.
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

// ============================================================================
// results.db — the unified, normalized results database. The schema and the compiler/lookup/run
// helpers live in jac313_results_db.hpp (jac313::results — shared with the test CLI, single source
// of truth). The wrappers below adapt store_bench to that shared API — results.db
// is the only thing store_bench writes (the legacy bench_run table is retired).
// ============================================================================
std::filesystem::path results_db_path(const std::string& bench_db_path) {
    namespace fs = std::filesystem;
    const fs::path p(bench_db_path);
    return (p.has_parent_path() ? p.parent_path() : fs::path(".")) / "results.db";
}

const std::string& bench_harness_version() {
    static const std::string ver = jac313::harness_version_from_cwd();
    return ver;
}

// Shared schema + the 11 bench-suite testList names (store_bench owns those; the CLI seeds its own).
void ensure_results_schema(jac313::Qlite::v001::Sqlite& db) {
    jac313::results::ensure_schema(db);
    db.exec("INSERT OR IGNORE INTO testList(name) VALUES"
            " ('0 flags, non-durable'),('2 flags, non-durable'),('4 flags, non-durable'),"
            " ('6 flags, non-durable'),('durable jtext'),('durable sql'),('durable binary'),"
            " ('durable html'),('durable json')");
}

std::int64_t results_next_run_id(jac313::Qlite::v001::Sqlite& db) {
    return jac313::results::next_run_id(db);
}

// Allocate the next results.db run_id up front (one per --suite run, shared by all its configs).
std::int64_t next_results_run_id(const std::string& bench_db_path) {
    try {
        jac313::Qlite::v001::Sqlite db(results_db_path(bench_db_path).string());
        ensure_results_schema(db);
        return results_next_run_id(db);
    } catch (...) { return 0; }
}

std::int64_t results_lookup_id(jac313::Qlite::v001::Sqlite& db, const char* sql, const std::string& name) {
    return jac313::results::lookup_id(db, sql, name);
}

std::int64_t compiler_id_for(jac313::Qlite::v001::Sqlite& db, const jac313::results::CompilerInfo& c) {
    return jac313::results::compiler_id(db, c);
}

// Insert-or-find a bench parameter combo. How this binary was built (build type, modules, strict,
// hardened, contracts) comes from the CMake tree (kBuild*, contracts_mode); batch is the persistence
// batch the run used (0 = non-durable); flags_raw the exact flag mask (flag_count its popcount); the
// production run's mix (keeper / database %, binary log on); the --max-runs / --max-records caps
// given; size/output_mode/valgrind_tool N/A => ''.
std::int64_t parameter_id_bench(jac313::Qlite::v001::Sqlite& db, std::int64_t compiler_id, const Params& p,
                                const std::string& persist, std::int64_t batch) {
    jac313::results::ParameterRow r;
    r.compiler_id = compiler_id;
    r.build_type = kBuildType; r.modules = kBuildModules; r.strict = kBuildStrict; r.hardened = kBuildHardened;
    r.contracts = contracts_mode;
    r.persist = persist;
    r.threads = static_cast<std::int64_t>(p.threads);
    r.events_per_thread = static_cast<std::int64_t>(p.events_per_thread);
    r.runs = static_cast<std::int64_t>(p.runs);
    r.batch = batch;
    r.flag_count = set_flag_count(p.flags);
    r.flags_raw = static_cast<std::int64_t>(p.flags);
    if (p.persist.rfind("production", 0) == 0) {   // "production", or "production/cap=N" when capped
        r.keeper_pct = static_cast<std::int64_t>(p.prod.keeper_pct);
        r.database_pct = static_cast<std::int64_t>(p.prod.database_pct);
        r.binary_log = p.prod.binary ? 1 : 0;
    }
    r.max_runs = static_cast<std::int64_t>(p.max_runs);
    r.max_records = static_cast<std::int64_t>(p.max_records);
    return jac313::results::parameter_id(db, r);
}

// The value of `key` in a READBACK line's "k=v k=v" text ("" when absent).
std::string readback_field(const std::string& line, const std::string& key) {
    const std::string k = key + "=";
    for (std::size_t at = 0; (at = line.find(k, at)) != std::string::npos; at += k.size()) {
        if (at != 0 && line[at - 1] != ' ') continue;
        const auto v = at + k.size();
        if (v < line.size() && line[v] == '"') {
            const auto close = line.find('"', v + 1);
            return line.substr(v + 1, (close == std::string::npos ? line.size() : close) - v - 1);
        }
        return line.substr(v, line.find(' ', v) == std::string::npos ? std::string::npos : line.find(' ', v) - v);
    }
    return {};
}

// Insert this bench's run row (run_id 0 = the next free one) and return its id. All three component
// versions come from the package_version table (pinned from the code version() literals by the CLI at
// host-pin time) — the single source store_bench can reach for jText, whose header isn't visible in
// this TU. Store/Qlite fall back to their in-process literal if the table is somehow unpinned (same
// value; belt-and-suspenders), jText to empty. The run-wide settings: its command line, keep-logs,
// what the PostgreSQL sinks did with their tables, the Store tests (store_bench is one), the pinned
// disk's source.
std::int64_t bench_insert_run(jac313::Qlite::v001::Sqlite& db, const Params& p, std::int64_t machine_id,
                              std::int64_t run_id) {
    namespace R = jac313::results;
    if (run_id == 0) run_id = results_next_run_id(db);
    const std::string& jver = bench_harness_version();   // 'v001'|'v002' — the world scope for package_version
    R::RunSettings rs;
    rs.command_line = R::JAC313_RUN_SETTINGS.command_line;
    rs.keep_logs = p.keep_logs ? 1 : 0;
    rs.keep_logs_dir = p.keep_logs_dir;
    rs.pg_cleanup = p.pg_cleanup;
    rs.store_tests = 1;
    rs.disk_source = R::current_host_disk_source(db, jver);
    R::insert_run(db, run_id, R::current_os_id(db), machine_id, jver,
                  R::package_version_get(db, machine_id, jver, "Store", std::string(jac313::Store::v001::version())),
                  R::package_version_get(db, machine_id, jver, "Qlite", std::string(jac313::Qlite::v001::version())),
                  R::package_version_get(db, machine_id, jver, "jText"), rs);
    return run_id;
}

// Write one normalized testRun fact (resolving run / parameter / testList / testType) into results.db.
// A production config also writes its hot-path ops/sec and the three drain times (`prod`).
void record_to_results_db(const Params& p, const BenchSummary& s, const ProdSummary* prod) {
    if (p.db_path.empty()) { not_recorded("no --db given"); return; }
    try {
        jac313::Qlite::v001::Sqlite db(results_db_path(p.db_path).string());
        ensure_results_schema(db);
        // Read the pinned machine (set by the CLI/bootstrap). store_bench no longer self-senses the
        // host, so it can't disagree on disk/cores and split the machine in two. The OS is sensed
        // here, per run, like every recorded run.
        const std::int64_t machine_id = jac313::results::current_host(db, bench_harness_version());
        if (machine_id == 0) {
            not_recorded("no current_host pin in " + results_db_path(p.db_path).string()
                         + " (run `jac313_test_cli host` or ./bsir.sh first)");
            return;
        }
        const std::int64_t run_id = bench_insert_run(db, p, machine_id, p.run_id);
        namespace R = jac313::results;
        // The config name as run ("durable sql @10M [capped]"): a capped or rescaled config keeps a
        // name of its own, so it never merges with the full-size one on the report.
        db.exec("INSERT OR IGNORE INTO testList(name) VALUES(?)", p.label);
        const std::int64_t test_list_id = results_lookup_id(db, "SELECT id FROM testList WHERE name=?", p.label);
        const std::int64_t test_type_id = results_lookup_id(db, "SELECT id FROM testType WHERE name=?", std::string("bench"));
        const std::int64_t comp_id = compiler_id_for(db, this_compiler());
        // A production config files under its sink pair ("jtext+sql", binary implied), so a different
        // pair is a different parameter row; its mix is in the testList name.
        std::string persist = p.persist;
        if (persist.rfind("production", 0) == 0) persist = p.prod.persist_label() + persist.substr(10);
        // parameter.batch: the run's batch, for the production run its database sink's (Params::batch).
        // The production row key is its sink pair, so the other two batches would add nothing to the
        // parameter identity beyond what the database's does; testRun carries all three below.
        const std::int64_t batch = p.persist == "none" ? 0 : static_cast<std::int64_t>(p.batch);
        // The batch each sink ran at: a one-sink run's sink is a database (sql | pg-*), the binary log,
        // or a keeper-style file (jtext | html | json); the production run's three each have their own.
        // (A capped suite run's persist carries "/cap=N": the sinks are those of the backend before it.)
        std::int64_t batch_keeper = 0, batch_database = 0, batch_binary = 0;
        Params sinks = p;
        sinks.persist = p.persist.substr(0, p.persist.find('/'));
        for (const auto& sp : sink_plan(sinks)) {
            const bool is_db = sp.backend == "sql" || sp.backend == "pg-local" || sp.backend == "pg-remote";
            (sp.backend == "binary" ? batch_binary : is_db ? batch_database : batch_keeper)
                = static_cast<std::int64_t>(sp.batch);
        }
        const std::int64_t param_id = parameter_id_bench(db, comp_id, p, persist, batch);
        const std::int64_t bytes = (p.persist == "none") ? 0
                                 : static_cast<std::int64_t>(measure_output_bytes(p.base_name));
        // status 'pass': a bench row is written only on success; duration_ms 0 (throughput is in the ops columns).
        // hot_ops / drain_*_ms: the production run's figures (0 for every other config).
        const ProdSummary none{};
        const ProdSummary& ps = prod ? *prod : none;
        // The read-back (the untimed extra run, one READBACK line per sink), as the smoke recorder
        // writes it: rows_persisted / pg figures from the dataset sink (the database one in production),
        // rows_checked and read_ms summed over the sinks, status FAIL when any sink failed.
        std::string rb_mode, rb_status;
        std::int64_t rows_persisted = 0, rows_checked = 0;
        double read_ms = 0, pg_connect = 0, pg_ping = 0, pg_write = 0;
        auto num = [](const std::string& v) { try { return v.empty() ? 0.0 : std::stod(v); } catch (...) { return 0.0; } };
        if (p.persist != "none" && p.readbacks.empty()) rb_mode = p.readback;   // no read-back run: its mode (off)
        for (const auto& l : p.readbacks) {
            rb_mode = readback_field(l, "mode");
            const std::string st = readback_field(l, "status");
            if (rb_status.empty() || st != "PASS") rb_status = st;
            rows_persisted = static_cast<std::int64_t>(num(readback_field(l, "expected")));   // the last = the dataset sink
            rows_checked += static_cast<std::int64_t>(num(readback_field(l, "checked")));
            read_ms += num(readback_field(l, "read_ms"));
            pg_connect = num(readback_field(l, "connect_ms"));
            pg_ping = num(readback_field(l, "ping_ms"));
            pg_write = num(readback_field(l, "write_ms"));
        }
        auto tx = db.transaction();   // the row, its targets and the safeness refresh commit together
        db.exec("INSERT INTO testRun(run_id, test_type_id, test_list_id, parameter_id, status, duration_ms, "
                "median_ops, low_ops, high_ops, avg_ops, stddev_ops, bytes, hot_ops, drain_keeper_ms, "
                "drain_database_ms, drain_binary_ms, batch_keeper, batch_database, batch_binary, max_persist_rows, "
                "rows_persisted, readback_mode, readback_status, rows_checked, read_ms, pg_connect_ms, pg_ping_ms, "
                "pg_write_ms) VALUES(?,?,?,?, 'pass', 0, ?,?,?,?,?, ?, ?,?,?,?, ?,?,?, ?, ?,?,?,?,?, ?,?,?)",
                run_id, test_type_id, test_list_id, param_id,
                static_cast<std::int64_t>(s.median), static_cast<std::int64_t>(s.low),
                static_cast<std::int64_t>(s.high), static_cast<std::int64_t>(s.avg),
                static_cast<std::int64_t>(s.stddev), bytes,
                static_cast<std::int64_t>(ps.hot_ops), static_cast<std::int64_t>(ps.drain_keeper_ms),
                static_cast<std::int64_t>(ps.drain_database_ms), static_cast<std::int64_t>(ps.drain_binary_ms),
                batch_keeper, batch_database, batch_binary,
                p.persist == "none" ? std::int64_t{0} : static_cast<std::int64_t>(p.max_persist_rows),
                rows_persisted, rb_mode, rb_status, rows_checked, read_ms, pg_connect, pg_ping, pg_write);
        jac313::results::insert_run_targets(db, run_id, jac313::results::parse_target_lines(p.target_lines));
        jac313::results::rebuild_safeness(db);   // refresh the safeness summary (read by report + verdict)
        tx.commit();
    } catch (const std::exception& e) {
        not_recorded(std::string("record failed: ") + e.what());
    }
}

// bench_results.db / bench_run is retired — store_bench writes only the unified results.db.
void record_to_db(const Params& p, const BenchSummary& s, const ProdSummary* prod = nullptr) {
    record_to_results_db(p, s, prod);
}
#else
void record_to_db(const Params&, const BenchSummary&, const ProdSummary* = nullptr) {   // built without SQL persist -> no DB
    not_recorded("this store_bench was built without SQL persistence (no results.db writer)");
}
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
// Per-machine durable-backend calibration. For each of binary/jtext/sql/html/json, sweep the persistence batch
// record size (batch) over a schedule under the same process-level timing as everything else, keeping
// two running numbers: the best median ops/sec seen and the batch that produced it. useThis is that
// PEAK batch — the record size at the max ops/sec — NOT an average. (The old "middle of the plateau
// band" averaged the peak toward its slower neighbours, which measurably hurt the pick.) We still
// report the near-peak band low_size/high_size (every batch within tol% of the peak) for reference;
// best_size/best_ops (the raw peak) and useThis land in io_best_fit for THIS machine. The recorded
// suite then runs each durable config at useThis.
// Needs --db and a pinned current_host (./bsir.sh or a CLI gate).
struct CalResult { std::string type; std::size_t low, high, best, use; std::uint64_t best_ops; };

int run_calibration(Params base, const std::vector<std::size_t>& schedule, std::size_t tol_pct,
                    const std::string& sched_desc) {
    if (base.db_path.empty()) { std::cerr << "ERROR: --calibrate needs --db <results.db>\n"; return 2; }
    if (schedule.empty()) { std::cerr << "ERROR: empty calibrate schedule (check --cal-* range)\n"; return 2; }

    std::int64_t mid = 0;
    try {
        jac313::Qlite::v001::Sqlite db(results_db_path(base.db_path).string());
        ensure_results_schema(db);
        mid = jac313::results::current_host(db, bench_harness_version());
    } catch (const std::exception& e) { std::cerr << "[calibrate] DB error: " << e.what() << "\n"; return 1; }
    if (mid == 0) {
        std::cerr << "[calibrate] no current_host pinned (run ./bsir.sh or a CLI gate first)\n";
        std::cout << "NOT RECORDED: calibration: no current_host pin in " << results_db_path(base.db_path).string() << "\n";
        return 1;
    }
    // The calibration is a run of its own (its command line holds the knobs); each backend's result
    // is a 'calibrate' runEvent and its io_best_fit row names the run.
    std::int64_t cal_run = 0;
    try {
        jac313::Qlite::v001::Sqlite db(results_db_path(base.db_path).string());
        cal_run = bench_insert_run(db, base, mid, 0);
    } catch (const std::exception& e) { std::cerr << "[calibrate] DB error: " << e.what() << "\n"; return 1; }

    std::cout << "\n===============================================================\n";
    std::cout <<   "--- calibrating persistence batch best size for this machine ---\n";
    std::cout <<   "===============================================================\n";
    std::cout << "host " << jac313::results::host_label(mid) << "   threads=" << base.threads
              << "  events/thread=" << bench_group_thousands(base.events_per_thread)
              << "  runs=" << base.runs
              << "  batch " << sched_desc << " (" << schedule.size() << " pts)"
              << "  (plateau = within " << tol_pct << "% of peak)\n";

    std::vector<CalResult> results;
    for (const char* ty : {"binary", "jtext", "sql", "html", "json"}) {
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
        // Robust plateau. At this scale the ops/sec curve is a flat, noisy band — run-to-run variance on
        // an identical config is large — so the raw argmax (best_batch) is a coin toss over the plateau
        // and a lone spike can either crown a random batch or, via a tight tol% window, punch legitimate
        // low batches out of the band. Two defenses:
        //   1. Smooth the per-batch medians with a small moving average, so a single spike neither defines
        //      the peak nor holes the plateau. Threshold and membership are computed on the smoothed curve.
        //   2. useThis = the SMALLEST batch within tol% of the smoothed peak (low_b), not the argmax. Every
        //      batch in [low_b..high_b] delivers the same real throughput; among those the smallest is
        //      strictly better on secondary costs (less memory pinned in the worker batch, lower drain
        //      latency, smaller tail-loss on crash) and is far more stable across runs.
        // best_batch/best_median stay as the RAW peak in the record/report for reference.
        std::vector<std::uint64_t> smoothed(medians.size(), 0);
        for (std::size_t i = 0; i < medians.size(); ++i) {
            std::uint64_t sum = 0; std::size_t n = 0;
            const std::size_t lo = (i == 0) ? 0 : i - 1;
            for (std::size_t j = lo; j < medians.size() && j <= i + 1; ++j) { sum += medians[j]; ++n; }
            smoothed[i] = n ? sum / n : medians[i];
        }
        std::uint64_t robust_peak = 0;
        for (std::uint64_t v : smoothed) robust_peak = std::max(robust_peak, v);
        const std::uint64_t thresh = robust_peak - (robust_peak * tol_pct) / 100;
        std::size_t low_b = best_batch, high_b = best_batch; bool have_band = false;
        for (std::size_t i = 0; i < schedule.size(); ++i)
            if (smoothed[i] >= thresh) {
                if (!have_band) { low_b = high_b = schedule[i]; have_band = true; }
                else { low_b = std::min(low_b, schedule[i]); high_b = std::max(high_b, schedule[i]); }
            }
        const std::size_t use = low_b;
        std::cout << "  band [" << bench_group_thousands(low_b) << ".." << bench_group_thousands(high_b)
                  << "]  peak " << bench_group_thousands(best_median) << " @ batch "
                  << bench_group_thousands(best_batch) << "  -> useThis " << bench_group_thousands(use) << "\n";
        results.push_back({ty, low_b, high_b, best_batch, use, best_median});
        try {
            jac313::Qlite::v001::Sqlite db(results_db_path(base.db_path).string());
            ensure_results_schema(db);
            jac313::results::upsert_io_best_fit(db, mid, ty,
                static_cast<std::int64_t>(low_b), static_cast<std::int64_t>(high_b),
                static_cast<std::int64_t>(best_batch), static_cast<std::int64_t>(best_median),
                static_cast<std::int64_t>(use), cal_run);
            jac313::results::log_run_event(db, cal_run, ty, "calibrate",
                "band " + std::to_string(low_b) + ".." + std::to_string(high_b) + ", peak "
                + std::to_string(best_median) + " ops/sec @ batch " + std::to_string(best_batch)
                + ", useThis " + std::to_string(use));
        } catch (const std::exception& e) {
            std::cerr << "[calibrate] record failed: " << e.what() << "\n";
            std::cout << "NOT RECORDED: calibration: " << e.what() << "\n";
            return 1;
        }
    }

    std::cout << "\n===============================================================\n";
    std::cout <<   "--- calibration summary: " << jac313::results::host_label(mid) << " ---\n";
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
        jac313::Qlite::v001::Sqlite db(results_db_path(db_path).string());
        ensure_results_schema(db);
        const std::int64_t mid = jac313::results::current_host(db, bench_harness_version());
        if (mid == 0) return fallback;
        return jac313::results::io_best_fit_use(db, mid, type, fallback);
    } catch (...) { return fallback; }
}

// The production run's batches (results::production_batches): the database sink's into p.batch, the
// replay log's and the keeper's into p.prod. p.batch on entry is the fallback; override_batch > 0
// (--batch) gives all three that.
void apply_production_batches(Params& p, std::size_t override_batch) {
    const auto fb = static_cast<std::int64_t>(p.batch), o = static_cast<std::int64_t>(override_batch);
    jac313::results::SinkBatches b{o ? o : fb, o ? o : fb, o ? o : fb};   // no --db: the fallback
    if (!p.db_path.empty()) try {
        jac313::Qlite::v001::Sqlite db(results_db_path(p.db_path).string());
        ensure_results_schema(db);
        const std::int64_t mid = jac313::results::current_host(db, bench_harness_version());   // 0: no rows
        b = jac313::results::production_batches(db, mid, p.prod.keeper, p.prod.database, fb, o);
    } catch (...) {}
    p.prod.batch_binary = static_cast<std::size_t>(b.binary);
    p.prod.batch_keeper = static_cast<std::size_t>(b.keeper);
    p.batch = static_cast<std::size_t>(b.database);
}
#else
int run_calibration(Params, const std::vector<std::size_t>&, std::size_t, const std::string&) {
    std::cerr << "ERROR: --calibrate needs the SQL-persist build (JAC313_STORE_HAS_SQL_PERSIST)\n"; return 2;
}
std::int64_t calibrated_use(const std::string&, const std::string&, std::int64_t fb) { return fb; }
void apply_production_batches(Params& p, std::size_t override_batch) {
    if (override_batch) p.batch = override_batch;
    p.prod.batch_binary = p.prod.batch_keeper = p.batch;
}
#endif

// The suite, in the order it runs (and the report shows): the production run (@1M, @10M), then each
// database alone (sql, pg-local, pg-remote), then each file alone (binary, jtext, html, json), each
// @1M and @10M, then the non-durable flag sweep (0/2/4/6 flags). 16 configs by default.
// Two scales of the SAME configs:
//   full  (--suite)         — the big numbers; the recorded throughput benchmark (Release).
//   smoke (--suite --smoke) — the same 10 capped to <=10k events, run as a CORRECTNESS GATE:
//                             pass iff ALL configs run clean (incl. structural verify); the
//                             numbers are meaningless at this scale, so nothing is recorded.
// One line per suite config for Jac313Runner's summary (--summary-parts): key=value pairs
// (shared/jac313_summary_part.hpp), assembled into the summary jText by jac313_test_cli.
void append_bench_part(const Params& p, const char* status, const BenchSummary* sum, std::uint64_t total,
                       const ProdSummary* prod = nullptr) {
    if (p.summary_parts.empty()) return;
    namespace SP = jac313::summary_part;
    SP::Part part{{"gate", "bench"}, {"test", p.label}, {"backend", p.persist}, {"status", status},
                  {"threads", std::to_string(p.threads)}, {"events_per_thread", std::to_string(p.events_per_thread)},
                  {"runs", std::to_string(p.runs)}};
    // the recorded run and its machine (the summary's run row shows them; absent when not recorded)
    if (p.run_id != 0) part.emplace_back("run_id", std::to_string(p.run_id));
#ifdef JAC313_STORE_HAS_SQL_PERSIST
    if (!p.db_path.empty()) {
        try {
            jac313::Qlite::v001::Sqlite db(results_db_path(p.db_path).string());
            if (const auto mid = jac313::results::current_host(db, bench_harness_version()); mid != 0)
                part.emplace_back("host", jac313::results::host_label(mid));
        } catch (...) {}
    }
#endif
    if (sum) {
        part.emplace_back("median_ops", std::to_string(sum->median));
        part.emplace_back("low_ops", std::to_string(sum->low));
        part.emplace_back("high_ops", std::to_string(sum->high));
        part.emplace_back("events", std::to_string(total));
        if (sum->median > 0) part.emplace_back("ms", std::to_string(total * 1000 / sum->median));
    }
    if (prod) {
        part.emplace_back("hot_ops", std::to_string(prod->hot_ops));
        part.emplace_back("drain_binary_ms", std::to_string(prod->drain_binary_ms));
        part.emplace_back("drain_keeper_ms", std::to_string(prod->drain_keeper_ms));
        part.emplace_back("drain_database_ms", std::to_string(prod->drain_database_ms));
    }
    if (p.persist != "none") {
        part.emplace_back("max_persist_rows", std::to_string(p.max_persist_rows));
        // "k=v k=v ... error=\"x\"" -> fields "<prefix>k"; `expected` also goes to rows_written when asked.
        auto readback_fields = [&](const std::string& l, const std::string& prefix, bool rows_written) {
            std::size_t i = 0;
            while (i < l.size()) {
                while (i < l.size() && l[i] == ' ') ++i;
                const auto eq = l.find('=', i);
                if (eq == std::string::npos) break;
                const std::string k = l.substr(i, eq - i);
                std::string v;
                std::size_t j = eq + 1;
                if (j < l.size() && l[j] == '"') {
                    const auto cl = l.find('"', j + 1);
                    v = l.substr(j + 1, (cl == std::string::npos ? l.size() : cl) - j - 1);
                    j = cl == std::string::npos ? l.size() : cl + 1;
                } else {
                    const auto sp = l.find(' ', j);
                    v = l.substr(j, (sp == std::string::npos ? l.size() : sp) - j);
                    j = sp == std::string::npos ? l.size() : sp;
                }
                i = j;
                if (k == "backend") continue;
                part.emplace_back(prefix + k, v);
                if (k == "expected" && rows_written) part.emplace_back("rows_written", v);
            }
        };
        if (!p.last_readback.empty()) readback_fields(p.last_readback, "readback.", true);
        if (p.readbacks.size() > 1)   // the production run: one set per sink
            for (std::size_t k = 0; k < p.readbacks.size(); ++k)
                readback_fields(p.readbacks[k], "readbacks." + std::to_string(k + 1) + ".", false);
        // The production run's dataset is the database sink's (the replay log and keeper file sit beside it).
        const auto plan = sink_plan(p);
        const SinkPlan& ds = plan.back();
        part.emplace_back("dataset.backend", ds.backend);
        part.emplace_back("dataset.base", ds.base);
    }
    std::ofstream out(p.summary_parts, std::ios::app);
    if (out) out << SP::format(part) << "\n";
}

// The suite table supplies threads (50), events per thread and runs per config; --threads,
// --events-per-thread and --runs, when given, replace those defaults for every config. --batch, when
// given, replaces the calibrated batch (io_best_fit.useThis) of every durable config.
struct SuiteAxes { bool threads = false, events = false, runs = false, batch = false; };

int run_suite(Params base, bool dry, bool smoke, SuiteAxes given) {
    if (!given.threads) base.threads = 50;             // the suite's default thread count
    struct Cfg { std::string persist; std::size_t flags; std::string label; std::size_t evt, runs; };
    std::vector<Cfg> cfgs;
    // The production run first: the user's keeper + database sinks and the replay log, flags routed.
    bool production = true;
#ifndef JAC313_STORE_HAS_PG_PERSIST
    if (base.prod.database == "pg-local" || base.prod.database == "pg-remote") {
        std::cerr << "NOTE: production run skipped: its database " << base.prod.database
                  << " needs PostgreSQL persistence, not built\n";
        production = false;
    }
#endif
    if (production) {
        cfgs.push_back({"production", 0, base.prod.config_name(), 20000, 3});            // 1M × 3
        cfgs.push_back({"production", 0, base.prod.config_name() + " @10M", 200000, 3}); // 10M × 3
    }
    // Durable backends: the default five, or --persist-list (which may add pg-local / pg-remote).
    std::vector<std::string> durable = {"sql", "binary", "jtext", "html", "json"};
    if (!base.persist_list.empty()) {
        durable.clear();
        std::string cur;
        for (char c : base.persist_list + ",") {
            if (c == ',') {
                if (!cur.empty() && cur != "none") {
#ifndef JAC313_STORE_HAS_PG_PERSIST
                    if (cur == "pg-local" || cur == "pg-remote") {
                        std::cerr << "NOTE: " << cur << " skipped: PostgreSQL persistence not built\n";
                        cur.clear();
                        continue;
                    }
#endif
                    durable.push_back(cur);
                }
                cur.clear();
            } else if (c != ' ') cur += c;
        }
    }
    // Databases first, then files (whatever order --persist-list named them in).
    const std::vector<std::string> canonical = {"sql", "pg-local", "pg-remote", "binary", "jtext", "html", "json"};
    std::stable_sort(durable.begin(), durable.end(), [&](const std::string& a, const std::string& b) {
        return std::find(canonical.begin(), canonical.end(), a) < std::find(canonical.begin(), canonical.end(), b);
    });
    auto is_db = [](const std::string& b) { return b == "sql" || b == "pg-local" || b == "pg-remote"; };
    for (const bool dbs : {true, false})
        for (const auto& pn : durable)
            if (is_db(pn) == dbs) {
                cfgs.push_back({pn, 0, "durable " + pn, 20000, 3});           // 1M × 3
                cfgs.push_back({pn, 0, "durable " + pn + " @10M", 200000, 3}); // 10M × 3 (scaling)
            }
    for (const std::size_t n : {0uz, 2uz, 4uz, 6uz})
        cfgs.push_back({"none", n, std::to_string(n) + " flags, non-durable", 200000, 10});

    // --threads / --events-per-thread / --runs given: every config uses them (its label says so).
    if (given.threads || given.events || given.runs) {
        std::string tag;
        if (given.threads) tag += "threads=" + std::to_string(base.threads);
        if (given.events)  tag += (tag.empty() ? "" : " ") + std::string("events/thread=") + std::to_string(base.events_per_thread);
        if (given.runs)    tag += (tag.empty() ? "" : " ") + std::string("runs=") + std::to_string(base.runs);
        for (auto& cf : cfgs) {
            if (given.events) cf.evt = base.events_per_thread;
            if (given.runs)   cf.runs = base.runs;
            cf.label += " [" + tag + "]";
        }
    }
    // --max-runs / --max-records: lower runs and events per run (never raise them).
    for (auto& cf : cfgs) {
        bool capped = false;
        if (base.max_runs > 0 && cf.runs > base.max_runs) { cf.runs = base.max_runs; capped = true; }
        if (base.max_records > 0 && base.threads * cf.evt > base.max_records) {
            cf.evt = std::max<std::size_t>(1, base.max_records / base.threads);
            capped = true;
        }
        if (capped) cf.label += " [capped]";   // e.g. "@10M" no longer means 10M events
    }

    if (smoke) {
        const std::size_t cap = base.threads ? 10000 / base.threads : 10000;  // 200 -> 10k total / config
        for (auto& cf : cfgs) {
            cf.evt = std::min<std::size_t>(cf.evt, cap == 0 ? 1 : cap);
            cf.runs = std::min<std::size_t>(cf.runs, 2);
            cf.label += " (smoke)";
        }
    }

    const bool retaining = keep_logs_enabled(base);
    if (retaining) {
        std::error_code ec;
        std::filesystem::create_directories(resolved_keep_logs_dir(base), ec);
        std::cout << "keep-logs: durable suite output -> " << resolved_keep_logs_dir(base) << "\n";
    }

    if (dry) {
        std::cout << "# Store benchmark suite — " << cfgs.size() << " configs"
                  << (smoke ? " [SMOKE gate: <=10k events, pass/fail, not recorded]" : "")
                  << " (run from the build dir)\n\n";
        for (const auto& cf : cfgs) {
            std::cout << "./jac313_store_bench --threads " << base.threads << " --events-per-thread " << cf.evt
                      << " --runs " << cf.runs;
            if (cf.persist == "production") std::cout << " --production " << base.prod.spec();
            else std::cout << " --persist " << cf.persist;
            if (cf.persist == "none") std::cout << " --flag-count " << cf.flags;
            else if (given.batch) std::cout << " --batch " << base.batch;
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
            if (retaining && cf.persist != "none") p.base_name = suite_persist_base(base, cf.persist, cf.label);
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
    // Announce the calibrated persistence batch sizes (io_best_fit.useThis) this run will use per durable
    // backend — so the recorded numbers are tied to a visible batch (falls back to the default if a
    // backend isn't calibrated yet; run --calibrate to populate io_best_fit).
    if (given.batch) std::cout << "durable batch (--batch): " << bench_group_thousands(base.batch);
    else {
        std::cout << "durable batch (io_best_fit.useThis):";
        for (const char* ty : {"sql", "binary", "jtext", "html", "json"})
            std::cout << "  " << ty << "=" << bench_group_thousands(
                static_cast<std::uint64_t>(calibrated_use(base.db_path, ty, static_cast<std::int64_t>(base.batch))));
    }
    if (production) {
        // the production run's three sinks, each at its own batch
        Params pb = base;
        pb.persist = "production";
        apply_production_batches(pb, given.batch ? base.batch : 0);
        std::cout << "\nproduction batch: " << production_batch_fields(pb);
    }
    std::cout << std::endl;   // flush: surface the chosen batches promptly (before the long runs)
    for (const auto& cf : cfgs) {
        Params p = base;
        p.persist = cf.persist; p.flags = flags_by_count(cf.flags); p.label = cf.label;
        p.events_per_thread = cf.evt; p.runs = cf.runs;
        if (retaining && cf.persist != "none") p.base_name = suite_persist_base(base, cf.persist, cf.label);
        // Durable configs run at this machine's calibrated persistence batch size (io_best_fit.useThis);
        // falls back to the default batch when uncalibrated. Non-durable has no writer, so batch is moot.
        // The production run's three sinks each run at their own type's batch.
        if (cf.persist == "production")
            apply_production_batches(p, given.batch ? base.batch : 0);
        else if (cf.persist != "none" && !given.batch)
            p.batch = static_cast<std::size_t>(calibrated_use(base.db_path, cf.persist, static_cast<std::int64_t>(base.batch)));
        std::cout << "\n=== " << cf.label << " ===";
        if (cf.persist == "production") std::cout << "  (" << production_batch_fields(p) << ")";
        else if (cf.persist != "none") std::cout << "  (batch " << bench_group_thousands(p.batch) << ")";
        std::cout << "\n";
        std::vector<ProdTimes> prod_runs;
        auto durations = measure_set(p, cf.persist == "production" ? &prod_runs : nullptr);
        if (durations.empty()) {
            std::cerr << "FAILED: " << cf.label << "\n";
            append_bench_part(p, "FAIL", nullptr, 0);
            return 1;
        }
        const std::uint64_t total = static_cast<std::uint64_t>(p.threads) * p.events_per_thread;
        emit_bench_stats(durations, total);
        const BenchSummary sum = compute_bench_summary(durations, total);
        std::optional<ProdSummary> prod;
        if (cf.persist == "production") prod = summarize_production(p, prod_runs, total);
        Params rec = p;
        // A capped durable run measures "events in, fewer rows out": record it under its own
        // persist label so it never mixes with full-size durable results in the report.
        if (rec.persist != "none" && rec.max_persist_rows > 0)
            rec.persist += "/cap=" + std::to_string(rec.max_persist_rows);
        record_to_db(rec, sum, prod ? &*prod : nullptr);
        append_bench_part(p, "PASS", &sum, total, prod ? &*prod : nullptr);
    }
    std::cout << "\n\n";   // two blank lines after the bench suite, for visual separation
    return 0;
}


} // namespace

int main(int argc, char** argv) {
#ifdef JAC313_STORE_HAS_SQL_PERSIST
    jac313::results::JAC313_RUN_SETTINGS.command_line = jac313::results::command_line_of(argc, argv);
#endif
    Params p;
    p.flags = default_flags();
    std::optional<Sweep> sweep;
    bool do_suite = false, dry = false, smoke = false;
    bool single_run = false, verify_flag = false;   // worker mode (internal: re-exec by run_one)
    bool readback_run = false;                       // worker mode: the untimed read-back run
    bool do_calibrate = false;                       // --calibrate: per-machine batch best-fit -> io_best_fit
    // Two-phase sweep: fine (cal_step) from cal_lo up to cal_knee, then coarse (cal_step2) up to cal_hi.
    // Default: 250,500,750,1000, then 1500..5000:500 — fine where the curve moves, coarse in the flat tail.
    std::size_t cal_lo = 250, cal_knee = 1000, cal_step = 250, cal_step2 = 500, cal_hi = 5000;
    std::size_t cal_tol = 3;                          // plateau = within cal_tol% of peak (band -> middle = useThis)
    bool threads_set = false, runs_set = false;      // so --calibrate and --suite can apply their own defaults
    bool events_set = false, batch_set = false;      // --events-per-thread / --batch given (the suite honours them)
    bool no_record = false;                          // --no-record: measure only, on purpose (the ctest smoke)
    bool production_set = false;                     // --production given (selects it outside --suite)

    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        auto next = [&]() -> std::string { return (i + 1 < argc) ? argv[++i] : std::string{}; };
        if (a == "--threads") { p.threads = std::stoull(next()); threads_set = true; }
        else if (a == "--events-per-thread") { p.events_per_thread = std::stoull(next()); events_set = true; }
        else if (a == "--runs") { p.runs = std::stoull(next()); runs_set = true; }
        else if (a == "--batch") { p.batch = std::stoull(next()); batch_set = true; }
        else if (a == "--batch-binary") p.prod.batch_binary = std::stoull(next());   // worker: the replay log's batch
        else if (a == "--batch-keeper") p.prod.batch_keeper = std::stoull(next());   // worker: the keeper's batch
        else if (a == "--persist") p.persist = next();
        else if (a == "--production") {
            // the production run's mix; on its own (not --suite) it also selects that run
            if (const std::string err = parse_production(next(), p.prod); !err.empty()) {
                std::cerr << "bad --production: " << err << "\n"; return 2;
            }
            production_set = true;
        }
        else if (a == "--base-name") p.base_name = next();
        else if (a == "--max-persist-rows") p.max_persist_rows = std::stoull(next());
        else if (a == "--readback") p.readback = next();
        else if (a == "--pg-cleanup") p.pg_cleanup = next();
        else if (a == "--persist-list") p.persist_list = next();
        else if (a == "--max-runs") p.max_runs = std::stoull(next());
        else if (a == "--max-records") p.max_records = std::stoull(next());
        else if (a == "--summary-parts") p.summary_parts = next();
        else if (a == "--readback-run") readback_run = true;          // worker: untimed read-back run
        else if (a == "--flags") p.flags = parse_flags(next());
        else if (a == "--flags-raw") p.flags = std::stoull(next());   // exact mask (worker re-exec)
        else if (a == "--flag-count") p.flags = flags_by_count(std::stoull(next()));
        else if (a == "--single-run") single_run = true;              // worker mode (one workload, no stats/DB)
        else if (a == "--verify") verify_flag = true;                 // worker: structural-verify this run
        else if (a == "--db") p.db_path = next();
        else if (a == "--no-record") no_record = true;
        else if (a == "--label") p.label = next();
        else if (a == "--keep-logs") p.keep_logs = true;
        else if (a == "--keep-logs-dir") p.keep_logs_dir = next();
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

    if (production_set) p.persist = "production";
    if (no_record && !p.db_path.empty()) { std::cerr << "--no-record and --db exclude each other\n"; return 2; }
    if (!p.db_path.empty() && !single_run) {
        std::string err;
        p.db_path = resolve_bench_db(p.db_path, err);
        if (p.db_path.empty()) { std::cerr << "store_bench: " << err << "\n"; return 2; }
    }
    // The end of a recording run: one "NOT RECORDED: ..." line per reason and a non-zero exit when
    // any measured config was not written to results.db.
    const auto finish = [&](int rc) {
        if (no_record) { std::cout << "not recorded (--no-record)\n"; return rc; }
        for (const auto& why : g_not_recorded) std::cout << "NOT RECORDED: " << why << "\n";
        return (rc == 0 && !g_not_recorded.empty()) ? 1 : rc;
    };

    // Worker mode: do exactly one workload and exit. The parent (run_one) times this whole
    // process from fork to exit, so nothing is printed and nothing is recorded here.
    if (single_run) return do_workload(p, verify_flag, readback_run) ? 0 : 1;

    if (do_calibrate) {
        // Calibration defaults to the durable 1M config's shape (50 threads, 1M events) unless the user
        // overrode them — so the best-fit batch is found at the scale it's used. We KEEP threads=50 (that
        // is production's contention regime; calibrating at fewer threads would pick for a different one).
        // The plateau is noisy at this scale, so calibration runs more repetitions than a normal bench
        // point to shrink per-batch variance (the median over N runs feeds the robust plateau selection);
        // runtime scales with N but calibration is occasional, so this is a deliberate accuracy/time trade.
        if (!threads_set) p.threads = 50;
        if (!runs_set)    p.runs = 9;
        const auto schedule = build_batch_schedule(cal_lo, cal_knee, cal_step, cal_hi, cal_step2);
        std::string desc = bench_group_thousands(cal_lo) + ".." + bench_group_thousands(cal_knee)
                         + ":" + bench_group_thousands(cal_step) + ", then .." + bench_group_thousands(cal_hi)
                         + ":" + bench_group_thousands(cal_step2);
        return run_calibration(p, schedule, cal_tol, desc);
    }

    if (do_suite) {
        const int rc = run_suite(p, dry, smoke, SuiteAxes{threads_set, events_set, runs_set, batch_set});
        return (dry || smoke) ? rc : finish(rc);   // dry-run and the smoke gate record nothing by design
    }

    if (!sweep) {
        // single config — one measurement set
        const bool prod_run = p.persist == "production";
        if (p.label.empty()) p.label = generated_label(p);
        // --max-runs / --max-records: lower runs and events per run (never raise them), as in the suite;
        // the name says so, so the report keeps the capped run apart from the full-size one.
        bool capped = false;
        if (p.max_runs > 0 && p.runs > p.max_runs) { p.runs = p.max_runs; capped = true; }
        if (p.max_records > 0 && p.threads * p.events_per_thread > p.max_records) {
            p.events_per_thread = std::max<std::size_t>(1, p.max_records / p.threads);
            capped = true;
        }
        if (capped) p.label += " [capped]";
        // The production run: each sink at its type's calibrated batch; --batch gives all three its N.
        if (prod_run) apply_production_batches(p, batch_set ? p.batch : 0);
        std::cout << "store_bench: threads=" << p.threads << " events/thread=" << p.events_per_thread
                  << " runs=" << p.runs << " persist=" << p.persist;
        if (prod_run) std::cout << " production=" << p.prod.spec() << " " << production_batch_fields(p);
        else std::cout << " batch=" << p.batch;
        std::cout << "\n";
        std::vector<ProdTimes> prod_runs;
        auto durations = measure_set(p, prod_run ? &prod_runs : nullptr);
        if (durations.empty()) { std::cerr << "benchmark failed\n"; return 1; }
        const std::uint64_t total = static_cast<std::uint64_t>(p.threads * p.events_per_thread);
        emit_bench_stats(durations, total);
        std::optional<ProdSummary> prod;
        if (prod_run) prod = summarize_production(p, prod_runs, total);
        if (!no_record) record_to_db(p, compute_bench_summary(durations, total), prod ? &*prod : nullptr);
        return finish(0);
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
    const std::string peak_line = "axis=" + sweep->axis + " best_val=" + std::to_string(best_val)
        + " median=" + std::to_string(best_median) + " high=" + std::to_string(best_high)
        + " stddev=" + std::to_string(best_stddev) + " persist=" + p.persist;
    std::cout << "BENCHSWEEP " << peak_line << "\n";
    // An exploratory sweep is recorded as what it ran and found: a run (its command line holds the
    // axis and range) with one 'sweep' runEvent carrying the peak; no testRun rows.
    if (!no_record && !p.db_path.empty()) {
#ifdef JAC313_STORE_HAS_SQL_PERSIST
        try {
            jac313::Qlite::v001::Sqlite db(results_db_path(p.db_path).string());
            ensure_results_schema(db);
            const std::int64_t mid = jac313::results::current_host(db, bench_harness_version());
            if (mid == 0) not_recorded("sweep: no current_host pin");
            else jac313::results::log_run_event(db, bench_insert_run(db, p, mid, 0), "", "sweep", peak_line);
        } catch (const std::exception& e) { not_recorded(std::string("sweep: ") + e.what()); }
#endif
    }
    return finish(0);
}
