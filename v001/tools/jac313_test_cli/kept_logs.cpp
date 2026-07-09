#include "kept_logs.hpp"

#include <iostream>

namespace jac313::test_cli {

std::string keep_logs_cli_suffix(const GlobalOptions& global) {
    if (!global.keep_logs) return {};
    std::string s = " --keep-logs";
    if (!global.keep_logs_dir.empty()) {
        s += " --keep-logs-dir " + global.keep_logs_dir.string();
    }
    return s;
}

std::string bench_keep_logs_args(const GlobalOptions& global, const std::string& compiler) {
    if (!global.keep_logs) return {};
    const std::string root = global.keep_logs_dir.empty()
        ? "kept-logs" : global.keep_logs_dir.string();
    return " --keep-logs --keep-logs-dir " + root + "/bench/" + compiler;
}

void print_keep_logs_status(const GlobalOptions& global) {
    if (!global.keep_logs) return;
    const auto kept = resolve_kept_logs_root(global.source_dir, global.keep_logs_dir);
    std::cout << "keep-logs: durable captures -> " << kept.string() << "/{bench,smoke}/...\n";
}

} // namespace jac313::test_cli