// Regression: a different OS on the same hardware must get a NEW group_id, not overwrite
// an existing host_spec row (the Fedora-at-002 / Linux-Mint-took-002 bug). Same hw+os with
// different instance_hash must also allocate a new group (or require --claim). current_host
// from another machine must not drive assignment — identity compare against host_spec comes first.

#include "jac313_results_db.hpp"

#include <jac313/Qlite/v001.hpp>

#include <cstdint>
#include <iostream>
#include <string>

using Sqlite = jac313::Qlite::v001::Sqlite;
using jac313::results::HostId;
using jac313::results::HostPinOptions;

namespace {

int failures = 0;

#define CHECK(cond)                                                            \
    do {                                                                       \
        if (!(cond)) {                                                         \
            std::cerr << "FAIL: " #cond " at " << __FILE__ << ":" << __LINE__  \
                      << "\n";                                                  \
            ++failures;                                                        \
        }                                                                      \
    } while (0)

HostId fedora_i7(const std::string& hash = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa") {
    return {"Intel(R) Core(TM) i7-7820HQ CPU @ 2.90GHz", 8, 31, "fedora-44", "ssd", hash, 4, 2900, 3900};
}

HostId mint_i7(const std::string& hash = "bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb") {
    return {"Intel(R) Core(TM) i7-7820HQ CPU @ 2.90GHz", 8, 31, "linuxmint-22.3", "ssd", hash, 4, 2900, 3900};
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
    CHECK(fedora_gid == 1);
    CHECK(os_at(db, 1) == "fedora-44");

    jac313::results::set_current_host(db, fedora_gid, kVer);

    {
        const auto propose = jac313::results::propose_group_id(db, mint_i7(), false);
        CHECK(propose.error == jac313::results::HostPinError::None);
        CHECK(propose.gid == 2);
    }
    const std::int64_t mint_gid = jac313::results::pin_host(db, mint_i7(), kVer);
    CHECK(mint_gid == 2);

    CHECK(os_at(db, 1) == "fedora-44");
    CHECK(os_at(db, 2) == "linuxmint-22.3");
    CHECK(jac313::results::current_host(db, kVer) == 2);

    // Same hw+os, different instance_hash -> ambiguous auto path.
    {
        Sqlite db2(":memory:");
        jac313::results::ensure_schema(db2);
        CHECK(jac313::results::pin_host(db2, fedora_i7(), kVer) == 1);
        const auto propose = jac313::results::propose_group_id(db2, fedora_vm2(), false);
        CHECK(propose.error == jac313::results::HostPinError::Ambiguous);
        CHECK(!propose.ambiguous_candidates.empty());
        const auto assigned = jac313::results::resolve_and_pin(db2, fedora_vm2(), HostPinOptions{}, false, kVer);
        CHECK(assigned.error == jac313::results::HostPinError::Ambiguous);
        HostPinOptions assign_opts;
        assign_opts.assign_new = 2;
        const auto created = jac313::results::resolve_and_pin(db2, fedora_vm2(), assign_opts, false, kVer);
        CHECK(created.error == jac313::results::HostPinError::None);
        CHECK(created.gid == 2);
        CHECK(hash_at(db2, 2) == fedora_vm2().instance_hash);
        const auto reclaim = jac313::results::resolve_and_pin(db2, fedora_i7(), HostPinOptions{"jac313-001"}, false, kVer);
        CHECK(reclaim.error == jac313::results::HostPinError::None);
        CHECK(reclaim.gid == 1);
    }

    // v001 and v002 pins are independent rows; same machine should share host_spec / group_id.
    {
        Sqlite db3(":memory:");
        jac313::results::ensure_schema(db3);
        CHECK(jac313::results::pin_host(db3, fedora_i7(), "v001") == 1);
        CHECK(jac313::results::pin_host(db3, fedora_i7(), "v002") == 1);
        CHECK(jac313::results::current_host(db3, "v001") == 1);
        CHECK(jac313::results::current_host(db3, "v002") == 1);
        jac313::results::set_current_host(db3, 99, "v002");
        CHECK(jac313::results::current_host(db3, "v001") == 1);
        CHECK(jac313::results::current_host(db3, "v002") == 99);
    }

    // claim fails on OS mismatch.
    {
        Sqlite db3(":memory:");
        jac313::results::ensure_schema(db3);
        jac313::results::pin_host(db3, fedora_i7(), kVer);
        const auto claim = jac313::results::claim_group(db3, mint_i7(), "jac313-001");
        CHECK(claim.error == jac313::results::HostPinError::ClaimOsMismatch);
    }

    // assign-new errors when taken.
    {
        Sqlite db4(":memory:");
        jac313::results::ensure_schema(db4);
        jac313::results::pin_host(db4, fedora_i7(), kVer);
        const auto assign = jac313::results::assign_new_group(db4, fedora_vm2(), 1);
        CHECK(assign.error == jac313::results::HostPinError::AssignTaken);
    }

    if (failures == 0) {
        std::cout << "host group — OS/hash identity allocates distinct jac313-### rows\n";
        return 0;
    }
    std::cerr << failures << " check(s) failed\n";
    return 1;
}