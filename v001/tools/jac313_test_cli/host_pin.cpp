#include "host_pin.hpp"

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
            const std::string num = arg.substr(15);
            if (!num.empty()) {
                flags.assign_new = std::stoll(num);
            }
        } else if (arg == "--assign-new" && i + 1 < argc) {
            flags.assign_new = std::stoll(argv[++i]);
        }
    }
    return flags;
}

jac313::results::HostPinOptions to_pin_options(const HostPinFlags& flags) {
    return jac313::results::HostPinOptions{flags.claim, flags.assign_new};
}

void print_fleet_table(std::ostream& out, jac313::Qlite::v001::Sqlite& db) {
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

int exit_ambiguous(std::ostream& err, jac313::Qlite::v001::Sqlite& db,
                   const jac313::results::HostPinResult& r, const jac313::results::HostId& h) {
    err << "\nFleet (host_spec):\n";
    print_fleet_table(err, db);
    err << '\n';
    print_ambiguity_help(err, r, h);
    return 1;
}

std::optional<std::int64_t> try_resolve_and_pin(jac313::Qlite::v001::Sqlite& db,
                                                const HostHardwareRecord& hw,
                                                const HostPinFlags& flags,
                                                const std::string& version,
                                                std::ostream& err) {
    const jac313::results::HostId h = make_host_id(hw);
    const bool generic = is_generic_nodename(nodename_for_hash());
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

jac313::results::HostPinResult propose_host_group(jac313::Qlite::v001::Sqlite& db,
                                                  const HostHardwareRecord& hw) {
    const jac313::results::HostId h = make_host_id(hw);
    return jac313::results::propose_group_id(db, h, is_generic_nodename(nodename_for_hash()));
}

} // namespace jac313::test_cli