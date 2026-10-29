// Unit tests for the RunIdentity primitive — the single source of truth for a run's
// identity. These pin the generated SQL clauses, the results path, the bind order,
// and the OS-key parsing, so a future dimension change can't silently drift one site.

#include "run_identity.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <source_location>
#include <string_view>
#include <cstdlib>

using namespace jac313::test_cli;

namespace {

int failures = 0;

// A failed check says what and where, and counts toward failures.
void check(bool ok, std::string_view what, std::source_location at = std::source_location::current()) {
    if (ok) return;
    std::cerr << "FAIL: " << what << " at " << at.file_name() << ":" << at.line() << "\n";
    ++failures;
}

// A failed equality check shows both values.
template <class Got, class Want>
void check_eq(const Got& got, const Want& want, std::source_location at = std::source_location::current()) {
    if (!(got != want)) return;
    std::cerr << "FAIL at " << at.file_name() << ":" << at.line() << "\n  got:  " << got << "\n  want: " << want
              << "\n";
    ++failures;
}

void test_columns_order() {
    const std::vector<std::string> expected = {
        "os", "compiler", "build_type", "disk_type", "size_label", "modules"};
    check(run_identity_columns() == expected, "run_identity_columns() == expected");
}

void test_sql_clauses() {
    check_eq(identity_select_list(),
             std::string("os, compiler, build_type, disk_type, size_label, modules"));
    check_eq(identity_select_list("r"),
             std::string("r.os, r.compiler, r.build_type, r.disk_type, r.size_label, r.modules"));
    check_eq(identity_group_by("r"), identity_select_list("r"));
    check_eq(identity_where_eq(),
             std::string("os=? AND compiler=? AND build_type=? AND disk_type=? AND size_label=? AND modules=?"));
    check_eq(identity_join_on("r", "latest"),
             std::string("r.os = latest.os AND r.compiler = latest.compiler"
                         " AND r.build_type = latest.build_type"
                         " AND r.disk_type = latest.disk_type"
                         " AND r.size_label = latest.size_label"
                         " AND r.modules = latest.modules"));
}

void test_values_order_matches_columns() {
    RunIdentity id{"rhel-10.2", "gcc15", "Release", "xssd", "Smoke", "textual"};
    const auto vals = identity_values(id);
    check(vals.size() == run_identity_columns().size(), "vals.size() == run_identity_columns().size()");
    check_eq(vals[0], std::string("rhel-10.2"));   // os
    check_eq(vals[1], std::string("gcc15"));        // compiler
    check_eq(vals[2], std::string("Release"));      // build_type
    check_eq(vals[3], std::string("xssd"));         // disk_type
    check_eq(vals[4], std::string("Smoke"));        // size_label
    check_eq(vals[5], std::string("textual"));      // modules
}

void test_path() {
    RunIdentity id{"rhel-10.2", "gcc15", "Release", "xssd", "Smoke", "textual"};
    check_eq(identity_path("/proj", id).generic_string(),
             std::string("/proj/test-results/rhel-10.2/gcc15/Release/xssd/Smoke/textual"));
}

void test_os_key_parsing() {
    check_eq(os_key_from_release("ID=rhel\nVERSION_ID=\"10.2\"\n"),
             std::string("rhel-10.2"));
    check_eq(os_key_from_release("NAME=\"Linux Mint\"\nID=linuxmint\nVERSION_ID=22\n"),
             std::string("linuxmint-22"));
    check_eq(os_key_from_release("ID=\"rhel\"\nVERSION_ID=\"9.8\"\n"),
             std::string("rhel-9.8"));
    check_eq(os_key_from_release("ID=Fedora\nVERSION_ID=42\n"),
             std::string("fedora-42"));               // lowercased
    check_eq(os_key_from_release("ID=arch\n"),
             std::string("arch"));                    // no version -> id only
    check_eq(os_key_from_release(""), std::string("unknown"));
    // 10.1 vs 10.2 are distinct — the whole point.
    check(os_key_from_release("ID=rhel\nVERSION_ID=10.1\n")
          != os_key_from_release("ID=rhel\nVERSION_ID=10.2\n"), "os_key_from_release(\"ID=rhel\\nVERSION_ID=10.1\\n\") != os_key_from_release(\"ID=rhel\\nVERSION_ID=10.2\\n\")");
}

} // namespace

int main() {
    test_columns_order();
    test_sql_clauses();
    test_values_order_matches_columns();
    test_path();
    test_os_key_parsing();

    if (failures == 0) {
        std::cout << "RunIdentity — all identity/SQL/path/os-key tests passed\n";
        return 0;
    }
    std::cerr << failures << " check(s) failed\n";
    return 1;
}
