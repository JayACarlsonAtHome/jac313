#include "fleet_registry.hpp"

#include <cstdlib>
#include <fstream>
#include <sstream>

#if JAC313_TEST_CLI_HAS_PG
#include <jac313/Store/v001/headers/persistence/PgClient.hpp>
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
    return jac313::Store::v001::pg_conninfo_redacted(r);
#else
    return r;
#endif
}

namespace {

#if JAC313_TEST_CLI_HAS_PG
std::string identity_where(const jac313::results::MachineId& h) {
    return "cpu = " + lit(h.cpu) + " AND cores = " + std::to_string(h.cores)
         + " AND ram_gb = " + std::to_string(h.ram_gb)
         + " AND disk = " + lit(h.disk) + " AND instance_hash = " + lit(h.instance_hash);
}
#endif

// Connect to the server, make sure fleet.machine exists, then run `body` inside one transaction
// that holds the table lock; `body` returns this machine's number (0 = refused, r.error says why).
template <class Body>
FleetResult with_registry(const std::filesystem::path& source_dir, Body&& body) {
    FleetResult r;
    const char* remote = remote_conninfo();
    if (!remote) {
        r.error = "no remote PG server set";
        return r;
    }
#if !JAC313_TEST_CLI_HAS_PG
    (void)source_dir; (void)body;
    r.error = "this jac313_test_cli was built without libpq 18+";
    return r;
#else
    const std::filesystem::path ddl_path = source_dir / "schema" / "fleet_v0_2.sql";
    const std::string ddl = read_file(ddl_path);
    if (ddl.empty()) {
        r.error = "missing " + ddl_path.string();
        return r;
    }
    std::string conninfo = remote;
    if (conninfo.find("connect_timeout") == std::string::npos)
        conninfo += " connect_timeout=5";   // an unreachable server must not stall every run
    try {
        jac313::Store::v001::PgConn c(conninfo);
        r.host = c.host();
        r.address = c.hostaddr().empty() ? c.host() : c.hostaddr();
        r.port = c.port();
        r.dbname = c.dbname();
        try {
            c.exec(ddl);
        } catch (const std::exception&) {
            c.exec(ddl);   // two machines making the schema at the same moment: the second try sees it
        }
        c.exec("BEGIN; LOCK TABLE fleet.machine IN SHARE ROW EXCLUSIVE MODE;");
        r.mid = body(c, r);
        c.exec(r.mid > 0 ? "COMMIT" : "ROLLBACK");
    } catch (const std::exception& e) {
        // the connection's destructor ends an open transaction (rolled back)
        r.mid = 0;
        r.error = e.what();
        while (!r.error.empty() && (r.error.back() == '\n' || r.error.back() == ' ')) r.error.pop_back();
    }
    return r;
#endif
}

#if JAC313_TEST_CLI_HAS_PG
// The first column of the first row as a number (0 when there is none).
template <class Rows>
std::int64_t first_id(const Rows& rows) {
    return (rows.empty() || rows[0].empty() || !rows[0][0]) ? 0 : std::stoll(*rows[0][0]);
}
#endif

} // namespace

FleetResult fleet_register(const jac313::results::MachineId& h, std::int64_t preferred,
                           const std::filesystem::path& source_dir) {
#if !JAC313_TEST_CLI_HAS_PG
    (void)h; (void)preferred;
    return with_registry(source_dir, [](auto&, FleetResult&) { return std::int64_t{0}; });
#else
    return with_registry(source_dir, [&](jac313::Store::v001::PgConn& c, FleetResult& r) {
        const std::string pref = std::to_string(preferred > 0 ? preferred : 0);
        const std::int64_t mid = first_id(c.query(
            "WITH hit AS (\n"
            "  UPDATE fleet.machine SET last_seen = now(), p_cores = " + std::to_string(h.p_cores)
            + ", cpu_mhz_min = " + std::to_string(h.cpu_mhz_min) + ", cpu_mhz_max = " + std::to_string(h.cpu_mhz_max)
            + "\n  WHERE " + identity_where(h) + " RETURNING id),\n"
            "ins AS (\n"
            "  INSERT INTO fleet.machine(id, cpu, cores, ram_gb, disk, instance_hash, "
            "p_cores, cpu_mhz_min, cpu_mhz_max)\n"
            "  SELECT CASE WHEN " + pref + " > 0 AND NOT EXISTS (SELECT 1 FROM fleet.machine WHERE id = "
            + pref + ")\n"
            "              THEN " + pref + " ELSE (SELECT COALESCE(MAX(id), 0) + 1 FROM fleet.machine) END,\n"
            "         " + lit(h.cpu) + ", " + std::to_string(h.cores) + ", " + std::to_string(h.ram_gb) + ", "
            + lit(h.disk) + ", " + lit(h.instance_hash) + ", " + std::to_string(h.p_cores)
            + ", " + std::to_string(h.cpu_mhz_min) + ", " + std::to_string(h.cpu_mhz_max) + "\n"
            "  WHERE NOT EXISTS (SELECT 1 FROM hit) RETURNING id)\n"
            "SELECT id FROM hit UNION ALL SELECT id FROM ins"));
        if (mid == 0) r.error = "the server returned no machine id";
        return mid;
    });
#endif
}

FleetResult fleet_assign(const jac313::results::MachineId& h, std::int64_t mid,
                         const std::filesystem::path& source_dir) {
#if !JAC313_TEST_CLI_HAS_PG
    (void)h; (void)mid;
    return with_registry(source_dir, [](auto&, FleetResult&) { return std::int64_t{0}; });
#else
    return with_registry(source_dir, [&](jac313::Store::v001::PgConn& c, FleetResult& r) -> std::int64_t {
        const std::string id = std::to_string(mid);
        // another machine on that number: refuse (it would lose its number)
        if (first_id(c.query("SELECT id FROM fleet.machine WHERE id = " + id + " AND NOT ("
                             + identity_where(h) + ")")) != 0) {
            r.error = jac313::results::host_label(mid) + " belongs to another machine on the shared server";
            return 0;
        }
        const std::int64_t moved = first_id(c.query(
            "UPDATE fleet.machine SET id = " + id + ", last_seen = now() WHERE " + identity_where(h)
            + " RETURNING id"));
        if (moved != 0) return moved;
        return first_id(c.query(
            "INSERT INTO fleet.machine(id, cpu, cores, ram_gb, disk, instance_hash, p_cores, cpu_mhz_min, "
            "cpu_mhz_max) VALUES (" + id + ", " + lit(h.cpu) + ", " + std::to_string(h.cores) + ", "
            + std::to_string(h.ram_gb) + ", " + lit(h.disk) + ", " + lit(h.instance_hash) + ", "
            + std::to_string(h.p_cores) + ", " + std::to_string(h.cpu_mhz_min) + ", "
            + std::to_string(h.cpu_mhz_max) + ") RETURNING id"));
    });
#endif
}

} // namespace jac313::test_cli
