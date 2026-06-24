// Unit tests for the RunIdentity primitive — the single source of truth for a run's
// identity. These pin the generated SQL clauses, the results path, the bind order,
// and the OS-key parsing, so a future dimension change can't silently drift one site.

#include "run_identity.hpp"

#include <iostream>
#include <string>
#include <vector>

using namespace jac313::test_cli;

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

#define CHECK_EQ(got, want)                                                    \
    do {                                                                       \
        const auto g_ = (got);                                                 \
        const auto w_ = (want);                                                \
        if (g_ != w_) {                                                        \
            std::cerr << "FAIL at " << __FILE__ << ":" << __LINE__             \
                      << "\n  got:  " << g_ << "\n  want: " << w_ << "\n";      \
            ++failures;                                                        \
        }                                                                      \
    } while (0)

void test_columns_order() {
    const std::vector<std::string> expected = {
        "os", "compiler", "build_type", "disk_type", "size_label", "modules"};
    CHECK(run_identity_columns() == expected);
}

void test_sql_clauses() {
    CHECK_EQ(identity_select_list(),
             std::string("os, compiler, build_type, disk_type, size_label, modules"));
    CHECK_EQ(identity_select_list("r"),
             std::string("r.os, r.compiler, r.build_type, r.disk_type, r.size_label, r.modules"));
    CHECK_EQ(identity_group_by("r"), identity_select_list("r"));
    CHECK_EQ(identity_where_eq(),
             std::string("os=? AND compiler=? AND build_type=? AND disk_type=? AND size_label=? AND modules=?"));
    CHECK_EQ(identity_join_on("r", "latest"),
             std::string("r.os = latest.os AND r.compiler = latest.compiler"
                         " AND r.build_type = latest.build_type"
                         " AND r.disk_type = latest.disk_type"
                         " AND r.size_label = latest.size_label"
                         " AND r.modules = latest.modules"));
}

void test_values_order_matches_columns() {
    RunIdentity id{"rhel-10.2", "gcc15", "Release", "ssd", "Smoke", "textual"};
    const auto vals = identity_values(id);
    CHECK(vals.size() == run_identity_columns().size());
    CHECK_EQ(vals[0], std::string("rhel-10.2"));   // os
    CHECK_EQ(vals[1], std::string("gcc15"));        // compiler
    CHECK_EQ(vals[2], std::string("Release"));      // build_type
    CHECK_EQ(vals[3], std::string("ssd"));          // disk_type
    CHECK_EQ(vals[4], std::string("Smoke"));        // size_label
    CHECK_EQ(vals[5], std::string("textual"));      // modules
}

void test_path() {
    RunIdentity id{"rhel-10.2", "gcc15", "Release", "ssd", "Smoke", "textual"};
    CHECK_EQ(identity_path("/proj", id).generic_string(),
             std::string("/proj/test-results/rhel-10.2/gcc15/Release/ssd/Smoke/textual"));
}

void test_os_key_parsing() {
    CHECK_EQ(os_key_from_release("ID=rhel\nVERSION_ID=\"10.2\"\n"),
             std::string("rhel-10.2"));
    CHECK_EQ(os_key_from_release("NAME=\"Linux Mint\"\nID=linuxmint\nVERSION_ID=22\n"),
             std::string("linuxmint-22"));
    CHECK_EQ(os_key_from_release("ID=\"rhel\"\nVERSION_ID=\"9.8\"\n"),
             std::string("rhel-9.8"));
    CHECK_EQ(os_key_from_release("ID=Fedora\nVERSION_ID=42\n"),
             std::string("fedora-42"));               // lowercased
    CHECK_EQ(os_key_from_release("ID=arch\n"),
             std::string("arch"));                    // no version -> id only
    CHECK_EQ(os_key_from_release(""), std::string("unknown"));
    // 10.1 vs 10.2 are distinct — the whole point.
    CHECK(os_key_from_release("ID=rhel\nVERSION_ID=10.1\n")
          != os_key_from_release("ID=rhel\nVERSION_ID=10.2\n"));
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
