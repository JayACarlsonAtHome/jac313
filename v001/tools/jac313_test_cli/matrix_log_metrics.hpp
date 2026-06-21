#pragma once

#include <cstdint>
#include <filesystem>
#include <optional>
#include <string>

namespace jac313::test_cli {

std::optional<std::uint64_t> peak_ops_from_text(const std::string& text);

std::optional<std::uint64_t> persist_log_bytes(const std::filesystem::path& log_dir,
                                               const std::string& persist);

} // namespace jac313::test_cli