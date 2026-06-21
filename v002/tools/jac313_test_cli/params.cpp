#include "params.hpp"
#include "host_hardware.hpp"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <string_view>

namespace jac313::test_cli {

namespace {

std::string trim(std::string s) {
    auto not_space = [](unsigned char c) { return !std::isspace(c); };
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), not_space));
    s.erase(std::find_if(s.rbegin(), s.rend(), not_space).base(), s.end());
    return s;
}

std::string lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return s;
}

bool is_truthy(const std::string& value) {
    const std::string v = lower(value);
    return v == "x" || v == "1" || v == "true" || v == "yes";
}

// Resolve DISK_TYPE=auto (or empty) by detecting the disk backing the repo/cwd; an explicit
// value (ssd/x7k/10k/15k) is kept as an override. Falls back to "ssd" if undeterminable.
void resolve_disk_type(MatrixParams& params) {
    const std::string current = lower(trim(params.disk_type));
    if (current.empty() || current == "auto") {
        const std::string detected = detect_disk_type(".");
        params.disk_type = detected.empty() ? "ssd" : detected;
    }
}

} // namespace

MatrixParams load_matrix_params(const std::filesystem::path& config_file) {
    MatrixParams params;
    std::ifstream file(config_file);
    if (!file) {
        params.selected_packages["qlite"] = true;
        params.selected_packages["jtext"] = true;
        params.selected_packages["store"] = true;
        resolve_disk_type(params);
        return params;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (auto hash = line.find('#'); hash != std::string::npos) {
            line = line.substr(0, hash);
        }
        line = trim(line);
        if (line.empty() || line.find('=') == std::string::npos) {
            continue;
        }

        const auto eq = line.find('=');
        std::string key = trim(line.substr(0, eq));
        std::string value = trim(line.substr(eq + 1));
        const std::string key_lower = lower(key);

        if (key_lower == "size") {
            params.size = value;
        } else if (key_lower == "disk_type") {
            params.disk_type = value;
        } else if (key_lower == "os_id") {
            params.os_id = value;
        } else if (key_lower == "qlite" || key_lower == "jtext" || key_lower == "store") {
            if (is_truthy(value)) {
                params.selected_packages[key_lower] = true;
            }
        } else if (key.size() == 3 && std::all_of(key.begin(), key.end(), ::isdigit)) {
            if (is_truthy(value)) {
                params.selected_tests[key] = true;
            }
        } else if (key_lower == "flags") {
            if (is_truthy(value)) {
                params.selected_tests["flags"] = true;
            }
        } else if (is_truthy(value)) {
            params.selected_tests[key] = true;
        }
    }

    if (params.selected_packages.empty() && params.selected_tests.empty()) {
        params.selected_packages["qlite"] = true;
        params.selected_packages["jtext"] = true;
        params.selected_packages["store"] = true;
    }
    resolve_disk_type(params);
    return params;
}

std::vector<std::string> selected_packages(const MatrixParams& params) {
    std::vector<std::string> out;
    for (const auto& [name, enabled] : params.selected_packages) {
        if (enabled) {
            out.push_back(name);
        }
    }
    return out;
}

std::string size_label(const std::string& size) {
    if (size == "full" || size == "FULL") {
        return "xFull";
    }
    if (size == "bench" || size == "BENCH") {
        return "Bench";
    }
    return "Smoke";
}

MatrixScaling get_matrix_scaling(const std::string& test_key, const std::string& size) {
    MatrixScaling res;
    const bool is_full = (size == "full" || size == "FULL");

    std::string tnum = test_key;
    if (tnum.size() > 3) {
        tnum = tnum.substr(tnum.size() - 3);
    }
    while (tnum.size() < 3) {
        tnum = "0" + tnum;
    }

    // Big in-memory benchmark tier: ~1M events/run on the throughput tests so the
    // hot-path peak ops/sec (best in Release, persist=none) amortizes setup overhead
    // and is recorded to the results DB under size_label "Bench". Run with
    // `--filter none` for pure in-memory; drop it to also measure persist at scale.
    if (size == "bench" || size == "BENCH") {
        if (tnum == "005" || tnum == "007") {
            res.threads = 50;
            res.events_per_thread = 20000;   // 50 x 20,000 = 1,000,000 events/run
            res.runs = 5;
        } else if (tnum == "008") {
            res.threads = 50;
            res.events_per_thread = 20000;
            res.runs = 3;
        } else if (tnum == "006") {
            res.threads = 50;
            res.events_per_thread = 1999;    // stay under tail-reader track cap (100k)
            res.runs = 3;
        }
        return res;
    }

    if (!is_full) {
        if (tnum == "003" || tnum == "006"
            || tnum == "005" || tnum == "007" || tnum == "008") {
            res.threads = 10;
            res.events_per_thread = 100;
            res.runs = 1;
        }
        return res;
    }

    if (tnum == "001") {
        res.threads = 8;
        res.events_per_thread = 64;
        res.runs = 1;
    } else if (tnum == "002") {
        res.threads = 12;
        res.events_per_thread = 64;
        res.runs = 2;
    } else if (tnum == "003") {
        res.threads = 24;
        res.events_per_thread = 64;
        res.runs = 3;
    } else if (tnum == "004") {
        res.threads = 32;
        res.events_per_thread = 80;
        res.runs = 4;
    } else if (tnum == "005" || tnum == "007") {
        res.threads = 50;
        res.events_per_thread = 2000;
        res.runs = 3;
    } else if (tnum == "006") {
        res.threads = 50;
        res.events_per_thread = 1999; // stay under tail-reader track cap (100k)
        res.runs = 3;
    } else if (tnum == "008") {
        res.threads = 50;
        res.events_per_thread = 20000;
        res.runs = 3;
    }
    return res;
}

namespace {

std::string matrix_test_key(const std::string& test_name) {
    if (test_name == "jac313_store_flags") {
        return "flags";
    }
    if (test_name == "jac313_store_metric_view") {
        return "metric_view";
    }
    constexpr std::string_view prefix = "jac313_store_";
    if (test_name.starts_with(prefix)) {
        const std::string_view rest = std::string_view(test_name).substr(prefix.size());
        if (rest.size() >= 3) {
            return std::string(rest.substr(0, 3));
        }
    }
    return {};
}

bool matrix_key_selected(const MatrixParams& params, const std::string& key) {
    if (key.empty()) {
        return false;
    }
    auto it = params.selected_tests.find(key);
    return it != params.selected_tests.end() && it->second;
}

bool has_matrix_selection(const MatrixParams& params) {
    for (const auto& [key, enabled] : params.selected_tests) {
        if (!enabled) {
            continue;
        }
        if (key == "flags" || key == "metric_view" || (key.size() == 3 && std::all_of(key.begin(), key.end(), ::isdigit))) {
            return true;
        }
    }
    return false;
}

} // namespace

bool test_selected(const MatrixParams& params,
                   const std::string& test_name,
                   const std::string& package)
{
    if (has_matrix_selection(params)) {
        const std::string key = matrix_test_key(test_name);
        if (!key.empty()) {
            return matrix_key_selected(params, key);
        }
    }

    if (!params.selected_tests.empty()) {
        if (params.selected_tests.count(test_name) > 0) {
            return params.selected_tests.at(test_name);
        }
        if (has_matrix_selection(params)) {
            return false;
        }
    }

    if (params.selected_packages.empty()) {
        return true;
    }
    return params.selected_packages.count(package) > 0
        && params.selected_packages.at(package);
}

} // namespace jac313::test_cli