#pragma once

#include <jac313/Store/v002.hpp>
#include <jac313/Store/v002/headers/impl_details/format_locale.hpp>
#include <jac313/Store/v002/headers/persistence/BinaryEventSink.hpp>
#include <jac313/Store/v002/headers/persistence/HtmlEventSink.hpp>
#include <jac313/Store/v002/headers/persistence/JsonEventSink.hpp>
#include <jac313/Store/v002/headers/persistence/JTextEventSink.hpp>

#include <algorithm>
#include <filesystem>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <string_view>

#ifdef JAC313_STORE_HAS_SQL_PERSIST
#include <jac313/Store/v002/headers/persistence/SqlEventSink.hpp>
#endif

#ifdef JAC313_STORE_HAS_PG_PERSIST
#include <jac313/Store/v002/headers/persistence/PgEventSink.hpp>
#endif

namespace jac313::Store::v002::matrix_test {

// Tail-reader tests (003/006) track stream IDs in a fixed-size ring.
inline constexpr size_t kTailReaderMaxTracked = 100'000;

inline void clamp_tail_reader_workload(size_t& threads,
                                       size_t& events_per_thread,
                                       const size_t max_tracked = kTailReaderMaxTracked)
{
    if (threads == 0 || events_per_thread == 0) {
        return;
    }
    const size_t total = threads * events_per_thread;
    if (total <= max_tracked) {
        return;
    }
    events_per_thread = std::max<size_t>(1, max_tracked / threads);
    if (threads * events_per_thread > max_tracked) {
        threads = std::max<size_t>(1, max_tracked / events_per_thread);
    }
    std::cerr << "NOTE: tail-reader workload clamped to "
              << threads << " x " << events_per_thread << " = "
              << (threads * events_per_thread) << " (cap " << max_tracked << ")\n";
}

inline size_t tail_reader_tracked_limit(const size_t total_events,
                                        const size_t max_tracked = kTailReaderMaxTracked)
{
    return std::min(total_events, max_tracked);
}

inline void consume_pending_stdin() {
    if (std::cin.rdbuf()->in_avail() > 0) {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
}

#ifdef JAC313_STORE_HAS_PG_PERSIST
// Timings of the PostgreSQL sink this process opened (connect, ping, per-batch COPY), for the
// READBACK line: they show local vs remote latency.
inline std::shared_ptr<PgTimings> g_pg_timings;
#endif

// verify labels the pure hot-path as "inmem"; smoke/matrix use "none" — same behavior.
inline bool persist_skips_sink(std::string_view ptype) {
    return ptype == "none" || ptype == "inmem";
}

inline std::unique_ptr<IEventSink> make_persistence_sink(const std::string& ptype,
                                                         const std::string& bname,
                                                         const size_t im,
                                                         const size_t dm,
                                                         const PersistMode mode = PersistMode::All,
                                                         const JTextRowLimits jtext_limits = {})
{
    if (ptype == "binary") {
        return std::make_unique<BinaryEventSink>(bname, im, dm, mode);
    }
    if (ptype == "html") {
        return std::make_unique<HtmlEventSink>(bname, im, dm, mode);
    }
    if (ptype == "json") {
        return std::make_unique<JsonEventSink>(bname, im, dm, mode);
    }
    if (ptype == "sql") {
#ifdef JAC313_STORE_HAS_SQL_PERSIST
        return std::make_unique<SqlEventSink>(bname, im, dm, mode, false);
#else
        std::cerr << "ERROR: SQL persistence not enabled at compile time\n";
        return nullptr;
#endif
    }
    if (ptype == "jtext") {
        return std::make_unique<JTextEventSink>(bname, im, dm, mode, jtext_limits);
    }
    if (ptype == "pg-local" || ptype == "pg-remote") {
#ifdef JAC313_STORE_HAS_PG_PERSIST
        try {
            g_pg_timings = std::make_shared<PgTimings>();
            return std::make_unique<PgEventSink>(ptype, bname, im, dm, mode, g_pg_timings);
        } catch (const std::exception& ex) {
            std::cerr << "ERROR: " << ptype << ": " << ex.what() << "\n";
            return nullptr;
        }
#else
        std::cerr << "ERROR: PostgreSQL persistence not enabled at compile time (needs libpq 18+)\n";
        return nullptr;
#endif
    }
    std::cerr << "ERROR: unknown persistence type: " << ptype << "\n";
    return nullptr;
}

/// One "TARGET role=... backend=... host=\"...\" address=\"...\" port=N dbname=\"...\" table=\"...\""
/// line per table / file sink `index` wrote (IEventSink::targets()). The test runner and the bench
/// parse these (jac313_results_db.hpp parse_target_lines) into results.db run_target rows. A file
/// (a file sink's path, SQLite's .db) is given as an absolute path, so it names the file wherever
/// the run was started from.
template<typename Store>
void print_targets(const Store& store, size_t index, std::string_view backend, std::string_view role) {
    auto q = [](std::string v) {
        for (auto& c : v) if (c == '"' || c == '\n') c = '\'';
        return "\"" + v + "\"";
    };
    auto abs = [](const std::string& path) {
        std::error_code ec;
        const auto a = std::filesystem::absolute(path, ec);
        return ec ? path : a.lexically_normal().string();
    };
    for (auto t : store.persistence_targets(index)) {
        if (t.host.empty()) {   // not a server: a file sink's file, or SQLite's .db
            if (t.dbname.empty()) t.table = abs(t.table);
            else t.dbname = abs(t.dbname);
        }
        std::cout << "TARGET role=" << role << " backend=" << backend << " host=" << q(t.host)
                  << " address=" << q(t.address) << " port=" << t.port << " dbname=" << q(t.dbname)
                  << " table=" << q(t.table) << "\n";
    }
}

/// The persistence batch (rows per sink call) every matrix test attaches its sink with. The test
/// prints it as "BATCH backend=<b> batch=<n>" (finish_persistence) and the test runner records it.
inline constexpr size_t kPersistBatch = 10'000;

template<typename Config, typename Store>
bool attach_persistence_from_opts(Store& prod, const TestOptions& opts) {
    std::string ptype = opts.persist.empty() ? "jtext" : opts.persist;
    std::string bname = opts.base_name.empty() ? "persist" : opts.base_name;

    if (persist_skips_sink(ptype)) {
        std::cout << "No persistence attached — pure in-memory hot path\n";
        return true;
    }

    const size_t im = Config::the_IntMetrics;
    const size_t dm = Config::the_DblMetrics;
    auto sink = make_persistence_sink(ptype, bname, im, dm, PersistMode::All, jtext_row_limits_for<Config>());
    if (!sink) return false;

    prod.attach_persistence(std::move(sink), kPersistBatch);
    prod.set_max_persist_rows(opts.max_persist_rows);
    return true;
}

} // namespace jac313::Store::v002::matrix_test

#include "readback.hpp"

namespace jac313::Store::v002::matrix_test {

/// After the run: drain + finalize persistence, then read the dataset back and compare it with
/// the in-memory rows (tests/matrix/readback.hpp). Prints the READBACK line; false on a mismatch.
template<typename Config, typename Store>
bool finish_persistence(Store& prod, const TestOptions& opts) {
    const std::string ptype = opts.persist.empty() ? "jtext" : opts.persist;
    if (persist_skips_sink(ptype)) return true;
    prod.finalize_persistence();
    std::cout << "BATCH backend=" << ptype << " batch=" << kPersistBatch << "\n";
    for (size_t i = 0; i < prod.persistence_count(); ++i) print_targets(prod, i, ptype, "persist");
    if (opts.readback == "off") return true;
    const std::string bname = opts.base_name.empty() ? "persist" : opts.base_name;
    const auto rep = run_readback(prod, ptype, bname, Config::the_IntMetrics, Config::the_DblMetrics,
                                  opts.readback, opts.pg_cleanup);
    std::string extra;
#ifdef JAC313_STORE_HAS_PG_PERSIST
    if (is_pg_backend(ptype) && g_pg_timings) {
        auto b = g_pg_timings->batch_ms;
        std::sort(b.begin(), b.end());
        extra = std::format(" connect_ms={:.3f} ping_ms={:.3f} batches={} batch_med_ms={:.3f} write_ms={:.3f}",
                            g_pg_timings->connect_ms, g_pg_timings->ping_median_ms, b.size(),
                            b.empty() ? 0.0 : b[b.size() / 2], g_pg_timings->write_total_ms);
    }
#endif
    print_readback(rep, extra);
    if (!rep.ok) std::cerr << "READBACK FAILED (" << ptype << "): " << rep.error << "\n";
    return rep.ok;
}

} // namespace jac313::Store::v002::matrix_test

namespace jac313::Store::v002 {
using matrix_test::finish_persistence;
using matrix_test::run_readback;
using matrix_test::print_readback;
using matrix_test::ReadbackReport;
#ifdef JAC313_STORE_HAS_PG_PERSIST
using matrix_test::g_pg_timings;
#endif
using matrix_test::attach_persistence_from_opts;
using matrix_test::make_persistence_sink;
using matrix_test::persist_skips_sink;
using matrix_test::print_targets;
}