#pragma once

// PgClient.hpp
// Thin RAII wrapper over libpq for the PostgreSQL persistence sink and read-back.
// Built only with JAC313_STORE_PG_PERSIST (libpq 18+). Credentials come from libpq's
// own mechanisms (PGPASSFILE / ~/.pgpass); nothing in this repo holds a password.
//
// Backends:
//   pg-local   conninfo from $JAC313_PG_LOCAL,  default "host=localhost dbname=jac313 user=jac313"
//   pg-remote  conninfo from $JAC313_PG_REMOTE only: there is no default remote server. The
//              user names theirs (Jac313Runner's Remote PG fields, or jac313_test_cli
//              --pg-remote "host=... dbname=... user=..."), which sets this variable.
// Test tables live in schema store_test (never results.* or store.*).

#include <chrono>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

typedef struct pg_conn PGconn;   // from <libpq-fe.h>; kept out of this header

namespace jac313::Store::v002 {

inline constexpr std::string_view kPgTestSchema = "store_test";

/// True for the persist backend names served by PostgreSQL.
inline bool is_pg_backend(std::string_view backend) {
    return backend == "pg-local" || backend == "pg-remote";
}

/// libpq conninfo for a backend name ("pg-local" / "pg-remote"); env overrides the default.
/// pg-remote has no default: throws std::runtime_error when $JAC313_PG_REMOTE is unset.
std::string pg_conninfo_for(std::string_view backend);

/// The conninfo with any password value replaced by "***" (safe for logs, summaries, AI reports).
std::string pg_conninfo_redacted(std::string_view conninfo);

/// Table base name in store_test for a persist base path: "<host>_<parent dir>_<file>_<hash>",
/// lower-case, [a-z0-9_] only, short enough that "_floats" still fits PostgreSQL's
/// 63-byte identifier limit. The host part keeps machines that share the remote server
/// apart; <hash> (FNV-1a of the absolute path) keeps versions, compilers, build types and
/// gates apart, so each configuration keeps its own last run.
std::string pg_table_base(std::string_view persist_base);

/// Timing gathered by a connection, in milliseconds (for latency reporting).
struct PgTimings {
    double connect_ms = 0.0;
    double ping_median_ms = 0.0;          // median of `SELECT 1` round trips
    std::vector<double> batch_ms;         // one COPY transaction per write batch
    double write_total_ms = 0.0;
};

class PgConn {
public:
    explicit PgConn(const std::string& conninfo);   // throws std::runtime_error
    ~PgConn();
    PgConn(const PgConn&) = delete;
    PgConn& operator=(const PgConn&) = delete;

    /// Run one or more statements; throws on error.
    void exec(const std::string& sql);

    /// Run a query; every value as text (nullopt = SQL NULL).
    std::vector<std::vector<std::optional<std::string>>> query(const std::string& sql);

    /// COPY ... FROM STDIN with `data` in COPY text format; throws on error.
    void copy_in(const std::string& copy_sql, std::string_view data);

    /// Median round trip of `n` `SELECT 1` queries, in ms.
    double ping_median_ms(int n = 5);

    [[nodiscard]] double connect_ms() const noexcept { return connect_ms_; }

private:
    PGconn* conn_ = nullptr;
    double connect_ms_ = 0.0;
};

/// Append `v` to `out` in COPY text format (backslash, tab, newline, CR escaped).
void pg_copy_append_text(std::string& out, std::string_view v);

/// Append a double in COPY text format, exact (shortest round-trip), NaN/Infinity spelled for PG.
void pg_copy_append_double(std::string& out, double v);

} // namespace jac313::Store::v002
