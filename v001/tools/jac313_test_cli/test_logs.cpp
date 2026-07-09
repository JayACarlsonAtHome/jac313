#include "test_logs.hpp"

#include "format.hpp"
#include "host_hardware.hpp"
#include "kept_logs.hpp"
#include "process.hpp"

#include <fstream>
#include <iostream>
#include <set>

namespace jac313::test_cli {
namespace fs = std::filesystem;

namespace {

std::uint64_t entry_size(const fs::directory_entry& e, std::error_code& ec) {
    if (e.is_regular_file(ec)) return static_cast<std::uint64_t>(e.file_size(ec));
    return 0;
}

void wipe_tree_keep_readme(const fs::path& root, WipeLogsReport& rep) {
    std::error_code ec;
    if (!fs::exists(root, ec)) return;
    for (const auto& e : fs::directory_iterator(root, ec)) {
        if (ec) break;
        if (e.path().filename() == "README.md") continue;
        if (e.is_regular_file(ec)) {
            rep.bytes_removed += entry_size(e, ec);
            fs::remove(e.path(), ec);
            if (!ec) ++rep.entries_removed;
        } else if (e.is_directory(ec)) {
            for (const auto& f : fs::recursive_directory_iterator(e.path(), ec)) {
                if (ec) break;
                if (f.is_regular_file(ec)) rep.bytes_removed += entry_size(f, ec);
            }
            fs::remove_all(e.path(), ec);
            if (!ec) ++rep.entries_removed;
        } else {
            fs::remove(e.path(), ec);
            if (!ec) ++rep.entries_removed;
        }
    }
}

} // namespace

fs::path ctest_logs_dir(const fs::path& source_dir,
                        const fs::path& build_dir,
                        const std::string& compiler_label)
{
    const BuildFeatures bf = read_build_features(build_dir);
    const std::string modules = bf.modules ? "modules" : "textual";
    const std::string build_type = bf.build_type.empty() ? "Debug" : bf.build_type;
    const RunIdentity id{sensed_os_key(), compiler_label, build_type,
                         detect_disk_type("."), "Ctest", modules};
    return identity_path(source_dir, id) / "logs";
}

void write_ctest_logs(const GlobalOptions& global,
                      const std::string& compiler_label,
                      const std::vector<TestResult>& results)
{
    if (results.empty()) return;
    const fs::path log_dir = ctest_logs_dir(global.source_dir, global.build_dir, compiler_label);
    std::error_code ec;
    fs::create_directories(log_dir, ec);
    for (const auto& r : results) {
        write_test_result_log(log_dir / (r.entry.name + ".log"), r.entry, r);
    }
    std::cout << "Logs: " << log_dir.string() << "  (" << results.size()
              << " .log — text only, safe to commit; see test-results/README.md)\n";
}

WipeLogsReport wipe_local_logs(const fs::path& version_root,
                               const fs::path& keep_logs_override)
{
    WipeLogsReport rep;
    std::set<std::string> seen;
    auto wipe_root = [&](const fs::path& p) {
        const fs::path abs = fs::absolute(p);
        const std::string key = abs.string();
        if (!seen.insert(key).second) return;
        wipe_tree_keep_readme(abs, rep);
    };
    wipe_root(version_root / "test-results");
    wipe_root(resolve_kept_logs_root(version_root, {}));
    if (!keep_logs_override.empty()) {
        wipe_root(resolve_kept_logs_root(version_root, keep_logs_override));
    }
    return rep;
}

int run_wipe_logs_command(const GlobalOptions& global) {
    const fs::path root = fs::absolute(global.source_dir.empty() ? fs::current_path()
                                                                 : global.source_dir);
    const WipeLogsReport rep = wipe_local_logs(root, global.keep_logs_dir);
    std::cout << "wipe-logs: removed " << format_count(rep.entries_removed) << " entries ("
              << format_count(rep.bytes_removed) << " bytes) under test-results/ and kept-logs/\n"
              << "  README.md files kept — see test-results/README.md and kept-logs/README.md\n";
    return 0;
}

std::string compiler_label_from_build(const fs::path& build_dir) {
    std::string cxx;
    { std::ifstream in(build_dir / "CMakeCache.txt"); std::string line;
      while (std::getline(in, line))
          if (line.rfind("CMAKE_CXX_COMPILER:", 0) == 0) { cxx = line.substr(line.find('=') + 1); break; } }
    if (cxx.empty()) return "unknown";
    const auto r = run_process({cxx, "--version"}, 0, "");
    const std::string out = r.stdout_text + r.stderr_text;
    const bool clang = out.find("clang") != std::string::npos;
    for (std::size_t i = 0; i < out.size(); ++i) {
        if (out[i] >= '0' && out[i] <= '9' && (i == 0 || out[i - 1] < '0' || out[i - 1] > '9')) {
            std::size_t j = i;
            while (j < out.size() && ((out[j] >= '0' && out[j] <= '9') || out[j] == '.')) ++j;
            const std::string tok = out.substr(i, j - i);
            if (tok.find('.') != std::string::npos) {
                return (clang ? "clang" : "gcc") + tok.substr(0, tok.find('.'));
            }
        }
    }
    return clang ? "clang" : "gcc";
}

} // namespace jac313::test_cli