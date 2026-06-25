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

HostInfo sense_host() {
    HostInfo h;
    char hn[256] = {0};
    if (::gethostname(hn, sizeof(hn) - 1) == 0) h.host = hn;
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

void record_to_db(const Params& p, const BenchSummary& s) {
    if (p.db_path.empty()) return;
    try {
        jac313::Qlite::v002::Sqlite db(p.db_path);
        db.exec(R"SQL(
CREATE TABLE IF NOT EXISTS bench_run (
    id                INTEGER PRIMARY KEY AUTOINCREMENT,
    ts_utc            TEXT NOT NULL,
    host TEXT, cpu TEXT, cores INTEGER, ram_gb INTEGER, os TEXT, compiler TEXT,
    store_ver TEXT, qlite_ver TEXT, jtext_ver TEXT,
    label TEXT, threads INTEGER, events_per_thread INTEGER, persist TEXT, flag_count INTEGER,
    runs INTEGER, events INTEGER, bytes INTEGER,
    median_ops INTEGER, high_ops INTEGER, low_ops INTEGER, avg_ops INTEGER, stddev_ops INTEGER
)
)SQL");
        const HostInfo h = sense_host();
        const std::int64_t bytes = (p.persist == "none") ? 0
                                 : static_cast<std::int64_t>(measure_output_bytes(p.base_name));
        db.exec(
            "INSERT INTO bench_run(ts_utc, host, cpu, cores, ram_gb, os, compiler, store_ver, qlite_ver, jtext_ver, "
            "label, threads, events_per_thread, persist, flag_count, runs, events, bytes, "
            "median_ops, high_ops, low_ops, avg_ops, stddev_ops) VALUES("
            "strftime('%Y-%m-%dT%H:%M:%SZ','now'), ?,?,?,?,?, ?, ?,?,?, ?,?,?,?,?, ?,?, ?, ?,?,?,?,?)",
            h.host, h.cpu, h.cores, h.ram_gb, h.os, compiler_id(),
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
        std::cerr << "[db] recorded 1 row -> " << p.db_path << "\n";
    } catch (const std::exception& e) {
        std::cerr << "[db] record failed: " << e.what() << "\n";
    }
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

// ---- the curated 7-config suite (replaces the old bench_suite.sh driver) ----
int run_suite(Params base, bool dry) {
    base.threads = 50;                                 // curated suite is opinionated about threads
    struct Cfg { std::string persist; int flags; std::string label; std::size_t evt, runs; };
    std::vector<Cfg> cfgs;
    for (int n : {0, 2, 4, 6})
        cfgs.push_back({"none", n, std::to_string(n) + " flags, non-durable", 200000, 10});
    for (const char* pn : {"jtext", "sql", "binary"})
        cfgs.push_back({std::string(pn), 0, std::string("durable ") + pn, 20000, 3});

    if (dry) {
        std::cout << "# Store benchmark suite — 7 configs (run from the build dir)\n\n";
        for (const auto& cf : cfgs) {
            std::cout << "./jac313_store_bench --threads 50 --events-per-thread " << cf.evt
                      << " --runs " << cf.runs << " --persist " << cf.persist;
            if (cf.persist == "none") std::cout << " --flag-count " << cf.flags;
            std::cout << "   # " << cf.label << "\n";
        }
        return 0;
    }
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

// one <td> cell: the speed table (Flags or Backend) for a single host+compiler.
// GitHub renders the inner markdown table because of the blank lines around it.
void emit_speed_cell(jac313::Qlite::v002::Sqlite& db, const std::string& hq,
                     const std::string& compiler, bool durable) {
    const std::string cq = "'" + compiler + "'";
    std::cout << "<td>\n\n**" << compiler << "**\n\n";
    if (durable) {
        std::cout << "| Backend | Median Ops/Sec | Band (Low–High) |\n|---|--:|---|\n";
        auto st = db.prepare(
            "SELECT persist,median_ops,low_ops,high_ops FROM bench_run b WHERE host=" + hq +
            " AND compiler=" + cq + " AND persist<>'none' AND ts_utc=(SELECT MAX(ts_utc) FROM bench_run "
            "WHERE host=b.host AND compiler=b.compiler AND persist=b.persist) "
            "ORDER BY median_ops DESC");
        while (st.step()) {
            std::string p; std::int64_t med, low, high; st.get(p, med, low, high);
            std::cout << "| " << p << " | " << commafy(med) << " | " << milstr(low) << " – " << milstr(high) << " |\n";
        }
    } else {
        std::cout << "| Flags | Median Ops/Sec | Band (Low–High) |\n|---|--:|---|\n";
        auto st = db.prepare(
            "SELECT flag_count,median_ops,low_ops,high_ops FROM bench_run b WHERE host=" + hq +
            " AND compiler=" + cq + " AND persist='none' AND ts_utc=(SELECT MAX(ts_utc) FROM bench_run "
            "WHERE host=b.host AND compiler=b.compiler AND persist='none' AND flag_count=b.flag_count) "
            "ORDER BY median_ops DESC");
        while (st.step()) {
            std::int64_t fc, med, low, high; st.get(fc, med, low, high);
            std::cout << "| " << fc << " | " << commafy(med) << " | " << milstr(low) << " – " << milstr(high) << " |\n";
        }
    }
    std::cout << "\n</td>\n";
}

int report_from_db(const std::string& db_path) {
    try {
        jac313::Qlite::v002::Sqlite db(db_path);
        std::vector<std::string> hosts;
        { auto st = db.prepare("SELECT DISTINCT host FROM bench_run ORDER BY host");
          while (st.step()) { std::string h; st.get(h); hosts.push_back(h); } }
        std::cout << "# Store benchmark results\n\n"
                     "_Generated from `bench_results.db` by `store_bench --report` — the curated suite_<br>\n"
                     "_(median + low–high band over N runs; timed **end-to-end** — store buffer allocation through flush, front edge included)._<br>\n"
                     "_Latest run per config, per host, per compiler._\n\n";
        if (hosts.empty()) { std::cout << "_No runs recorded yet._\n"; return 0; }
        for (const auto& host : hosts) {
            const std::string hq = "'" + host + "'";   // trusted (our own gethostname)
            { auto st = db.prepare("SELECT cpu,cores,ram_gb,os,store_ver,qlite_ver,jtext_ver,ts_utc "
                                   "FROM bench_run WHERE host=" + hq + " ORDER BY ts_utc DESC LIMIT 1");
              if (st.step()) {
                  std::string cpu, os, sv, qv, jv, ts; std::int64_t cores = 0, ram = 0;
                  st.get(cpu, cores, ram, os, sv, qv, jv, ts);
                  std::cout << "## " << host << "\n\n| | |\n|---|---|\n"
                            << "| **Hardware** | " << cpu << " · " << cores << " cores · " << ram << " GB · " << os << " |\n"
                            << "| **Versions** | Store " << sv << " · Qlite " << qv << " · jText " << jv << " |\n"
                            << "| **Latest run (UTC)** | " << ts << " |\n\n";
              }
            }
            std::vector<std::string> comps;
            { auto st = db.prepare("SELECT DISTINCT compiler FROM bench_run WHERE host=" + hq + " ORDER BY compiler");
              while (st.step()) { std::string c; st.get(c); comps.push_back(c); } }
            auto counts = [&](const char* clause) -> std::pair<std::uint64_t, std::int64_t> {
                auto st = db.prepare("SELECT threads,events_per_thread,runs FROM bench_run WHERE host=" + hq +
                                     " AND " + clause + " ORDER BY ts_utc DESC LIMIT 1");
                if (st.step()) { std::int64_t t, e, r; st.get(t, e, r); return {static_cast<std::uint64_t>(t) * e, r}; }
                return {0, 0};
            };
            // --- non-durable: side-by-side per compiler ---
            { auto [tot, runs] = counts("persist='none'");
              if (tot) {
                  std::cout << "### Flag-overhead (non-durable) — " << milint(tot) << " Events × " << runs
                            << " Runs<br>That's " << milint(tot * static_cast<std::uint64_t>(runs)) << " Events per config\n\n"
                            << "<table><tr>\n";
                  for (const auto& c : comps) emit_speed_cell(db, hq, c, false);
                  std::cout << "</tr></table>\n\n";
              }
            }
            // --- durable: side-by-side per compiler ---
            { auto [tot, runs] = counts("persist<>'none'");
              if (tot) {
                  std::cout << "### Durable — " << milint(tot) << " Events × " << runs
                            << " Runs<br>That's " << milint(tot * static_cast<std::uint64_t>(runs)) << " Events per config\n\n"
                            << "<table><tr>\n";
                  for (const auto& c : comps) emit_speed_cell(db, hq, c, true);
                  std::cout << "</tr></table>\n\n";
              }
            }
            // --- persisted size: ONE table (compiler-independent) ---
            { auto st = db.prepare(
                  "SELECT persist, bytes FROM bench_run b WHERE host=" + hq + " AND persist<>'none' "
                  "AND ts_utc=(SELECT MAX(ts_utc) FROM bench_run WHERE host=b.host AND persist=b.persist) "
                  "GROUP BY persist ORDER BY bytes DESC");
              bool first = true;
              while (st.step()) {
                  std::string p; std::int64_t by; st.get(p, by);
                  if (first) {
                      std::cout << "### Persisted size — jText vs SQL vs binary (same across compilers)\n\n"
                                << "| Backend | On-disk size |\n|---|--:|\n";
                      first = false;
                  }
                  std::cout << "| " << p << " | " << bytes_human(by) << " |\n";
              }
              if (!first) std::cout << "\n";
            }
        }
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "report failed: " << e.what() << "\n"; return 1;
    }
}
#else
int report_from_db(const std::string&) { std::cerr << "built without SQL persist -> no report\n"; return 1; }
#endif

} // namespace

int main(int argc, char** argv) {
    Params p;
    p.flags = default_flags();
    std::optional<Sweep> sweep;
    bool do_suite = false, do_report = false, dry = false;

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
        else if (a == "--report") do_report = true;
        else if (a == "--dry-run") dry = true;
        else if (a == "--sweep") { sweep = parse_sweep(next()); if (!sweep) { std::cerr << "bad --sweep (want AXIS=LO..HI:STEP)\n"; return 2; } }
        else if (!arg_value(a, "--sweep").empty()) { sweep = parse_sweep(arg_value(a, "--sweep")); }
    }

    if (do_report) return report_from_db(p.db_path.empty() ? "bench_results.db" : p.db_path);
    if (do_suite)  return run_suite(p, dry);

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
