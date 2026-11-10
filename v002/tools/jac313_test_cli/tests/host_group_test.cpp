// Machine identity is the hardware only: the same box under another OS is the SAME machine (one
// machine row, its runs carry two os rows) — the owner's box dual-boots Fedora and RHEL. Same
// hardware with a different instance_hash (another VM) is ambiguous and needs --claim or
// --assign-new-###. current_host from another machine must not drive assignment — identity compare
// against the machine table comes first.

#include "jac313_results_db.hpp"

#include <jac313/Qlite/v002.hpp>

#include <cstdint>
#include <iostream>
#include <string>
#include <source_location>
#include <string_view>
#include <cstdlib>

using Sqlite = jac313::Qlite::v002::Sqlite;
using jac313::results::MachineId;
using jac313::results::HostPinOptions;

namespace {

int failures = 0;

// A failed check says what and where, and counts toward failures.
void check(bool ok, std::string_view what, std::source_location at = std::source_location::current()) {
    if (ok) return;
    std::cerr << "FAIL: " << what << " at " << at.file_name() << ":" << at.line() << "\n";
    ++failures;
}

MachineId box_i7(const std::string& hash = "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa") {
    return {"Intel(R) Core(TM) i7-7820HQ CPU @ 2.90GHz", 8, 31, "xssd", hash, 4, 2900, 3900};
}

MachineId box_vm2() {
    return box_i7("cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc");
}

MachineId box_ryzen() {
    return {"AMD Ryzen 9 7950X", 32, 64, "xssd", "dddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddddd", 16, 3000, 5700};
}

const jac313::results::OsInfo kFedora{"fedora", "Fedora Linux 44 (Workstation Edition)", "44", "7.2.13-200.fc44.x86_64", "x86_64"};
const jac313::results::OsInfo kRhel{"rhel", "Red Hat Enterprise Linux 10.0 (Coughlan)", "10.0", "6.12.0-55.el10.x86_64", "x86_64"};

std::string cpu_at(Sqlite& db, std::int64_t mid) {
    std::string cpu;
    auto st = db.prepare("SELECT cpu FROM machine WHERE id=?");
    st.bind(mid);
    if (st.step()) st.get(cpu);
    return cpu;
}

std::string hash_at(Sqlite& db, std::int64_t mid) {
    std::string hash;
    auto st = db.prepare("SELECT instance_hash FROM machine WHERE id=?");
    st.bind(mid);
    if (st.step()) st.get(hash);
    return hash;
}

std::int64_t count(Sqlite& db, const std::string& sql) {
    std::int64_t n = -1;
    auto st = db.prepare(sql);
    if (st.step()) st.get(n);
    return n;
}

} // namespace

int main() {
    namespace R = jac313::results;
    constexpr const char* kVer = "v002";

    // One box, two OSes: one machine, two os rows, each run on its own os row.
    {
        Sqlite db(":memory:");
        R::ensure_schema(db);
        const std::int64_t m_fedora = R::pin_host(db, box_i7(), kVer);
        const std::int64_t os_fedora = R::os_id(db, kFedora);
        R::insert_run(db, 1, os_fedora, m_fedora, kVer);
        const std::int64_t m_rhel = R::pin_host(db, box_i7(), kVer);   // rebooted into RHEL
        const std::int64_t os_rhel = R::os_id(db, kRhel);
        R::insert_run(db, 2, os_rhel, m_rhel, kVer);
        check(m_fedora == 1 && m_rhel == 1, "the same hardware under another OS is the same machine");
        check(count(db, "SELECT COUNT(*) FROM machine") == 1, "one machine row");
        check(os_fedora != os_rhel && count(db, "SELECT COUNT(*) FROM os") == 2, "two os rows");
        check(R::os_id(db, kFedora) == os_fedora, "os_id finds the existing row");
        check(count(db, "SELECT COUNT(DISTINCT os_id) FROM run WHERE machine_id=1") == 2, "the machine's runs carry both OSes");
        check(R::run_header(db, 1) == "jac313-001 · Fedora Linux 44 (Workstation Edition) · 7.2.13-200.fc44.x86_64",
              "run_header: machine · OS · kernel");
        check(R::os_display("Debian GNU/Linux", "13") == "Debian GNU/Linux 13", "os_display appends a missing version");

        // Other hardware is another machine.
        const auto propose = R::propose_machine_id(db, box_ryzen(), false);
        check(propose.error == R::HostPinError::None && propose.mid == 2, "other hardware proposes machine 2");
        check(R::pin_host(db, box_ryzen(), kVer) == 2, "other hardware is machine 2");
        check(cpu_at(db, 1) == box_i7().cpu, "machine 1 kept its hardware");
        check(R::current_host(db, kVer) == 2, "the pin follows the last pinned machine");
    }

    // Same hardware, different instance_hash -> ambiguous auto path.
    {
        Sqlite db2(":memory:");
        R::ensure_schema(db2);
        check(R::pin_host(db2, box_i7(), kVer) == 1, "pin_host(db2, box_i7()) == 1");
        const auto propose = R::propose_machine_id(db2, box_vm2(), false);
        check(propose.error == R::HostPinError::Ambiguous, "same hardware, other hash: ambiguous");
        check(!propose.ambiguous_candidates.empty(), "!propose.ambiguous_candidates.empty()");
        const auto assigned = R::resolve_and_pin(db2, box_vm2(), HostPinOptions{}, false, kVer);
        check(assigned.error == R::HostPinError::Ambiguous, "resolve_and_pin stays ambiguous");
        HostPinOptions assign_opts;
        assign_opts.assign_new = 2;
        const auto created = R::resolve_and_pin(db2, box_vm2(), assign_opts, false, kVer);
        check(created.error == R::HostPinError::None, "assign-new 2 succeeds");
        check(created.mid == 2, "created.mid == 2");
        check(hash_at(db2, 2) == box_vm2().instance_hash, "hash_at(db2, 2) == box_vm2().instance_hash");
        const auto reclaim = R::resolve_and_pin(db2, box_i7(), HostPinOptions{"jac313-001"}, false, kVer);
        check(reclaim.error == R::HostPinError::None, "reclaim.error == None");
        check(reclaim.mid == 1, "reclaim.mid == 1");
    }

    // v001 and v002 pins are independent rows; the same machine shares its machine row.
    {
        Sqlite db3(":memory:");
        R::ensure_schema(db3);
        check(R::pin_host(db3, box_i7(), "v001") == 1, "pin v001 == 1");
        check(R::pin_host(db3, box_i7(), "v002") == 1, "pin v002 == 1");
        check(R::current_host(db3, "v001") == 1, "current_host v001 == 1");
        check(R::current_host(db3, "v002") == 1, "current_host v002 == 1");
        R::set_current_host(db3, 99, "v002");
        check(R::current_host(db3, "v001") == 1, "current_host v001 still 1");
        check(R::current_host(db3, "v002") == 99, "current_host v002 == 99");
    }

    // claim fails on other hardware.
    {
        Sqlite db3(":memory:");
        R::ensure_schema(db3);
        R::pin_host(db3, box_i7(), kVer);
        const auto claim = R::claim_machine(db3, box_ryzen(), "jac313-001");
        check(claim.error == R::HostPinError::ClaimHwMismatch, "claim on other hardware: ClaimHwMismatch");
    }

    // assign-new errors when taken.
    {
        Sqlite db4(":memory:");
        R::ensure_schema(db4);
        R::pin_host(db4, box_i7(), kVer);
        const auto assign = R::assign_new_machine(db4, box_ryzen(), 1);
        check(assign.error == R::HostPinError::AssignTaken, "assign-new 1 is taken");
    }

    // assign-new / claim for a machine already filed under another number: everything it owns
    // moves with it (2026-11-10 live test: only the machine row moved, the runs, the other
    // version's pin and the calibration were left on a number no machine had).
    {
        Sqlite db6(":memory:");
        R::ensure_schema(db6);
        const char* kOther = std::string_view(kVer) == "v001" ? "v002" : "v001";
        check(R::pin_host(db6, box_i7(), kVer) == 1, "this machine is 1");
        check(R::pin_host(db6, box_i7(), kOther) == 1, "both versions pinned to 1");
        const std::int64_t os = R::os_id(db6, kFedora);
        R::insert_run(db6, 10, os, 1, kVer, "s", "q", "j");
        R::insert_run(db6, 11, os, 1, kOther, "s", "q", "j");
        R::upsert_io_best_fit(db6, 1, "binary", 250, 250, 250, 1000, 250);
        R::mark_fleet_synced(db6, 1, "srv", "t");
        HostPinOptions to2;
        to2.assign_new = 2;
        const auto moved = R::resolve_and_pin(db6, box_i7(), to2, false, kVer);
        check(moved.error == R::HostPinError::None && moved.mid == 2, "assign-new 2 for a known machine");
        check(cpu_at(db6, 2) == box_i7().cpu && !R::machine_in_use(db6, 1), "nothing is left on 1");
        check(R::current_host(db6, kVer) == 2 && R::current_host(db6, kOther) == 2, "both pins moved to 2");
        check(R::io_best_fit_use(db6, 2, "binary", 0) == 250, "the calibration moved to 2");
        check(!R::fleet_synced(db6, 2, "srv"), "fleet confirmation dropped: the next run asks the server");
        const auto again = R::resolve_and_pin(db6, box_i7(), to2, false, kVer);
        check(again.error == R::HostPinError::None && again.mid == 2, "assign-new to its own number again: ok");
        R::insert_run(db6, 12, os, 7, kVer, "s", "q", "j");   // stale rows of a machine that is gone
        HostPinOptions to7;
        to7.assign_new = 7;
        check(R::resolve_and_pin(db6, box_i7(), to7, false, kVer).error == R::HostPinError::AssignTaken,
              "a number only stale rows use is taken too");
        const auto back = R::resolve_and_pin(db6, box_i7(), HostPinOptions{"jac313-002"}, false, kVer);
        check(back.error == R::HostPinError::None && back.mid == 2, "claim its own slot");
    }

    // renumber_machine: the shared server says this machine is 3, not its local 1. Every table moves,
    // and another machine's local rows already on 3 are moved out of the way.
    {
        Sqlite db5(":memory:");
        R::ensure_schema(db5);
        R::pin_host(db5, box_i7(), kVer);                       // local 1
        R::upsert_machine(db5, 3, box_ryzen());                 // another machine's local 3
        const std::int64_t os = R::os_id(db5, kFedora);
        R::insert_run(db5, 10, os, 1, kVer, "s", "q", "j");
        R::insert_run(db5, 11, os, 3, kVer, "s", "q", "j");
        R::upsert_package_version(db5, 1, kVer, "Store", "v002.001");
        R::mark_fleet_synced(db5, 1, "srv", "t");
        R::renumber_machine(db5, 1, 3);
        check(cpu_at(db5, 3) == box_i7().cpu, "this machine is now 3");
        check(cpu_at(db5, 4) == box_ryzen().cpu, "the other machine moved to the next free number");
        check(!R::machine_in_use(db5, 1), "!R::machine_in_use(db5, 1)");
        check(R::current_host(db5, kVer) == 3, "R::current_host(db5, kVer) == 3");
        check(R::package_version_get(db5, 3, kVer, "Store") == "v002.001", "package_version moved");
        check(R::fleet_synced(db5, 3, "srv"), "R::fleet_synced(db5, 3, \"srv\")");
        check(!R::fleet_synced(db5, 3, "other-srv"), "!R::fleet_synced(db5, 3, \"other-srv\")");
        check(R::fleet_synced_within(db5, 3, "srv", 24) == false, "\"t\" is no date: stale");
        R::mark_fleet_synced(db5, 3, "srv", R::utc_now_iso());
        check(R::fleet_synced_within(db5, 3, "srv", 24), "R::fleet_synced_within(db5, 3, \"srv\", 24)");
        db5.exec("UPDATE fleet_sync SET ts_utc='2000-01-01T00:00:00Z' WHERE machine_id=3");
        check(!R::fleet_synced_within(db5, 3, "srv", 24), "!R::fleet_synced_within(db5, 3, \"srv\", 24)");
        check(R::fleet_synced(db5, 3, "srv"), "R::fleet_synced(db5, 3, \"srv\")");
        check(count(db5, "SELECT machine_id FROM run WHERE run_id=10") == 3, "run 10 moved to machine 3");
        check(count(db5, "SELECT machine_id FROM run WHERE run_id=11") == 4, "run 11 moved to machine 4");
        check(R::run_header(db5, 10).rfind("jac313-003 · ", 0) == 0, "run 10's header names jac313-003");
        check(R::next_free_machine_id(db5) == 5, "R::next_free_machine_id(db5) == 5");
        R::renumber_machine(db5, 3, 3);                         // no-op
        check(cpu_at(db5, 3) == box_i7().cpu, "renumber to itself changes nothing");
    }

    if (failures == 0) {
        std::cout << "host machine — hardware identity: one machine across OSes, distinct jac313-### per box\n";
        return 0;
    }
    std::cerr << failures << " check(s) failed\n";
    return 1;
}
