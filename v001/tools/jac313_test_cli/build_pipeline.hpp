#pragma once

#include "options.hpp"

#include <filesystem>
#include <string>
#include <vector>

namespace jac313::test_cli {

void wipe_build_tree(const std::filesystem::path& build_dir);

std::filesystem::path nomodules_build_dir(const std::filesystem::path& modules_build_dir);


int run_configure_command(const GlobalOptions& global, const ConfigureOptions& opts);

int run_build_command(const GlobalOptions& global, const BuildOptions& opts);

int run_build_targets_command(const GlobalOptions& global,
                              const BuildOptions& opts,
                              const std::vector<std::string>& targets,
                              bool clean_first = false,
                              int failsafe_sec = 0);

int timed_build_targets(const std::filesystem::path& build_dir,
                        const std::vector<std::string>& targets,
                        int jobs,
                        bool clean_first = false,
                        int failsafe_sec = 0);

const std::vector<std::string>& module_library_targets();

} // namespace jac313::test_cli