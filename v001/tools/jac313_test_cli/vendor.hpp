#pragma once

// vendor.hpp — vendored copies (jText, Qlite in jac313) and their drift, per the shared
// VENDORED.jtext manifest (the same format jac314's pkg/vendored reads and writes).
//
// The Captain, 2026-10-16: vendored copies let each side work on a new version without
// breaking the other; drift gives **warnings, never failures**. PROCESS.md: canonical repo
// first, then a one-way forward-port (copy + the jac313:: transform) into vNNN/<lib>.
//
//   jac313_test_cli vendor-check [--strict] [MANIFEST...]   warnings only (exit 0; 1 with --strict)
//   jac313_test_cli vendor-sync MANIFEST [--write] [--allow-dirty]
//                                                         dry run by default; --write ports
//   jac313_test_cli vendor-baseline VENDORED_DIR --repo NAME --canonical DIR
//                   [--transform jac313-prefix|none] [--notes TEXT]
//                                                         a starting-line manifest for an
//                                                         existing copy (no record of its origin)

#include <filesystem>
#include <map>
#include <string>
#include <vector>

namespace jac313::test_cli::vendor {

struct FileRow {
    std::string path;              // vendored, relative to the manifest dir
    std::string canonical_path;    // relative to canonical_dir
    std::string canonical_sha256;  // canonical content when copied
    std::string vendored_sha256;   // vendored content as written
};

struct Manifest {
    std::map<std::string, std::string> source;   // canonical_repo, canonical_dir, ...
    std::vector<FileRow> files;
    std::filesystem::path path;                  // the VENDORED.jtext file
    std::string get(const std::string& key) const;
    std::vector<std::string> list(const std::string& key) const;   // comma list
};

std::string sha256_hex(const std::string& data);
bool glob_match(const std::string& pattern, const std::string& path);
bool matches_any(const std::vector<std::string>& patterns, const std::string& path);

bool read_manifest(const std::filesystem::path& file, Manifest& m, std::string& err);
std::string manifest_text(const Manifest& m);
bool write_manifest(const Manifest& m, std::string& err);

// The jac313-prefix transform (PROCESS.md) for library `lib` (jText or Qlite).
std::string transform_content(const std::string& transform, const std::string& lib, const std::string& path,
                              const std::string& content);
std::string transform_path(const std::string& transform, const std::string& lib, const std::string& path);

// The directory holding the git checkouts: the parent of the git root above `start`.
std::filesystem::path projects_dir(const std::filesystem::path& start);

int run_vendor_check(int argc, char** argv, int first_arg);
int run_vendor_sync(int argc, char** argv, int first_arg);
int run_vendor_baseline(int argc, char** argv, int first_arg);

} // namespace jac313::test_cli::vendor
