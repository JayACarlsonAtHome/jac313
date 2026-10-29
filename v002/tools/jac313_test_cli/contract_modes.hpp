#pragma once

// contract_modes.hpp — the contract evaluation modes (--contracts) a toolchain can run on this
// OS, read from the version's Setup/contracts.jtext: one row per OS and toolchain, a true/false
// column per mode (enforce, observe, quick_enforce, ignore). A toolchain with no row there, or a
// CLI built without jText, has enforce and ignore; ignore is always one of them.

#include <filesystem>
#include <string>
#include <vector>

namespace jac313::test_cli {

// The modes marked true for (os_family, toolchain), in the table's column order.
std::vector<std::string> contract_modes(const std::filesystem::path& source_dir,
                                        const std::string& os_family, const std::string& toolchain);

} // namespace jac313::test_cli
