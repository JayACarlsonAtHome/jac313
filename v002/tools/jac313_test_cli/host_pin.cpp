#include "host_pin.hpp"

#include "fleet_registry.hpp"

#include <cstdio>
#include <optional>
#include <string>

namespace jac313::test_cli {

HostPinFlags parse_host_pin_flags(int argc, char** argv, int start) {
    HostPinFlags flags;
    for (int i = start; i < argc; ++i) {
        const std::string arg = argv[i];
        if (arg == "--claim" && i + 1 < argc) {
            flags.claim = argv[++i];
        } else if (arg.rfind("--assign-new-", 0) == 0) {
            // Suffix after the "--assign-new-" prefix (length-derived, not a magic offset — the old
            // hardcoded 15 skipped two extra chars, so "--assign-new-42" parsed to "" / wrong group).
            const std::string num = arg.substr(std::string("--assign-new-").size());
            if (!num.empty()) {
                try { flags.assign_new = std::stoll(num); }
                catch (const std::exception&) { /* non-numeric suffix — leave assign_new unset */ }
            }
        } else if (arg == "--assign-new" && i + 1 < argc) {
            try { flags.assign_new = std::stoll(argv[++i]); }
            catch (const std::exception&) { /* non-numeric — leave assign_new unset */ }
        }
    }
    return flags;
}

jac313::results::HostPinOptions to_pin_options(const HostPinFlags& flags) {
    return jac313::results::HostPinOptions{flags.claim, flags.assign_new};
}

void print_fleet_table(std::ostream& out, jac313::Qlite::v002::Sqlite& db) {
    out << "  gid | jac313-### | cpu (cores/ram)              | disk | os              | hash\n"
        << "  ----+------------+------------------------------+------+-----------------+--------\n";
    auto st = db.prepare("SELECT group_id, cpu, cores, ram_gb, disk, os, instance_hash FROM host_spec "
                         "ORDER BY group_id");
    bool any = false;
    while (st.step()) {
        std::int64_t gid = 0, cores = 0, ram = 0;
        std::string cpu, disk, os, hash;
        st.get(gid, cpu, cores, ram, disk, os, hash);
        any = true;
        const std::string label = jac313::results::host_label(gid);
        const std::string hwd = cpu + " (" + std::to_string(cores) + "c/" + std::to_string(ram) + "G)";
        char line[512];
        std::snprintf(line, sizeof line, "  %3lld | %-10.10s | %-28.28s | %-4.4s | %-15.15s | %s\n",
                      static_cast<long long>(gid), label.c_str(), hwd.c_str(), disk.c_str(), os.c_str(),
                      jac313::results::hash_prefix(hash).c_str());
        out << line;
    }
    if (!any) {
        out << "  (none yet)\n";
    }
}

void print_ambiguity_help(std::ostream& out, const jac313::results::HostPinResult& r,
                          const jac313::results::HostId& h) {
    out << "Cannot auto-assign group_id: " << r.message << "\n"
        << "This machine instance_hash: " << jac313::results::hash_prefix(h.instance_hash) << "\n"
        << "Use --claim jac313-### to bind to an existing slot (same OS+hardware),\n"
        << "or --assign-new-### to create a new slot at an explicit number.\n";
}

int exit_ambiguous(std::ostream& err, jac313::Qlite::v002::Sqlite& db,
                   const jac313::results::HostPinResult& r, const jac313::results::HostId& h) {
    err << "\nFleet (host_spec):\n";
    print_fleet_table(err, db);
    err << '\n';
    print_ambiguity_help(err, r, h);
    return 1;
}

namespace {

// The shared server's number for this machine, pinned locally (local rows renumbered to it), or
// nullopt to fall back to local numbering: no server set, unreachable, or built without libpq.
std::optional<std::int64_t> pin_from_fleet(jac313::Qlite::v002::Sqlite& db,
                                           const jac313::results::HostId& h,
                                           const std::string& version,
                                           const std::filesystem::path& source_dir,
                                           std::ostream& err) {
    namespace R = jac313::results;
    const std::string server = fleet_server_key();
    if (server.empty()) return std::nullopt;
    const std::int64_t local = R::find_exact_group(db, h);
    if (local != 0 && R::fleet_synced(db, local, server)) {   // confirmed before: no round trip
        R::set_current_host(db, local, version);
        return local;
    }
    // one try per process: a run pins several times, and an unreachable server costs a timeout
    static bool tried = false;
    static FleetResult fr;
    if (!tried) {
        tried = true;
        // a new machine asks for the next number free here, so other machines' local rows stay put
        fr = fleet_register(h, local ? local : R::next_free_group_id(db), source_dir);
        if (fr.gid == 0)
            err << "[fleet] remote PG registry not used (" << fr.error << "); this machine keeps its local "
                << (local ? R::host_label(local) : std::string("number")) << " for now and is checked again next run\n";
    }
    if (fr.gid == 0) return std::nullopt;
    if (local != 0 && local != fr.gid) {
        R::renumber_group(db, local, fr.gid);
        err << "[fleet] this machine is " << R::host_label(fr.gid) << " on the shared server: local results moved from "
            << R::host_label(local) << "\n";
    } else if (local == fr.gid) {
        err << "[fleet] " << R::host_label(fr.gid) << " is registered on the shared server\n";
    } else {
        if (R::group_in_use(db, fr.gid))   // another machine's local rows sat on that number
            R::renumber_group(db, fr.gid, R::next_free_group_id(db));
        err << "[fleet] this machine is " << R::host_label(fr.gid) << " (registered on the shared server)\n";
    }
    R::upsert_host_spec(db, fr.gid, h);
    R::set_current_host(db, fr.gid, version);
    R::mark_fleet_synced(db, fr.gid, server, R::utc_now_iso());
    return fr.gid;
}

} // namespace

std::optional<std::int64_t> try_resolve_and_pin(jac313::Qlite::v002::Sqlite& db,
                                                const HostHardwareRecord& hw,
                                                const HostPinFlags& flags,
                                                const std::string& version,
                                                std::ostream& err,
                                                const std::filesystem::path& source_dir) {
    const jac313::results::HostId h = make_host_id(hw);
    const bool generic = is_generic_nodename(nodename_for_hash());
    // --claim / --assign-new and generic nodenames (no telling VMs apart) stay on the local path
    if (flags.claim.empty() && flags.assign_new == 0 && !generic) {
        if (const auto gid = pin_from_fleet(db, h, version, source_dir, err)) return gid;
    }
    const jac313::results::HostPinResult r =
        jac313::results::resolve_and_pin(db, h, to_pin_options(flags), generic, version);
    if (r.error == jac313::results::HostPinError::Ambiguous) {
        exit_ambiguous(err, db, r, h);
        return std::nullopt;
    }
    if (r.error != jac313::results::HostPinError::None) {
        err << "host pin failed: " << r.message << "\n";
        return std::nullopt;
    }
    return r.gid;
}

jac313::results::HostPinResult propose_host_group(jac313::Qlite::v002::Sqlite& db,
                                                  const HostHardwareRecord& hw) {
    const jac313::results::HostId h = make_host_id(hw);
    return jac313::results::propose_group_id(db, h, is_generic_nodename(nodename_for_hash()));
}

} // namespace jac313::test_cli