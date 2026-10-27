#include "fleet_registry.hpp"

#include <cstdlib>
#include <fstream>
#include <sstream>

#if JAC313_TEST_CLI_HAS_PG
#include <jac313/Store/v002/headers/persistence/PgClient.hpp>
#endif

namespace jac313::test_cli {

namespace {

const char* remote_conninfo() {
    const char* r = std::getenv("JAC313_PG_REMOTE");
    return (r && *r) ? r : nullptr;
}

#if JAC313_TEST_CLI_HAS_PG
// A SQL string literal ('' doubles a quote; standard_conforming_strings keeps backslashes as-is).
std::string lit(const std::string& s) {
    std::string out = "'";
    for (char c : s) {
        if (c == '\'') out += '\'';
        out += c;
    }
    return out + "'";
}

std::string read_file(const std::filesystem::path& p) {
    std::ifstream in(p);
    std::ostringstream ss;
    ss << in.rdbuf();
    return ss.str();
}
#endif

} // namespace

std::string fleet_server_key() {
    const char* r = remote_conninfo();
    if (!r) return {};
#if JAC313_TEST_CLI_HAS_PG
    return jac313::Store::v002::pg_conninfo_redacted(r);
#else
    return r;
#endif
}

FleetResult fleet_register(const jac313::results::HostId& h, std::int64_t preferred,
                           const std::filesystem::path& source_dir) {
    FleetResult r;
    const char* remote = remote_conninfo();
    if (!remote) {
        r.error = "no remote PG server set";
        return r;
    }
#if !JAC313_TEST_CLI_HAS_PG
    (void)h; (void)preferred; (void)source_dir;
    r.error = "this jac313_test_cli was built without libpq 18+";
    return r;
#else
    const std::filesystem::path ddl_path = source_dir / "schema" / "fleet_v0_1.sql";
    const std::string ddl = read_file(ddl_path);
    if (ddl.empty()) {
        r.error = "missing " + ddl_path.string();
        return r;
    }
    std::string conninfo = remote;
    if (conninfo.find("connect_timeout") == std::string::npos)
        conninfo += " connect_timeout=5";   // an unreachable server must not stall every run
    try {
        jac313::Store::v002::PgConn c(conninfo);
        try {
            c.exec(ddl);
        } catch (const std::exception&) {
            c.exec(ddl);   // two machines making the schema at the same moment: the second try sees it
        }
        const std::string ident =
            "cpu = " + lit(h.cpu) + " AND cores = " + std::to_string(h.cores)
            + " AND ram_gb = " + std::to_string(h.ram_gb) + " AND os = " + lit(h.os)
            + " AND disk = " + lit(h.disk) + " AND instance_hash = " + lit(h.instance_hash);
        const std::string pref = std::to_string(preferred > 0 ? preferred : 0);
        c.exec("BEGIN; LOCK TABLE fleet.host_spec IN SHARE ROW EXCLUSIVE MODE;");
        const auto rows = c.query(
            "WITH hit AS (\n"
            "  UPDATE fleet.host_spec SET last_seen = now(), p_cores = " + std::to_string(h.p_cores)
            + ", cpu_mhz_min = " + std::to_string(h.cpu_mhz_min) + ", cpu_mhz_max = " + std::to_string(h.cpu_mhz_max)
            + "\n  WHERE " + ident + " RETURNING group_id),\n"
            "ins AS (\n"
            "  INSERT INTO fleet.host_spec(group_id, cpu, cores, ram_gb, os, disk, instance_hash, "
            "p_cores, cpu_mhz_min, cpu_mhz_max)\n"
            "  SELECT CASE WHEN " + pref + " > 0 AND NOT EXISTS (SELECT 1 FROM fleet.host_spec WHERE group_id = "
            + pref + ")\n"
            "              THEN " + pref + " ELSE (SELECT COALESCE(MAX(group_id), 0) + 1 FROM fleet.host_spec) END,\n"
            "         " + lit(h.cpu) + ", " + std::to_string(h.cores) + ", " + std::to_string(h.ram_gb) + ", "
            + lit(h.os) + ", " + lit(h.disk) + ", " + lit(h.instance_hash) + ", " + std::to_string(h.p_cores)
            + ", " + std::to_string(h.cpu_mhz_min) + ", " + std::to_string(h.cpu_mhz_max) + "\n"
            "  WHERE NOT EXISTS (SELECT 1 FROM hit) RETURNING group_id)\n"
            "SELECT group_id FROM hit UNION ALL SELECT group_id FROM ins");
        c.exec("COMMIT");
        if (rows.empty() || rows[0].empty() || !rows[0][0]) {
            r.error = "the server returned no group_id";
            return r;
        }
        r.gid = std::stoll(*rows[0][0]);
    } catch (const std::exception& e) {
        // the connection's destructor ends an open transaction (rolled back)
        r.error = e.what();
        while (!r.error.empty() && (r.error.back() == '\n' || r.error.back() == ' ')) r.error.pop_back();
    }
    return r;
#endif
}

} // namespace jac313::test_cli
