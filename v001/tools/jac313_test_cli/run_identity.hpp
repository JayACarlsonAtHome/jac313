#pragma once

// RunIdentity — the single definition of "what uniquely identifies a test run."
//
// The results DB, the results path, and the rendered reports all need to agree on
// the same identity tuple. Historically that tuple was hand-spelled in ~6 SQL/path
// sites; adding a dimension and missing one silently overwrote data. This primitive
// is the one source of truth: the struct's fields and run_identity_columns() define
// the dimensions, and the SQL clauses, the path, and the bind order are all *derived*
// from that list — so adding a dimension is a single, deliberate edit.

#include <filesystem>
#include <string>
#include <vector>

namespace jac313::test_cli {

struct RunIdentity {
    std::string os;          // sensed OS key, e.g. "rhel-10.2" (id + VERSION_ID)
    std::string compiler;    // gcc15, clang
    std::string build_type;  // Debug, Release
    std::string disk_type;   // ssd, x7k, ...
    std::string size_label;  // Smoke, xFull, Bench
    std::string modules;     // "modules" (built --modules) | "textual" (no modules)
};

// THE ordered dimension list. Field declaration order above MUST match this.
// Add a dimension => add a struct field + an entry here; everything else follows.
const std::vector<std::string>& run_identity_columns();

// "os, compiler, build_type, disk_type, size_label"
// (prefixed when a table alias is given: "r.os, r.compiler, ...")
std::string identity_select_list(const std::string& table_prefix = "");

// Same column list, for GROUP BY.
std::string identity_group_by(const std::string& table_prefix = "");

// "a.os = b.os AND a.compiler = b.compiler AND ..."  (join two aliases on identity)
std::string identity_join_on(const std::string& a, const std::string& b);

// "os=? AND compiler=? AND build_type=? AND disk_type=? AND size_label=?"
// Bind the values with identity_values() (same order) so WHERE and binds can't drift.
std::string identity_where_eq();

// The identity's values in column order (for binding a prepared statement).
std::vector<std::string> identity_values(const RunIdentity& id);

// <project_root>/test-results/<os>/<compiler>/<build_type>/<disk>/<size_label>
std::filesystem::path identity_path(const std::filesystem::path& project_root,
                                    const RunIdentity& id);

// Parse an /etc/os-release body into a key "<id>-<version_id>" (lowercased), e.g.
// "rhel-10.2", "linuxmint-22". Pure function for testability. Missing version -> id;
// missing id -> "unknown".
std::string os_key_from_release(const std::string& os_release_text);

// Read /etc/os-release and return os_key_from_release(); "unknown" if unreadable.
std::string sensed_os_key();

} // namespace jac313::test_cli
