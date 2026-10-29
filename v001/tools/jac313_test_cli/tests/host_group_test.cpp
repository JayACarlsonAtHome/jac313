// Regression: a different OS on the same hardware must get a NEW group_id, not overwrite
// an existing host_spec row (the Fedora-at-002 / Linux-Mint-took-002 bug). Same hw+os with
// different instance_hash must also allocate a new group (or require --claim). current_host
// from another machine must not drive assignment — identity compare against host_spec comes first.

#include "jac313_results_db.hpp"

#include <jac313/Qlite/v001.hpp>

#include <cstdint>
#include <iostream>
#include <string>
#include <source_location>
#include <string_view>
#include <cstdlib>

using Sqlite = jac313::Qlite::v001::Sqlite;
using jac313::results::HostId;
using jac313::results::HostPinOptions;

namespace {

int failures = 0;

// A failed check says what and where, and counts toward failures.
void check(bool ok, std::string_view what, std::source_location at = std::source_location::current()) {
    if (ok) return;
    std::cerr << "FAIL: " << what << " at " << at.file_name() << ":" << at.line() << "\n";
    ++failures;
}

HostId fedora_i7(const std::string& hash = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa") {
    return {"Intel(R) Core(TM) i7-7820HQ CPU @ 2.90GHz", 8, 31, "fedora-44", "xssd", hash, 4, 2900, 3900};
}

HostId mint_i7(const std::string& hash = "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb") {
    return {"Intel(R) Core(TM) i7-7820HQ CPU @ 2.90GHz", 8, 31, "linuxmint-22.3", "xssd", hash, 4, 2900, 3900};
}

HostId fedora_vm2() {
    return fedora_i7("cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc");
}

std::string os_at(Sqlite& db, std::int64_t gid) {
    std::string os;
    auto st = db.prepare("SELECT os FROM host_spec WHERE group_id=?");
    st.bind(gid);
    if (st.step()) st.get(os);
    return os;
}

std::string hash_at(Sqlite& db, std::int64_t gid) {
    std::string hash;
    auto st = db.prepare("SELECT instance_hash FROM host_spec WHERE group_id=?");
    st.bind(gid);
    if (st.step()) st.get(hash);
    return hash;
}

} // namespace

int main() {
    Sqlite db(":memory:");
    jac313::results::ensure_schema(db);

    constexpr const char* kVer = "v001";
    const std::int64_t fedora_gid = jac313::results::pin_host(db, fedora_i7(), kVer);
    check(fedora_gid == 1, "fedora_gid == 1");
    check(os_at(db, 1) == "fedora-44", "os_at(db, 1) == \"fedora-44\"");

    jac313::results::set_current_host(db, fedora_gid, kVer);

    {
        const auto propose = jac313::results::propose_group_id(db, mint_i7(), false);
        check(propose.error == jac313::results::HostPinError::None, "propose.error == jac313::results::HostPinError::None");
        check(propose.gid == 2, "propose.gid == 2");
    }
    const std::int64_t mint_gid = jac313::results::pin_host(db, mint_i7(), kVer);
    check(mint_gid == 2, "mint_gid == 2");

    check(os_at(db, 1) == "fedora-44", "os_at(db, 1) == \"fedora-44\"");
    check(os_at(db, 2) == "linuxmint-22.3", "os_at(db, 2) == \"linuxmint-22.3\"");
    check(jac313::results::current_host(db, kVer) == 2, "jac313::results::current_host(db, kVer) == 2");

    // Same hw+os, different instance_hash -> ambiguous auto path.
    {
        Sqlite db2(":memory:");
        jac313::results::ensure_schema(db2);
        check(jac313::results::pin_host(db2, fedora_i7(), kVer) == 1, "jac313::results::pin_host(db2, fedora_i7(), kVer) == 1");
        const auto propose = jac313::results::propose_group_id(db2, fedora_vm2(), false);
        check(propose.error == jac313::results::HostPinError::Ambiguous, "propose.error == jac313::results::HostPinError::Ambiguous");
        check(!propose.ambiguous_candidates.empty(), "!propose.ambiguous_candidates.empty()");
        const auto assigned = jac313::results::resolve_and_pin(db2, fedora_vm2(), HostPinOptions{}, false, kVer);
        check(assigned.error == jac313::results::HostPinError::Ambiguous, "assigned.error == jac313::results::HostPinError::Ambiguous");
        HostPinOptions assign_opts;
        assign_opts.assign_new = 2;
        const auto created = jac313::results::resolve_and_pin(db2, fedora_vm2(), assign_opts, false, kVer);
        check(created.error == jac313::results::HostPinError::None, "created.error == jac313::results::HostPinError::None");
        check(created.gid == 2, "created.gid == 2");
        check(hash_at(db2, 2) == fedora_vm2().instance_hash, "hash_at(db2, 2) == fedora_vm2().instance_hash");
        const auto reclaim = jac313::results::resolve_and_pin(db2, fedora_i7(), HostPinOptions{"jac313-001"}, false, kVer);
        check(reclaim.error == jac313::results::HostPinError::None, "reclaim.error == jac313::results::HostPinError::None");
        check(reclaim.gid == 1, "reclaim.gid == 1");
    }

    // v001 and v002 pins are independent rows; same machine should share host_spec / group_id.
    {
        Sqlite db3(":memory:");
        jac313::results::ensure_schema(db3);
        check(jac313::results::pin_host(db3, fedora_i7(), "v001") == 1, "jac313::results::pin_host(db3, fedora_i7(), \"v001\") == 1");
        check(jac313::results::pin_host(db3, fedora_i7(), "v002") == 1, "jac313::results::pin_host(db3, fedora_i7(), \"v002\") == 1");
        check(jac313::results::current_host(db3, "v001") == 1, "jac313::results::current_host(db3, \"v001\") == 1");
        check(jac313::results::current_host(db3, "v002") == 1, "jac313::results::current_host(db3, \"v002\") == 1");
        jac313::results::set_current_host(db3, 99, "v002");
        check(jac313::results::current_host(db3, "v001") == 1, "jac313::results::current_host(db3, \"v001\") == 1");
        check(jac313::results::current_host(db3, "v002") == 99, "jac313::results::current_host(db3, \"v002\") == 99");
    }

    // claim fails on OS mismatch.
    {
        Sqlite db3(":memory:");
        jac313::results::ensure_schema(db3);
        jac313::results::pin_host(db3, fedora_i7(), kVer);
        const auto claim = jac313::results::claim_group(db3, mint_i7(), "jac313-001");
        check(claim.error == jac313::results::HostPinError::ClaimOsMismatch, "claim.error == jac313::results::HostPinError::ClaimOsMismatch");
    }

    // assign-new errors when taken.
    {
        Sqlite db4(":memory:");
        jac313::results::ensure_schema(db4);
        jac313::results::pin_host(db4, fedora_i7(), kVer);
        const auto assign = jac313::results::assign_new_group(db4, fedora_vm2(), 1);
        check(assign.error == jac313::results::HostPinError::AssignTaken, "assign.error == jac313::results::HostPinError::AssignTaken");
    }

    // renumber_group: the shared server says this machine is 3, not its local 1. Every table moves,
    // run.host is relabelled, and another machine's local rows already on 3 are moved out of the way.
    {
        namespace R = jac313::results;
        Sqlite db5(":memory:");
        R::ensure_schema(db5);
        R::pin_host(db5, fedora_i7(), kVer);                    // local 1
        R::upsert_host_spec(db5, 3, mint_i7());                 // another machine's local 3
        R::insert_run(db5, 10, 1, R::host_label(1), kVer, "s", "q", "j");
        R::insert_run(db5, 11, 3, R::host_label(3), kVer, "s", "q", "j");
        R::upsert_package_version(db5, 1, kVer, "Store", "v001.001");
        R::mark_fleet_synced(db5, 1, "srv", "t");
        R::renumber_group(db5, 1, 3);
        check(os_at(db5, 3) == "fedora-44", "os_at(db5, 3) == \"fedora-44\"");
        check(os_at(db5, 4) == "linuxmint-22.3", "os_at(db5, 4) == \"linuxmint-22.3\"");               // moved to the next free number
        check(!R::group_in_use(db5, 1), "!R::group_in_use(db5, 1)");
        check(R::current_host(db5, kVer) == 3, "R::current_host(db5, kVer) == 3");
        check(R::package_version_get(db5, 3, kVer, "Store") == "v001.001", "R::package_version_get(db5, 3, kVer, \"Store\") == \"v001.001\"");
        check(R::fleet_synced(db5, 3, "srv"), "R::fleet_synced(db5, 3, \"srv\")");
        check(!R::fleet_synced(db5, 3, "other-srv"), "!R::fleet_synced(db5, 3, \"other-srv\")");
        check(R::fleet_synced_within(db5, 3, "srv", 24) == false, "R::fleet_synced_within(db5, 3, \"srv\", 24) == false");   // "t" is no date: stale
        R::mark_fleet_synced(db5, 3, "srv", R::utc_now_iso());
        check(R::fleet_synced_within(db5, 3, "srv", 24), "R::fleet_synced_within(db5, 3, \"srv\", 24)");
        db5.exec("UPDATE fleet_sync SET ts_utc='2000-01-01T00:00:00Z' WHERE group_id=3");
        check(!R::fleet_synced_within(db5, 3, "srv", 24), "!R::fleet_synced_within(db5, 3, \"srv\", 24)");
        check(R::fleet_synced(db5, 3, "srv"), "R::fleet_synced(db5, 3, \"srv\")");
        std::string host10, host11;
        { auto st = db5.prepare("SELECT host FROM run WHERE run_id=10"); if (st.step()) st.get(host10); }
        { auto st = db5.prepare("SELECT host FROM run WHERE run_id=11"); if (st.step()) st.get(host11); }
        check(host10 == "jac313-003", "host10 == \"jac313-003\"");
        check(host11 == "jac313-004", "host11 == \"jac313-004\"");
        check(R::next_free_group_id(db5) == 5, "R::next_free_group_id(db5) == 5");
        R::renumber_group(db5, 3, 3);                           // no-op
        check(os_at(db5, 3) == "fedora-44", "os_at(db5, 3) == \"fedora-44\"");
    }

    if (failures == 0) {
        std::cout << "host group — OS/hash identity allocates distinct jac313-### rows\n";
        return 0;
    }
    std::cerr << failures << " check(s) failed\n";
    return 1;
}