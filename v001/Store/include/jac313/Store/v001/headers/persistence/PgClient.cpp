#include "PgClient.hpp"

#include <libpq-fe.h>

#include <algorithm>
#include <charconv>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <functional>
#include <format>
#include <stdexcept>
#include <filesystem>
#include <unistd.h>   // gethostname

namespace jac313::Store::v001 {

namespace {

double ms_since(std::chrono::steady_clock::time_point t0) {
    return std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - t0).count();
}

std::string sanitize_identifier(std::string_view s) {
    std::string out;
    out.reserve(s.size());
    for (char c : s) {
        if (c >= 'A' && c <= 'Z') out += static_cast<char>(c - 'A' + 'a');
        else if ((c >= 'a' && c <= 'z') || (c >= '0' && c <= '9')) out += c;
        else if (out.empty() || out.back() != '_') out += '_';
    }
    while (!out.empty() && out.back() == '_') out.pop_back();
    if (out.empty() || (out[0] >= '0' && out[0] <= '9')) out.insert(0, "t_");
    return out;
}

// RAII for PGresult.
struct ResultGuard {
    PGresult* r;
    ~ResultGuard() { if (r) PQclear(r); }
};

} // namespace

std::string pg_conninfo_redacted(std::string_view conninfo) {
    // "password=secret" / "password='a b'" -> "password=***" (keyword=value form only)
    std::string out(conninfo);
    for (std::size_t at = out.find("password"); at != std::string::npos; at = out.find("password", at + 1)) {
        std::size_t eq = at + 8;
        while (eq < out.size() && out[eq] == ' ') ++eq;
        if (eq >= out.size() || out[eq] != '=') continue;
        std::size_t v = eq + 1;
        while (v < out.size() && out[v] == ' ') ++v;
        std::size_t end = v;
        if (end < out.size() && out[end] == '\'') {
            for (++end; end < out.size() && out[end] != '\''; ++end) if (out[end] == '\\') ++end;
            if (end < out.size()) ++end;
        } else {
            while (end < out.size() && out[end] != ' ') ++end;
        }
        out.replace(v, end - v, "***");
    }
    return out;
}

std::string pg_conninfo_for(std::string_view backend) {
    const char* env = nullptr;
    std::string fallback;
    if (backend == "pg-remote") {
        env = std::getenv("JAC313_PG_REMOTE");
        // No default: a remote server is always the user's own choice, never a guess.
        if (!env || !*env)
            throw std::runtime_error("pg-remote: no remote PostgreSQL server set. Set it in Jac313Runner "
                                     "(Persist to > Remote PG), with jac313_test_cli --pg-remote "
                                     "\"host=... dbname=... user=...\", or in $JAC313_PG_REMOTE");
    } else {
        env = std::getenv("JAC313_PG_LOCAL");
        fallback = "host=localhost dbname=jac313 user=jac313 connect_timeout=10";
    }
    return (env && *env) ? std::string(env) : fallback;
}

std::string pg_table_base(std::string_view persist_base) {
    char host[256] = {};
    if (::gethostname(host, sizeof(host) - 1) != 0) host[0] = '\0';
    std::string_view h(host);
    h = h.substr(0, h.find('.'));   // short host name

    // last two path components: <test dir>_<file>
    std::string_view base = persist_base;
    while (!base.empty() && base.back() == '/') base.remove_suffix(1);
    const auto slash = base.rfind('/');
    std::string_view file = (slash == std::string_view::npos) ? base : base.substr(slash + 1);
    std::string_view parent;
    if (slash != std::string_view::npos) {
        std::string_view head = base.substr(0, slash);
        const auto s2 = head.rfind('/');
        parent = (s2 == std::string_view::npos) ? head : head.substr(s2 + 1);
    }

    // The full path tells apart runs the last two components share (version, compiler,
    // build type, gate), so its hash keeps one "last run" table per configuration.
    // FNV-1a, not std::hash: the name must be the same whichever compiler built the reader.
    // Hashed as an absolute, normalized path so the writer and the viewer agree.
    std::error_code ec;
    std::string full = std::filesystem::absolute(std::filesystem::path(persist_base), ec).lexically_normal().string();
    if (ec) full = std::string(persist_base);
    while (full.size() > 1 && full.back() == '/') full.pop_back();
    std::uint32_t hash = 2166136261u;
    for (unsigned char c : full) { hash ^= c; hash *= 16777619u; }

    std::string name = sanitize_identifier(std::format("{}_{}_{}", h, parent, file));
    constexpr std::size_t kMax = 63 - 7 - 9;   // room for "_<8 hex>" and "_floats"
    if (name.size() > kMax) name.resize(kMax);
    return std::format("{}_{:08x}", name, hash);
}

PgConn::PgConn(const std::string& conninfo) {
    const auto t0 = std::chrono::steady_clock::now();
    conn_ = PQconnectdb(conninfo.c_str());
    connect_ms_ = ms_since(t0);
    if (!conn_ || PQstatus(conn_) != CONNECTION_OK) {
        std::string msg = conn_ ? PQerrorMessage(conn_) : "out of memory";
        if (conn_) PQfinish(conn_);
        conn_ = nullptr;
        while (!msg.empty() && (msg.back() == '\n' || msg.back() == ' ')) msg.pop_back();
        throw std::runtime_error("PostgreSQL connect failed (" + pg_conninfo_redacted(conninfo) + "): " + msg);
    }
    // Quiet NOTICEs such as "table does not exist, skipping" from DROP ... IF EXISTS.
    exec("SET client_min_messages = warning");
}

PgConn::~PgConn() {
    if (conn_) PQfinish(conn_);
}

void PgConn::exec(const std::string& sql) {
    ResultGuard g{PQexec(conn_, sql.c_str())};
    const auto st = PQresultStatus(g.r);
    if (st != PGRES_COMMAND_OK && st != PGRES_TUPLES_OK) {
        throw std::runtime_error("PostgreSQL: " + std::string(PQerrorMessage(conn_)) + " [" + sql.substr(0, 200) + "]");
    }
}

std::vector<std::vector<std::optional<std::string>>> PgConn::query(const std::string& sql) {
    ResultGuard g{PQexec(conn_, sql.c_str())};
    if (PQresultStatus(g.r) != PGRES_TUPLES_OK) {
        throw std::runtime_error("PostgreSQL query: " + std::string(PQerrorMessage(conn_)) + " [" + sql.substr(0, 200) + "]");
    }
    const int rows = PQntuples(g.r);
    const int cols = PQnfields(g.r);
    std::vector<std::vector<std::optional<std::string>>> out(static_cast<std::size_t>(rows));
    for (int r = 0; r < rows; ++r) {
        auto& row = out[static_cast<std::size_t>(r)];
        row.reserve(static_cast<std::size_t>(cols));
        for (int c = 0; c < cols; ++c) {
            if (PQgetisnull(g.r, r, c)) row.emplace_back(std::nullopt);
            else row.emplace_back(std::string(PQgetvalue(g.r, r, c), static_cast<std::size_t>(PQgetlength(g.r, r, c))));
        }
    }
    return out;
}

void PgConn::copy_in(const std::string& copy_sql, std::string_view data) {
    {
        ResultGuard g{PQexec(conn_, copy_sql.c_str())};
        if (PQresultStatus(g.r) != PGRES_COPY_IN) {
            throw std::runtime_error("PostgreSQL COPY start: " + std::string(PQerrorMessage(conn_)));
        }
    }
    // Send in chunks (libpq accepts any size, but bounded chunks keep memory flat).
    constexpr std::size_t kChunk = 1 << 20;
    for (std::size_t off = 0; off < data.size(); off += kChunk) {
        const auto n = std::min(kChunk, data.size() - off);
        if (PQputCopyData(conn_, data.data() + off, static_cast<int>(n)) != 1) {
            throw std::runtime_error("PostgreSQL COPY data: " + std::string(PQerrorMessage(conn_)));
        }
    }
    if (PQputCopyEnd(conn_, nullptr) != 1) {
        throw std::runtime_error("PostgreSQL COPY end: " + std::string(PQerrorMessage(conn_)));
    }
    std::string err;
    while (PGresult* r = PQgetResult(conn_)) {
        if (PQresultStatus(r) != PGRES_COMMAND_OK && err.empty()) err = PQerrorMessage(conn_);
        PQclear(r);
    }
    if (!err.empty()) throw std::runtime_error("PostgreSQL COPY: " + err);
}

double PgConn::ping_median_ms(int n) {
    std::vector<double> t;
    t.reserve(static_cast<std::size_t>(n));
    for (int i = 0; i < n; ++i) {
        const auto t0 = std::chrono::steady_clock::now();
        query("SELECT 1");
        t.push_back(ms_since(t0));
    }
    std::sort(t.begin(), t.end());
    return t.empty() ? 0.0 : t[t.size() / 2];
}

void pg_copy_append_text(std::string& out, std::string_view v) {
    for (char c : v) {
        switch (c) {
            case '\\': out += "\\\\"; break;
            case '\t': out += "\\t";  break;
            case '\n': out += "\\n";  break;
            case '\r': out += "\\r";  break;
            default:   out += c;      break;
        }
    }
}

void pg_copy_append_double(std::string& out, double v) {
    if (std::isnan(v)) { out += "NaN"; return; }
    if (std::isinf(v)) { out += (v > 0 ? "Infinity" : "-Infinity"); return; }
    char buf[32];
    auto [p, ec] = std::to_chars(buf, buf + sizeof(buf), v);   // shortest exact round trip
    out.append(buf, p);
}

} // namespace jac313::Store::v001
