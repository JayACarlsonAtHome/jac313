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
struct CompilerInfo { std::string name; std::string version; std::int64_t major; };
CompilerInfo this_compiler() {
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
            if (c != std::string::npos) { h.cpu = line.substr(c + 1); while (!h.cpu.empty() && h.cpu.front() == ' ') h.cpu.erase(h.cpu.begin()); }
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

// External run identifier: group_id N -> "Run_NNN" (zero-padded to 3).
std::string run_label(std::int64_t gid) {
    char b[24]; std::snprintf(b, sizeof b, "Run_%03lld", static_cast<long long>(gid)); return b;
}

// A "group" is one machine running one OS — the container for its runs. Identity is the
// HARDWARE + OS: (cpu, cores, ram_gb, os). The hostname is NOT part of the key — it's recorded
// as the anonymized jac313-<group_id> label (so matching on it would desync), and "same
// hardware + same OS" is exactly what we want to call the same group. Internally it's group_id
// (NOT NULL); externally it renders as Run_NNN. Older DBs get the column added and every row
// backfilled — one id per (hardware, os) identity, first-seen.
void ensure_group_schema(jac313::Qlite::v002::Sqlite& db) {
    std::int64_t has = 0;
    { auto st = db.prepare("SELECT COUNT(*) FROM pragma_table_info('bench_run') WHERE name='group_id'");
      if (st.step()) st.get(has); }
    if (has == 0) db.exec("ALTER TABLE bench_run ADD COLUMN group_id INTEGER");
    struct Id { std::string cpu; std::int64_t cores, ram; std::string os; };
    std::vector<Id> pending;
    { auto st = db.prepare("SELECT cpu, cores, ram_gb, os FROM bench_run WHERE group_id IS NULL "
                           "GROUP BY cpu, cores, ram_gb, os ORDER BY MIN(id)");
      while (st.step()) { Id id; st.get(id.cpu, id.cores, id.ram, id.os); pending.push_back(id); } }
    for (const auto& id : pending) {
        std::int64_t next = 1;
        { auto st = db.prepare("SELECT COALESCE(MAX(group_id),0)+1 FROM bench_run"); if (st.step()) st.get(next); }
        db.exec("UPDATE bench_run SET group_id=? WHERE cpu=? AND cores=? AND ram_gb=? AND os=? "
                "AND group_id IS NULL", next, id.cpu, id.cores, id.ram, id.os);
    }
}

// testType: a small reference/dimension table naming the runner's test kinds, so results can be
// categorized by type. Created + seeded idempotently (CREATE IF NOT EXISTS + INSERT OR IGNORE), so
// it's safe to call on every DB open and never disturbs existing rows.
void ensure_testtype_schema(jac313::Qlite::v002::Sqlite& db) {
    db.exec("CREATE TABLE IF NOT EXISTS testType ("
            "id INTEGER PRIMARY KEY, name TEXT UNIQUE NOT NULL, description TEXT)");
    db.exec("INSERT OR IGNORE INTO testType(name, description) VALUES"
            " ('ctest',       'ctest unit suite'),"
            " ('smoke',       'persist x output smoke matrix'),"
            " ('bench',       'throughput benchmark suite'),"
            " ('verify-lite', 'valgrind memcheck gate'),"
            " ('verify',      'valgrind memcheck + helgrind + DRD')");

    // testList: the catalog of individual tests (id + name). store_bench seeds its own 7 bench
    // suite configs here; jac313_test_cli seeds the discovered ctest/smoke names. INSERT OR IGNORE
    // means both sources merge and re-runs are no-ops. (The @10M durable runs reuse jtext/sql/binary
    // with a larger event count — same test, a scale parameter, so they are not separate entries.)
    db.exec("CREATE TABLE IF NOT EXISTS testList ("
            "id INTEGER PRIMARY KEY, name TEXT UNIQUE NOT NULL)");
    db.exec("INSERT OR IGNORE INTO testList(name) VALUES"
            " ('0 flags, non-durable'),"
            " ('2 flags, non-durable'),"
            " ('4 flags, non-durable'),"
            " ('6 flags, non-durable'),"
            " ('durable jtext'),"
            " ('durable sql'),"
            " ('durable binary')");

    // Link results to their type: bench_run.test_type_id -> testType.id. Add the column on older
    // DBs and backfill existing rows to 'bench' (the only kind that records today). Idempotent;
    // guarded on bench_run existing (a brand-new DB creates the column in CREATE TABLE).
    std::int64_t has_tbl = 0;
    { auto st = db.prepare("SELECT COUNT(*) FROM sqlite_master WHERE type='table' AND name='bench_run'");
      if (st.step()) st.get(has_tbl); }
    if (has_tbl == 0) return;
    std::int64_t has_col = 0;
    { auto st = db.prepare("SELECT COUNT(*) FROM pragma_table_info('bench_run') WHERE name='test_type_id'");
      if (st.step()) st.get(has_col); }
    if (has_col == 0) db.exec("ALTER TABLE bench_run ADD COLUMN test_type_id INTEGER");
    db.exec("UPDATE bench_run SET test_type_id=(SELECT id FROM testType WHERE name='bench') "
            "WHERE test_type_id IS NULL");
}

// Resolve the group_id for this machine's (hardware, os) identity: a plain compare against the
// recorded groups — reuse the match, else allocate the next. No hostname, no side registry.
std::int64_t group_id_for(jac313::Qlite::v002::Sqlite& db, const HostInfo& h) {
    std::int64_t gid = 0;
    { auto st = db.prepare("SELECT group_id FROM bench_run WHERE cpu=? AND cores=? AND ram_gb=? AND os=? "
                           "AND group_id IS NOT NULL LIMIT 1");
      st.bind(h.cpu, h.cores, h.ram_gb, h.os); if (st.step()) st.get(gid); }
    if (gid == 0) { auto st = db.prepare("SELECT COALESCE(MAX(group_id),0)+1 FROM bench_run"); if (st.step()) st.get(gid); }
    return gid;
}

// External label for a group: jac313-<group_id> (zero-padded 3), matching --anonymize.
std::string host_label_for(std::int64_t gid) {
    char b[24]; std::snprintf(b, sizeof b, "jac313-%03lld", static_cast<long long>(gid)); return b;
}

// ============================================================================
// results.db — the unified, normalized results database (run / parameter / testRun + the
// testType / testList catalogs). store_bench writes here IN ADDITION to the legacy bench_run
// table during the port-over. It's a sibling file next to the bench DB (results.db).
// ============================================================================
std::filesystem::path results_db_path(const std::string& bench_db_path) {
    namespace fs = std::filesystem;
    const fs::path p(bench_db_path);
    return (p.has_parent_path() ? p.parent_path() : fs::path(".")) / "results.db";
}

void ensure_results_schema(jac313::Qlite::v002::Sqlite& db) {
    db.exec("CREATE TABLE IF NOT EXISTS testType (id INTEGER PRIMARY KEY, name TEXT UNIQUE NOT NULL, description TEXT)");
    db.exec("INSERT OR IGNORE INTO testType(name, description) VALUES"
            " ('ctest','ctest unit suite'),('smoke','persist x output smoke matrix'),"
            " ('bench','throughput benchmark suite'),('verify-lite','valgrind memcheck gate'),"
            " ('verify','valgrind memcheck + helgrind + DRD')");
    db.exec("CREATE TABLE IF NOT EXISTS testList (id INTEGER PRIMARY KEY, name TEXT UNIQUE NOT NULL)");
    db.exec("INSERT OR IGNORE INTO testList(name) VALUES"
            " ('0 flags, non-durable'),('2 flags, non-durable'),('4 flags, non-durable'),"
            " ('6 flags, non-durable'),('durable jtext'),('durable sql'),('durable binary')");
    db.exec("CREATE TABLE IF NOT EXISTS compiler ("
            "id INTEGER PRIMARY KEY, name TEXT, version TEXT, major INTEGER, UNIQUE(name, version))");
    db.exec("CREATE TABLE IF NOT EXISTS parameter ("
            "id INTEGER PRIMARY KEY, compiler_id INTEGER, build_type TEXT, modules TEXT, size TEXT, "
            "persist TEXT, output_mode TEXT, threads INTEGER, events_per_thread INTEGER, runs INTEGER, "
            "batch INTEGER, flag_count INTEGER, "
            "UNIQUE(compiler_id,build_type,modules,size,persist,output_mode,threads,events_per_thread,runs,batch,flag_count))");
    db.exec("CREATE TABLE IF NOT EXISTS run ("
            "run_id INTEGER PRIMARY KEY, ts_utc TEXT, group_id INTEGER, host TEXT, cpu TEXT, cores INTEGER, "
            "ram_gb INTEGER, os TEXT, store_ver TEXT, qlite_ver TEXT, jtext_ver TEXT)");
    db.exec("CREATE TABLE IF NOT EXISTS testRun ("
            "id INTEGER PRIMARY KEY, run_id INTEGER, test_type_id INTEGER, test_list_id INTEGER, parameter_id INTEGER, "
            "status TEXT, duration_ms INTEGER, median_ops INTEGER, low_ops INTEGER, high_ops INTEGER, "
            "avg_ops INTEGER, stddev_ops INTEGER, bytes INTEGER)");
}

// group_id in results.db is resolved from the run table by hardware+os identity (same rule as
// bench_run's group_id_for), so a machine keeps its number across the two databases.
std::int64_t results_group_id(jac313::Qlite::v002::Sqlite& db, const HostInfo& h) {
    std::int64_t gid = 0;
    { auto st = db.prepare("SELECT group_id FROM run WHERE cpu=? AND cores=? AND ram_gb=? AND os=? LIMIT 1");
      st.bind(h.cpu, h.cores, h.ram_gb, h.os); if (st.step()) st.get(gid); }
    if (gid == 0) { auto st = db.prepare("SELECT COALESCE(MAX(group_id),0)+1 FROM run"); if (st.step()) st.get(gid); }
    return gid;
}

std::int64_t results_next_run_id(jac313::Qlite::v002::Sqlite& db) {
    std::int64_t n = 1;
    { auto st = db.prepare("SELECT COALESCE(MAX(run_id),0)+1 FROM run"); if (st.step()) st.get(n); }
    return n;
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
    std::int64_t id = 0; auto st = db.prepare(sql); st.bind(name); if (st.step()) st.get(id); return id;
}

// Insert-or-find the compiler dimension row (name, exact version, major); UNIQUE(name,version) dedupes.
std::int64_t compiler_id_for(jac313::Qlite::v002::Sqlite& db, const CompilerInfo& c) {
    db.exec("INSERT OR IGNORE INTO compiler(name, version, major) VALUES(?,?,?)", c.name, c.version, c.major);
    std::int64_t id = 0;
    { auto st = db.prepare("SELECT id FROM compiler WHERE name=? AND version=?");
      st.bind(c.name, c.version); if (st.step()) st.get(id); }
    return id;
}

// Insert-or-find a bench parameter combo (build_type Release, modules off; size/output_mode/batch
// N/A => NULL). SELECT-then-INSERT, because a UNIQUE over NULL-bearing columns won't dedupe in SQLite.
std::int64_t parameter_id_bench(jac313::Qlite::v002::Sqlite& db, std::int64_t compiler_id,
                                const std::string& persist, std::int64_t threads, std::int64_t events,
                                std::int64_t runs, std::int64_t flags) {
    const char* where =
        "SELECT id FROM parameter WHERE compiler_id=? AND build_type='Release' AND modules='off' AND size IS NULL "
        "AND persist=? AND output_mode IS NULL AND threads=? AND events_per_thread=? AND runs=? AND batch IS NULL "
        "AND flag_count=?";
    auto find = [&]() -> std::int64_t {
        std::int64_t id = 0; auto st = db.prepare(where);
        st.bind(compiler_id, persist, threads, events, runs, flags); if (st.step()) st.get(id); return id;
    };
    std::int64_t id = find();
    if (id == 0) {
        db.exec("INSERT INTO parameter(compiler_id,build_type,modules,size,persist,output_mode,threads,"
                "events_per_thread,runs,batch,flag_count) VALUES(?, 'Release','off', NULL, ?, NULL, ?, ?, ?, NULL, ?)",
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
        db.exec("INSERT OR IGNORE INTO run(run_id, ts_utc, group_id, host, cpu, cores, ram_gb, os, "
                "store_ver, qlite_ver, jtext_ver) VALUES(?, strftime('%Y-%m-%dT%H:%M:%SZ','now'), ?,?,?,?,?,?, ?,?,?)",
                run_id, group_id, h.host, h.cpu, h.cores, h.ram_gb, h.os,
                std::string(jac313::Store::v002::version()), std::string(jac313::Qlite::v002::version()), p.jtext_ver);
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

void record_to_db(const Params& p, const BenchSummary& s) {
    if (p.db_path.empty()) return;
    try {
        jac313::Qlite::v002::Sqlite db(p.db_path);
        db.exec(R"SQL(
CREATE TABLE IF NOT EXISTS bench_run (
    id                INTEGER PRIMARY KEY AUTOINCREMENT,
    group_id          INTEGER NOT NULL,
    test_type_id      INTEGER,
    ts_utc            TEXT NOT NULL,
    host TEXT, cpu TEXT, cores INTEGER, ram_gb INTEGER, os TEXT, compiler TEXT,
    store_ver TEXT, qlite_ver TEXT, jtext_ver TEXT,
    label TEXT, threads INTEGER, events_per_thread INTEGER, persist TEXT, flag_count INTEGER,
    runs INTEGER, events INTEGER, bytes INTEGER,
    median_ops INTEGER, high_ops INTEGER, low_ops INTEGER, avg_ops INTEGER, stddev_ops INTEGER
)
)SQL");
        ensure_group_schema(db);          // migrate older DBs: add + backfill group_id
        ensure_testtype_schema(db);       // reference table of test kinds
        HostInfo h = sense_host();
        const std::int64_t group_id = group_id_for(db, h);
        // Record the anonymized jac313-<group_id> as host (no real hostname in the committed DB)
        // unless an explicit host_label.local / $JAC313_HOST_LABEL is pinned.
        if (host_label_override().empty()) h.host = host_label_for(group_id);
        const std::int64_t bytes = (p.persist == "none") ? 0
                                 : static_cast<std::int64_t>(measure_output_bytes(p.base_name));
        db.exec(
            "INSERT INTO bench_run(group_id, test_type_id, ts_utc, host, cpu, cores, ram_gb, os, compiler, store_ver, qlite_ver, jtext_ver, "
            "label, threads, events_per_thread, persist, flag_count, runs, events, bytes, "
            "median_ops, high_ops, low_ops, avg_ops, stddev_ops) VALUES("
            "?, (SELECT id FROM testType WHERE name='bench'), strftime('%Y-%m-%dT%H:%M:%SZ','now'), ?,?,?,?,?, ?, ?,?,?, ?,?,?,?,?, ?,?, ?, ?,?,?,?,?)",
            group_id, h.host, h.cpu, h.cores, h.ram_gb, h.os, compiler_id(),
            std::string(jac313::Store::v002::version()),
            std::string(jac313::Qlite::v002::version()),
            p.jtext_ver,
            p.label,
            static_cast<std::int64_t>(p.threads),
            static_cast<std::int64_t>(p.events_per_thread),
            p.persist,
            static_cast<std::int64_t>(set_flag_count(p.flags)),
            static_cast<std::int64_t>(s.runs),
            static_cast<std::int64_t>(s.events_per_run),
            bytes,
            static_cast<std::int64_t>(s.median),
            static_cast<std::int64_t>(s.high),
            static_cast<std::int64_t>(s.low),
            static_cast<std::int64_t>(s.avg),
            static_cast<std::int64_t>(s.stddev));
        std::cerr << "[db] recorded 1 row (" << run_label(group_id) << ") -> " << p.db_path << "\n";
    } catch (const std::exception& e) {
        std::cerr << "[db] record failed: " << e.what() << "\n";
    }
    record_to_results_db(p, s);   // also write the normalized results.db (port-over target)
}
#else
void record_to_db(const Params&, const BenchSummary&) {}   // built without SQL persist -> no DB
#endif

// ---- shared formatting ----
std::string commafy(std::uint64_t n) {
    std::string s = std::to_string(n), out; int c = 0;
    for (int i = static_cast<int>(s.size()) - 1; i >= 0; --i) {
        out.push_back(s[i]);
        if (++c % 3 == 0 && i > 0) out.push_back(',');
    }
    std::reverse(out.begin(), out.end());
    return out;
}
std::string milstr(std::uint64_t n) { char b[32]; std::snprintf(b, sizeof b, "%.2fM", n / 1e6); return b; }
std::string milint(std::uint64_t n) { return std::to_string(n / 1000000) + "M"; }   // clean "10M" for headers

// ---- the curated suite (replaces the old bench_suite.sh driver) ----
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

#ifdef JAC313_STORE_HAS_SQL_PERSIST
// ---- render the results page from the DB (replaces the old bench_report.py) ----
std::string bytes_human(std::int64_t b) {
    char buf[32];
    if (b >= 1024 * 1024) std::snprintf(buf, sizeof buf, "%.1f MB", b / (1024.0 * 1024.0));
    else if (b >= 1024)   std::snprintf(buf, sizeof buf, "%.1f KB", b / 1024.0);
    else                  std::snprintf(buf, sizeof buf, "%lld B", static_cast<long long>(b));
    return buf;
}

// one <td> cell: the speed table (Flags or Backend) for a single group+compiler.
// GitHub renders the inner markdown table because of the blank lines around it.
void emit_speed_cell(jac313::Qlite::v002::Sqlite& db, std::ostream& out, std::int64_t gid,
                     const std::string& compiler, bool durable, std::int64_t dtotal = 0) {
    const std::string gq = std::to_string(gid);
    const std::string cq = "'" + compiler + "'";
    out << "<td>\n\n**" << compiler << "**\n\n";
    if (durable) {
        const std::string ev = std::to_string(dtotal);
        out << "| Backend | Median Ops/Sec | Band (Low–High) |\n|---|--:|---|\n";
        auto st = db.prepare(
            "SELECT persist,median_ops,low_ops,high_ops FROM bench_run b WHERE group_id=" + gq +
            " AND compiler=" + cq + " AND persist<>'none' AND threads*events_per_thread=" + ev +
            " AND ts_utc=(SELECT MAX(ts_utc) FROM bench_run WHERE group_id=b.group_id AND compiler=b.compiler "
            "AND persist=b.persist AND threads*events_per_thread=" + ev + ") "
            "ORDER BY median_ops DESC");
        while (st.step()) {
            std::string p; std::int64_t med, low, high; st.get(p, med, low, high);
            out << "| " << p << " | " << commafy(med) << " | " << milstr(low) << " – " << milstr(high) << " |\n";
        }
    } else {
        out << "| Flags | Median Ops/Sec | Band (Low–High) |\n|---|--:|---|\n";
        auto st = db.prepare(
            "SELECT flag_count,median_ops,low_ops,high_ops FROM bench_run b WHERE group_id=" + gq +
            " AND compiler=" + cq + " AND persist='none' AND ts_utc=(SELECT MAX(ts_utc) FROM bench_run "
            "WHERE group_id=b.group_id AND compiler=b.compiler AND persist='none' AND flag_count=b.flag_count) "
            "ORDER BY median_ops DESC");
        while (st.step()) {
            std::int64_t fc, med, low, high; st.get(fc, med, low, high);
            out << "| " << fc << " | " << commafy(med) << " | " << milstr(low) << " – " << milstr(high) << " |\n";
        }
    }
    out << "\n</td>\n";
}

// the per-run (per-group) detail page body: hardware/versions + the result tables.
void render_run_detail(jac313::Qlite::v002::Sqlite& db, std::ostream& out, std::int64_t gid) {
    const std::string gq = std::to_string(gid);
    out << "# Store benchmark results — " << run_label(gid) << "\n\n"
           "_Generated from `bench_results.db` by `store_bench --report`._<br>\n"
           "_(median + low–high band over N runs; timed **end-to-end** — store buffer allocation through flush, front edge included)._\n\n"
           "[← back to index](README.md)\n\n";
    { auto st = db.prepare("SELECT host,cpu,cores,ram_gb,os,store_ver,qlite_ver,jtext_ver,ts_utc "
                           "FROM bench_run WHERE group_id=" + gq + " ORDER BY ts_utc DESC LIMIT 1");
      if (st.step()) {
          std::string host, cpu, os, sv, qv, jv, ts; std::int64_t cores = 0, ram = 0;
          st.get(host, cpu, cores, ram, os, sv, qv, jv, ts);
          out << "| | |\n|---|---|\n"
              << "| **Host** | " << host << " |\n"
              << "| **Hardware** | " << cpu << " · " << cores << " cores · " << ram << " GB · " << os << " |\n"
              << "| **Versions** | Store " << sv << " · Qlite " << qv << " · jText " << jv << " |\n"
              << "| **Latest run (UTC)** | " << ts << " |\n\n";
      }
    }
    std::vector<std::string> comps;
    { auto st = db.prepare("SELECT DISTINCT compiler FROM bench_run WHERE group_id=" + gq + " ORDER BY compiler");
      while (st.step()) { std::string c; st.get(c); comps.push_back(c); } }
    auto counts = [&](const char* clause) -> std::pair<std::uint64_t, std::int64_t> {
        auto st = db.prepare("SELECT threads,events_per_thread,runs FROM bench_run WHERE group_id=" + gq +
                             " AND " + clause + " ORDER BY ts_utc DESC LIMIT 1");
        if (st.step()) { std::int64_t t, e, r; st.get(t, e, r); return {static_cast<std::uint64_t>(t) * e, r}; }
        return {0, 0};
    };
    // --- non-durable: side-by-side per compiler ---
    { auto [tot, runs] = counts("persist='none'");
      if (tot) {
          out << "### Flag-overhead (non-durable) — " << milint(tot) << " Events × " << runs
              << " Runs<br>That's " << milint(tot * static_cast<std::uint64_t>(runs)) << " Events per config\n\n"
              << "<table><tr>\n";
          for (const auto& c : comps) emit_speed_cell(db, out, gid, c, false);
          out << "</tr></table>\n\n";
      }
    }
    // --- durable: one side-by-side grid PER event-size (scaling: 1M vs 10M) ---
    { std::vector<std::pair<std::int64_t, std::int64_t>> dsz;
      { auto st = db.prepare("SELECT DISTINCT threads*events_per_thread AS tot, runs FROM bench_run "
                             "WHERE group_id=" + gq + " AND persist<>'none' ORDER BY tot");
        while (st.step()) { std::int64_t t, r; st.get(t, r); dsz.push_back({t, r}); } }
      for (const auto& [tot, runs] : dsz) {
          out << "### Durable — " << milint(static_cast<std::uint64_t>(tot)) << " Events × " << runs
              << " Runs<br>That's " << milint(static_cast<std::uint64_t>(tot) * static_cast<std::uint64_t>(runs))
              << " Events per config\n\n<table><tr>\n";
          for (const auto& c : comps) emit_speed_cell(db, out, gid, c, true, tot);
          out << "</tr></table>\n\n";
      }
    }
    // --- persisted size: per backend × event-size (compiler-independent) ---
    { auto st = db.prepare(
          "SELECT persist, threads*events_per_thread AS tot, bytes FROM bench_run b WHERE group_id=" + gq +
          " AND persist<>'none' AND ts_utc=(SELECT MAX(ts_utc) FROM bench_run WHERE group_id=b.group_id "
          "AND persist=b.persist AND threads*events_per_thread=b.threads*b.events_per_thread) "
          "GROUP BY persist, tot ORDER BY tot, bytes ASC");
      bool first = true;
      while (st.step()) {
          std::string p; std::int64_t tot, by; st.get(p, tot, by);
          if (first) {
              out << "### Persisted size — jText vs SQL vs binary (same across compilers)\n\n"
                  << "| Backend | Events | On-disk size |\n|---|--:|--:|\n";
              first = false;
          }
          out << "| " << p << " | " << milint(static_cast<std::uint64_t>(tot)) << " | " << bytes_human(by) << " |\n";
      }
      if (!first) out << "\n";
    }
}

// --report: write the summary index (README.md) + one Run_NNN.md detail page per group,
// into out_dir (defaults to the directory holding the DB).
int report_from_db(const std::string& db_path, const std::string& out_dir) {
    try {
        namespace fs = std::filesystem;
        jac313::Qlite::v002::Sqlite db(db_path);
        ensure_group_schema(db);
        ensure_testtype_schema(db);
        struct G { std::int64_t gid = 0, cores = 0, ram = 0, max_ops = 0; std::string host, os, cpu, sv, ts; };
        std::vector<G> groups;
        { auto st = db.prepare(
              "SELECT group_id, host, os, cpu, cores, ram_gb, store_ver, MAX(ts_utc), MAX(median_ops) "
              "FROM bench_run GROUP BY group_id ORDER BY group_id");
          while (st.step()) {
              G g; st.get(g.gid, g.host, g.os, g.cpu, g.cores, g.ram, g.sv, g.ts, g.max_ops);
              groups.push_back(g);
          } }
        const fs::path dir = out_dir.empty() ? fs::path(".") : fs::path(out_dir);

        // --- summary index: README.md ---
        { std::ofstream idx(dir / "README.md");
          if (!idx) { std::cerr << "report failed: cannot write " << (dir / "README.md").string() << "\n"; return 1; }
          idx << "# Store benchmark results\n\n"
                 "_Generated from `bench_results.db` by `store_bench --report` — the curated suite._<br>\n"
                 "_One row per run group (a machine running an OS); open a Run for the full tables._\n\n";
          if (groups.empty()) { idx << "_No runs recorded yet._\n"; }
          else {
              idx << "| group_id | HW Details | Run | Max Ops/Sec |\n|--:|---|---|--:|\n";
              for (const auto& g : groups) {
                  const std::string rl = run_label(g.gid);
                  idx << "| " << g.gid << " | " << g.cpu << " · " << g.cores << "c · " << g.ram
                      << " GB · " << g.os << " | [" << rl << "](" << rl << ".md) | "
                      << commafy(g.max_ops) << " |\n";
              }
          }
        }
        std::cerr << "[report] wrote " << (dir / "README.md").string()
                  << " (" << groups.size() << " run group(s))\n";

        // --- one detail page per group ---
        for (const auto& g : groups) {
            const std::string rl = run_label(g.gid);
            std::ofstream det(dir / (rl + ".md"));
            if (!det) { std::cerr << "report failed: cannot write " << (dir / (rl + ".md")).string() << "\n"; return 1; }
            render_run_detail(db, det, g.gid);
            std::cerr << "[report] wrote " << (dir / (rl + ".md")).string() << "\n";
        }

        // --- prune stale detail pages: a Run_NNN.md whose group no longer exists
        // (e.g. it was fully --cleared) would otherwise linger as a dead link target.
        auto is_current = [&](const std::string& nm) {
            for (const auto& g : groups) if (run_label(g.gid) + ".md" == nm) return true;
            return false;
        };
        for (const auto& e : fs::directory_iterator(dir)) {
            if (!e.is_regular_file()) continue;
            const std::string nm = e.path().filename().string();
            bool is_run = nm.rfind("Run_", 0) == 0 && nm.size() > 7 && nm.compare(nm.size() - 3, 3, ".md") == 0;
            for (std::size_t i = 4; is_run && i + 3 < nm.size(); ++i)
                if (nm[i] < '0' || nm[i] > '9') is_run = false;
            if (is_run && !is_current(nm)) {
                std::error_code ec; fs::remove(e.path(), ec);
                if (!ec) std::cerr << "[report] removed stale " << nm << "\n";
            }
        }
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "report failed: " << e.what() << "\n"; return 1;
    }
}
#else
int report_from_db(const std::string&, const std::string&) { std::cerr << "built without SQL persist -> no report\n"; return 1; }
#endif

#ifdef JAC313_STORE_HAS_SQL_PERSIST
// ---- clear recorded runs so a re-run starts clean ----
// Scope is always THIS machine's (hardware, OS) group: it never touches another machine's
// numbers, and — crucially — a re-image / dual-boot (different OS on the same box, e.g.
// Fedora -> RHEL) keeps the other OS's rows (os is part of the identity). There is deliberately
// no "clear everything" flag; a full wipe is a manual, intentional act. A missing table is a
// no-op, not an error.
int clear_db(const std::string& db_path) {
    try {
        jac313::Qlite::v002::Sqlite db(db_path);
        { auto st = db.prepare("SELECT name FROM sqlite_master WHERE type='table' AND name='bench_run'");
          if (!st.step()) { std::cerr << "[clear] no bench_run table in " << db_path << " — nothing to clear\n"; return 0; } }
        const HostInfo h = sense_host();
        const char* where = "cpu=? AND cores=? AND ram_gb=? AND os=?";
        std::int64_t n = 0;
        { auto st = db.prepare(std::string("SELECT COUNT(*) FROM bench_run WHERE ") + where);
          st.bind(h.cpu, h.cores, h.ram_gb, h.os); if (st.step()) st.get(n); }
        db.exec(std::string("DELETE FROM bench_run WHERE ") + where, h.cpu, h.cores, h.ram_gb, h.os);
        std::cerr << "[clear] removed " << n << " row(s) for this machine ("
                  << h.cpu << " / " << h.cores << "c / " << h.os << ") from " << db_path << "\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "clear failed: " << e.what() << "\n"; return 1;
    }
}
#else
int clear_db(const std::string&) { std::cerr << "built without SQL persist -> no clear\n"; return 1; }
#endif

#ifdef JAC313_STORE_HAS_SQL_PERSIST
// Scrub identifying hostnames: replace each row's host with its stable jac313-<group_id>
// label (zero-padded 3). group_id IS the (hardware, hostname, os) identity, so every
// distinct machine gets a distinct label and no real hostname remains in a committed DB.
// Idempotent (group_id is stable). Caller re-renders the report afterwards.
int anonymize_db(const std::string& db_path) {
    try {
        jac313::Qlite::v002::Sqlite db(db_path);
        { auto st = db.prepare("SELECT name FROM sqlite_master WHERE type='table' AND name='bench_run'");
          if (!st.step()) { std::cerr << "[anonymize] no bench_run table in " << db_path << " — nothing to do\n"; return 0; } }
        ensure_group_schema(db);   // guarantee every row has its identity's group_id first
        db.exec("UPDATE bench_run SET host = printf('jac313-%03d', group_id)");
        std::int64_t n = 0;
        { auto st = db.prepare("SELECT COUNT(DISTINCT group_id) FROM bench_run"); if (st.step()) st.get(n); }
        std::cerr << "[anonymize] hostnames -> jac313-<group_id> (" << n << " machine-identities) in " << db_path << "\n";
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "anonymize failed: " << e.what() << "\n"; return 1;
    }
}
#else
int anonymize_db(const std::string&) { std::cerr << "built without SQL persist -> no anonymize\n"; return 1; }
#endif

#ifdef JAC313_STORE_HAS_SQL_PERSIST
// Read-only precheck: list the existing groups (group_id | host | hw | os | max ops/sec) and this
// machine's identity + the group_id a record WOULD land on — so you can see up front whether
// recording reuses an existing group (adds rows; `--clear` wipes it) or creates a brand-new one.
// Writes NOTHING: no DB row, no registry append.
int group_id_precheck(const std::string& db_path_in) {
    namespace fs = std::filesystem;
    const std::string db_path = db_path_in.empty() ? "bench_results.db" : db_path_in;
    std::cout << "=== bench group precheck (read-only) ===\nDB: " << db_path << "\n\n";

    HostInfo h = sense_host();
    const std::string override = host_label_override();
    std::int64_t db_max = 0, proposed = 0;
    bool reuse = false, resolved = false, have_rows = false;

    std::cout << "Existing groups:\n";
    std::cout << "  gid | host         | hardware                         | os                       |   max ops/sec\n";
    std::cout << "  ----+--------------+----------------------------------+--------------------------+--------------\n";
    try {
        if (fs::exists(db_path)) {
            jac313::Qlite::v002::Sqlite db(db_path);
            bool has_tbl = false;
            { auto st = db.prepare("SELECT 1 FROM sqlite_master WHERE type='table' AND name='bench_run'");
              has_tbl = st.step(); }
            if (has_tbl) {
                ensure_group_schema(db);
                ensure_testtype_schema(db);
                { auto st = db.prepare("SELECT group_id, host, cpu, cores, ram_gb, os, MAX(median_ops) "
                                       "FROM bench_run GROUP BY group_id ORDER BY group_id");
                  while (st.step()) {
                      std::int64_t gid = 0, cores = 0, ram = 0, mx = 0; std::string host, cpu, os;
                      st.get(gid, host, cpu, cores, ram, os, mx);
                      if (gid > db_max) db_max = gid;
                      have_rows = true;
                      const std::string hw = cpu + " (" + std::to_string(cores) + "c/" +
                                             std::to_string(ram) + "G)";
                      char line[400];
                      std::snprintf(line, sizeof line, "  %3lld | %-12.12s | %-32.32s | %-24.24s | %13lld\n",
                                    (long long)gid, host.c_str(), hw.c_str(), os.c_str(), (long long)mx);
                      std::cout << line;
                  } }
                proposed = group_id_for(db, h);    // the same hardware+os compare recording uses
                reuse = (proposed <= db_max);
                resolved = true;
            }
        }
    } catch (const std::exception& e) {
        std::cout << "  (could not read DB: " << e.what() << ")\n";
    }
    if (!have_rows) std::cout << "  (none yet)\n";
    if (!resolved) { proposed = 1; reuse = false; }   // no DB / no table yet -> this is group 1

    const std::string label = override.empty() ? host_label_for(proposed) : override;
    std::cout << "\nThis machine:\n"
              << "  cpu:   " << h.cpu << "\n"
              << "  cores: " << h.cores << "    ram_gb: " << h.ram_gb << "\n"
              << "  os:    " << h.os << "\n"
              << "  recorded as: " << label
              << (override.empty() ? "   (matched on cpu + cores + ram + os; hostname not used)\n"
                                   : "   (pinned via host_label.local / $JAC313_HOST_LABEL)\n");

    std::cout << "\nProposed group_id: " << proposed << "   (" << label << ")\n";
    std::cout << (reuse
        ? "  -> REUSES an existing group: recording ADDS rows to it; `store_bench --clear` wipes it first.\n"
        : "  -> NEW group: recording creates a fresh group; nothing existing is touched.\n");
    return 0;
}
#else
int group_id_precheck(const std::string&) { std::cerr << "built without SQL persist -> no group precheck\n"; return 1; }
#endif

} // namespace

int main(int argc, char** argv) {
    Params p;
    p.flags = default_flags();
    std::optional<Sweep> sweep;
    std::string out_dir;   // --out: where --report writes README.md + Run_NNN.md (default: the DB's dir)
    bool do_suite = false, do_report = false, dry = false, do_clear = false, smoke = false, do_anon = false;
    bool do_group_id = false;

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
        else if (a == "--out") out_dir = next();
        else if (a == "--label") p.label = next();
        else if (a == "--jtext-ver") p.jtext_ver = next();
        else if (a == "--suite") do_suite = true;
        else if (a == "--smoke") { do_suite = true; smoke = true; }   // smoke = the suite as a pass/fail gate
        else if (a == "--report") do_report = true;
        else if (a == "--anonymize") do_anon = true;   // scrub hostnames -> jac313-<group_id>, then re-render
        else if (a == "--group-id") do_group_id = true; // read-only: show groups + this machine's proposed id
        else if (a == "--clear") do_clear = true;
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

    if (do_group_id) return group_id_precheck(p.db_path);

    if (do_report || do_anon) {
        const std::string dbp = p.db_path.empty() ? "bench_results.db" : p.db_path;
        if (do_anon) { if (int rc = anonymize_db(dbp); rc != 0) return rc; }   // scrub, then re-render below
        // default output dir = the directory holding the DB (so test-summary/bench_results.db
        // writes test-summary/README.md + test-summary/Run_NNN.md). --out overrides.
        std::string od = out_dir;
        if (od.empty()) od = std::filesystem::path(dbp).parent_path().string();
        return report_from_db(dbp, od);
    }
    if (do_clear && !dry) {   // wipe THIS host+OS first so a following --suite re-measures clean
        if (int rc = clear_db(p.db_path.empty() ? "bench_results.db" : p.db_path); rc != 0) return rc;
    }
    if (do_suite)  return run_suite(p, dry, smoke);
    if (do_clear)  return 0;  // clear-only (no suite/single requested)

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
