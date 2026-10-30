#include "contract_modes.hpp"
#include "settings.hpp"

#include <algorithm>
#include <iostream>
#include <stdexcept>

namespace jac313::test_cli {

std::vector<std::string> contract_modes(const std::filesystem::path& source_dir,
                                        const std::string& os_family, const std::string& toolchain) {
    std::vector<std::string> modes;
    const auto file = source_dir / "Setup" / "contracts.jtext";
    try {
        const SettingsTable t = read_jtext_table(file, "contract_modes");
        require_columns(t, file, "contract_modes", {"os", "toolchain", "enforce", "observe", "quick_enforce", "ignore"});
        for (const auto& row : t.rows) {
            if (row.at("os") != os_family || row.at("toolchain") != toolchain) continue;
            for (const char* m : {"enforce", "observe", "quick_enforce", "ignore"})
                if (jtext_bool(row.at(m))) modes.emplace_back(m);
        }
    } catch (const std::exception& e) {
        std::cerr << "warning: " << e.what() << "; contracts: enforce and ignore\n";
    }
    if (modes.empty()) modes = {"enforce", "ignore"};
    if (std::find(modes.begin(), modes.end(), "ignore") == modes.end()) modes.emplace_back("ignore");
    return modes;
}

} // namespace jac313::test_cli
