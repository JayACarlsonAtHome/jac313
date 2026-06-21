#pragma once

#include <filesystem>
#include <map>
#include <string>
#include <vector>

namespace jac313::test_cli {

struct MatrixParams {
    std::string size{"smoke"};
    std::string disk_type{"auto"};  // "auto" -> detect from repo disk; ssd/x7k/10k = override
    std::string os_id;
    std::map<std::string, bool> selected_packages;
    std::map<std::string, bool> selected_tests;
};

struct MatrixScaling {
    size_t threads{0};
    size_t events_per_thread{0};
    size_t runs{0};
};

MatrixParams load_matrix_params(const std::filesystem::path& config_file);
MatrixScaling get_matrix_scaling(const std::string& test_key, const std::string& size);
std::vector<std::string> selected_packages(const MatrixParams& params);
bool test_selected(const MatrixParams& params,
                   const std::string& test_name,
                   const std::string& package);
std::string size_label(const std::string& size);

} // namespace jac313::test_cli