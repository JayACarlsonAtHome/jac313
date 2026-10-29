#include "contract_modes.hpp"

#ifdef JAC313_TEST_CLI_HAS_JTEXT
#include "parser/section.hpp"
#include "validator/validator.hpp"
#endif

#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

namespace jac313::test_cli {

std::vector<std::string> contract_modes(const std::filesystem::path& source_dir,
                                        const std::string& os_family, const std::string& toolchain) {
    std::vector<std::string> modes;
#ifdef JAC313_TEST_CLI_HAS_JTEXT
    const auto file = source_dir / "Setup" / "contracts.jtext";
    std::ifstream in(file);
    if (in) {
        std::stringstream ss;
        ss << in.rdbuf();
        const std::string text = ss.str();
        auto pf = jtext::parse_file_structure(std::string_view(text));
        const auto vr = pf ? jtext::validate(*pf) : decltype(jtext::validate(*pf)){};
        if (!pf || vr.report.error_count() > 0) {
            std::cerr << "warning: " << file.string() << " does not read as jText; contracts: enforce and ignore\n";
        } else {
            for (const auto& sec : vr.file.sections) {
                auto col = [&](std::string_view name) -> int {
                    for (std::size_t i = 0; i < sec.fields.size(); ++i)
                        if (sec.fields[i].name == name) return static_cast<int>(i);
                    return -1;
                };
                const int co = col("os"), ct = col("toolchain");
                if (co < 0 || ct < 0) continue;
                for (const auto& rec : sec.records) {
                    auto get = [&](int c) {
                        return c >= 0 && c < static_cast<int>(rec.values.size()) ? rec.values[c] : std::string();
                    };
                    if (get(co) != os_family || get(ct) != toolchain) continue;
                    for (const char* m : {"enforce", "observe", "quick_enforce", "ignore"})
                        if (get(col(m)) == "true") modes.emplace_back(m);
                }
            }
        }
    }
#else
    (void)source_dir; (void)os_family; (void)toolchain;
#endif
    if (modes.empty()) modes = {"enforce", "ignore"};
    if (std::find(modes.begin(), modes.end(), "ignore") == modes.end()) modes.emplace_back("ignore");
    return modes;
}

} // namespace jac313::test_cli
