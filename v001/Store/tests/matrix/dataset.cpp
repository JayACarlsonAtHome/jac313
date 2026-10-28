// jac313_store_dataset — print a persisted dataset as one jText table (for Jac313Runner's viewer).
//
//   jac313_store_dataset --backend <binary|jtext|html|json|sql|pg-local|pg-remote> --base <path> [--limit N]
//                        [--conninfo "host=... dbname=... user=..."]
//   jac313_store_dataset --ping <pg-local|pg-remote> [--conninfo "..."]
//
// --conninfo names the PostgreSQL server for a pg-* backend (else $JAC313_PG_LOCAL / _REMOTE).
// --ping only connects: it prints "PING ok connect_ms=.. ping_ms=.. server=.." or
// "PING failed: <reason>" (Jac313Runner's Test button) and exits 0 / 1.
//
// Reads the dataset with the same loaders read-back uses (tests/matrix/readback.hpp), joins the
// main rows with their _ints / _floats companions, and writes ONE jText section to stdout:
//   1. id  (the record id, field 1)   2. thread_id   3. per_thread_event_id   4. flags_raw
//   5. category   6. payload   7. timestamp_us   then int0.. and dbl0..
// The last line on stderr is "VALIDATION errors=E warnings=W" from jText's own validator run
// over that output. Exit status: 0 = printed, 1 = could not read the dataset, 2 = bad arguments.

#include "test_common.hpp"

#include "jText.h"
#include "parser/section.hpp"
#include "validator/validator.hpp"

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <format>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

using namespace jac313::Store::v001;
using namespace jac313::Store::v001::matrix_test;
using namespace jac313::Store::v001::matrix_test::readback_detail;
namespace fs = std::filesystem;

namespace {

// Metric column counts of a database companion table (0 when it does not exist).
size_t db_metric_columns(DbQuery& q, const std::string& table, std::string_view prefix, bool sqlite) {
    try {
        if (sqlite) {
            auto r = q.query(std::format("SELECT COUNT(*) FROM pragma_table_info('{}') WHERE name LIKE '{}%'",
                                         table, prefix));
            return r.empty() ? 0 : static_cast<size_t>(to_u64(r[0][0]));
        }
        const auto dot = table.find('.');
        auto r = q.query(std::format("SELECT COUNT(*) FROM information_schema.columns WHERE table_schema='{}' "
                                     "AND table_name='{}' AND column_name LIKE '{}%'",
                                     table.substr(0, dot), table.substr(dot + 1), prefix));
        return r.empty() ? 0 : static_cast<size_t>(to_u64(r[0][0]));
    } catch (...) {
        return 0;
    }
}

} // namespace

int main(int argc, char** argv) {
    std::string backend, base, conninfo, ping;
    size_t limit = 1000;
    for (int i = 1; i < argc; ++i) {
        std::string a = argv[i];
        auto next = [&]() -> std::string { return (i + 1 < argc) ? argv[++i] : std::string{}; };
        if (a == "--backend") backend = next();
        else if (a == "--base") base = next();
        else if (a == "--limit") limit = std::stoull(next());
        else if (a == "--conninfo") conninfo = next();
        else if (a == "--ping") ping = next();
        else {
            std::cerr << "usage: jac313_store_dataset --backend B --base PATH [--limit N] [--conninfo C]\n"
                         "       jac313_store_dataset --ping pg-local|pg-remote [--conninfo C]\n";
            return 2;
        }
    }
    // --conninfo wins over the environment for this process (and the libpq calls in it).
    const std::string pg_backend = ping.empty() ? backend : ping;
    if (!conninfo.empty() && (pg_backend == "pg-local" || pg_backend == "pg-remote"))
        ::setenv(pg_backend == "pg-remote" ? "JAC313_PG_REMOTE" : "JAC313_PG_LOCAL", conninfo.c_str(), 1);
    if (!ping.empty()) {
#ifdef JAC313_STORE_HAS_PG_PERSIST
        try {
            PgConn c(pg_conninfo_for(ping));
            const double ping_ms = c.ping_median_ms();
            auto v = c.query("SHOW server_version");
            const std::string server = (!v.empty() && v[0][0]) ? *v[0][0] : std::string("?");
            std::cout << std::format("PING ok connect_ms={:.1f} ping_ms={:.3f} server={}\n",
                                     c.connect_ms(), ping_ms, server);
            return 0;
        } catch (const std::exception& ex) {
            std::string msg = ex.what();
            for (char& ch : msg) if (ch == '\n') ch = ' ';
            std::cout << "PING failed: " << msg << "\n";
            return 1;
        }
#else
        std::cout << "PING failed: PostgreSQL support is not built in (libpq 18+ not found)\n";
        return 1;
#endif
    }
    if (backend.empty() || base.empty()) {
        std::cerr << "usage: jac313_store_dataset --backend B --base PATH [--limit N]\n";
        return 2;
    }

    RowSet rs;
    try {
        const bool has_i = fs::exists(base + "_Ints.html") || fs::exists(base + "_Ints.json") || fs::exists(base + "_Ints.jtext");
        const bool has_f = fs::exists(base + "_Floats.html") || fs::exists(base + "_Floats.json") || fs::exists(base + "_Floats.jtext");
        if (backend == "binary")     rs = load_binary(base);
        else if (backend == "json")  rs = load_json(base, has_i, has_f);
        else if (backend == "html")  rs = load_html(base, has_i, has_f);
        else if (backend == "jtext") rs = load_jtext(base, has_i, has_f);
        else if (backend == "sql" || backend == "pg-local" || backend == "pg-remote") {
            std::unique_ptr<DbQuery> q;
#ifdef JAC313_STORE_HAS_SQL_PERSIST
            if (backend == "sql") q = std::make_unique<SqliteQuery>(base);
#endif
#ifdef JAC313_STORE_HAS_PG_PERSIST
            if (backend != "sql") q = std::make_unique<PgQuery>(backend, base);
#endif
            if (!q) throw std::runtime_error(backend + " persistence not built in");
            const bool sqlite = backend == "sql";
            const size_t ic = db_metric_columns(*q, q->ints_table, "int", sqlite);
            const size_t dc = db_metric_columns(*q, q->dbls_table, "dbl", sqlite);
            rs = db_fetch(*q, ic, dc, std::format("WHERE id < {}", limit));
        } else {
            std::cerr << "unknown backend '" << backend << "'\n";
            return 2;
        }
    } catch (const std::exception& ex) {
        rs.error = ex.what();
    }
    if (!rs.error.empty()) {
        std::cerr << "cannot read " << backend << " dataset at " << base << ": " << rs.error << "\n";
        std::cerr << "VALIDATION errors=1 warnings=0\n";
        return 1;
    }

    std::sort(rs.rows.begin(), rs.rows.end(),
              [](const PersistedEvent& a, const PersistedEvent& b) { return a.event_id < b.event_id; });
    const size_t total = rs.rows.size();
    if (rs.rows.size() > limit) rs.rows.resize(limit);
    size_t ic = 0, dc = 0;
    for (const auto& r : rs.rows) { ic = std::max(ic, r.int_metrics.size()); dc = std::max(dc, r.dbl_metrics.size()); }

    // Written by the jText library's writer: header, Fields and Data blocks, closers, and a
    // formatter for each row that its data cannot clash with (U+25AE / U+258C by default).
    using namespace jac313::jText::v001;
    const std::string name = fs::path(base).filename().string();
    std::ostringstream out;
    out << "//File:    " << name << ".jtext\n"
        << "//Purpose: jac313 persisted dataset, shown by Jac313Runner\n"
        << "//\n";
    {
        JTextWriter w(out);
        w.set_profile(JTextProfile::Full);
        w.set_filename(name + ".jtext");
        w.set_purpose(std::format("{} dataset ({} rows read{})", backend, total,
                                  total > rs.rows.size() ? std::format(", first {} shown", rs.rows.size())
                                                         : std::string{}));
        w.write_header();
        w.begin_section(name);
        w.begin_fields();
        std::vector<std::string> fields = {"id/Number/Not Null", "thread_id/Number", "per_thread_event_id/Number",
                                           "flags_raw/String", "category/String", "payload/String", "timestamp_us/Number"};
        for (size_t k = 0; k < ic; ++k) fields.push_back(std::format("int{}/Number", k));
        for (size_t k = 0; k < dc; ++k) fields.push_back(std::format("dbl{}/Number", k));
        for (size_t f = 0; f < fields.size(); ++f) {
            std::vector<std::string> levels;
            for (std::string_view def = fields[f];;) {
                const auto slash = def.find('/');
                levels.emplace_back(def.substr(0, slash));
                if (slash == std::string_view::npos) break;
                def.remove_prefix(slash + 1);
            }
            w.append_row(f + 1, levels, "", U'/');
        }
        w.begin_data();
        for (const auto& r : rs.rows) {
            std::vector<std::string> row = {std::to_string(r.event_id), std::to_string(r.thread_id),
                                            std::to_string(r.per_thread_event_id), std::format("0x{:016x}", r.flags),
                                            r.category, r.payload, std::to_string(r.timestamp_us)};
            for (size_t k = 0; k < ic; ++k) row.push_back(std::to_string(k < r.int_metrics.size() ? r.int_metrics[k] : 0));
            for (size_t k = 0; k < dc; ++k) row.push_back(std::format("{}", k < r.dbl_metrics.size() ? r.dbl_metrics[k] : 0.0));
            w.append_row(1, row);
            w.end_record();
        }
        w.finalize();
    }

    const std::string text = out.str();
    std::cout << text;
    std::cout.flush();

    // Validate what we printed with jText's own parser + validator.
    size_t errors = 0, warnings = 0;
    auto pf = jtext::parse_file_structure(std::string_view(text));
    if (!pf) {
        std::cerr << "jText structure: " << pf.error().to_string() << "\n";
        errors = 1;
    } else {
        const auto vr = jtext::validate(*pf);
        errors = vr.report.error_count();
        warnings = vr.report.warning_count();
        size_t shown = 0;
        for (const auto& is : vr.report.issues) if (shown++ < 5) std::cerr << is.to_string() << "\n";
    }
    std::cerr << "VALIDATION errors=" << errors << " warnings=" << warnings << "\n";
    return 0;
}
