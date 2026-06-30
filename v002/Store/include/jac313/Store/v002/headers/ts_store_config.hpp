// ts_store_headers/ts_store_config.hpp

#pragma once

// import-std module build: std comes from `import std;` and the global C names from the
// module unit's GMF, so skip the textual includes; and the content is pasted into the
// module's own `export namespace`, so skip this header's namespace wrapper too. The normal
// textual build (macro undefined) is unchanged. See Claude_Forward STORE import-std plan.
#ifndef JAC313_STORE_IMPORT_STD
#include <cstdint>
#include <cstring>
#include <string>
#include <string_view>
#endif

#ifndef JAC313_STORE_IMPORT_STD
namespace jac313::Store::v002 {
#endif

    // Package version, "major.minor": major = the v002 API line, minor = the revision. Bump on
    // each small code change (edit the minor); v002 stays until a real (breaking) change, then
    // this becomes "v002.001". Enforced by `jac313_test_cli version-check` (pre-push).
    [[nodiscard]] constexpr std::string_view version() noexcept { return "v002.006"; }

    /// Fixed-size storage for category/payload.
    /// Replaces std::string for the hot-path row storage.
    /// Pre-sized at template time (using the Max* codepoint limits from config),
    /// direct writes via memcpy after UTF-8 codepoint truncation.
    /// No heap allocations, no capacity management, better locality for the in-memory path.
    template <size_t MaxCodepoints>
    struct bounded_string {
        static constexpr size_t max_codepoints = MaxCodepoints;
        static constexpr size_t max_bytes      = MaxCodepoints * 4 + 1;

        char buf[max_bytes] = {};
        size_t len = 0;   // byte length of valid content

        bounded_string() = default;

        // Construct/assign from string_view or std::string (for call-site compatibility, including string literals)
        bounded_string(std::string_view sv) {
            assign_truncated(sv, MaxCodepoints);
        }
        bounded_string(const std::string& s) : bounded_string(std::string_view(s)) {}
        bounded_string(std::string&& s) : bounded_string(std::string_view(s)) {}
        bounded_string(const char* s) : bounded_string(std::string_view(s ? s : "")) {}

        void clear() {
            len = 0;
            if (max_bytes > 0) buf[0] = '\0';
        }

        std::string_view view() const noexcept {
            return std::string_view(buf, len);
        }

        bool empty() const noexcept { return len == 0; }
        size_t size() const noexcept { return len; }  // bytes

        // Direct write into our fixed buffer. This is the hot path replacement
        // for the old std::string + truncate + assign.
        void assign_truncated(std::string_view sv, size_t max_cp) {
            len = 0;
            size_t count = 0;
            for (size_t i = 0; i < sv.size() && count < max_cp; ) {
                uint8_t byte = static_cast<uint8_t>(sv[i]);
                size_t seq_len = 1;
                if (byte >= 0x80) {
                    if ((byte & 0xE0) == 0xC0) seq_len = 2;
                    else if ((byte & 0xF0) == 0xE0) seq_len = 3;
                    else if ((byte & 0xF8) == 0xF0) seq_len = 4;
                }
                if (i + seq_len <= sv.size() && len + seq_len < max_bytes) {
                    std::memcpy(buf + len, sv.data() + i, seq_len);
                    len += seq_len;
                    ++count;
                }
                i += seq_len;
            }
            if (len < max_bytes) buf[len] = '\0';
        }

        void assign_truncated(std::string_view sv) {
            assign_truncated(sv, MaxCodepoints);
        }

        // For places that still want a std::string (verify, printing, persist copy-out, etc.)
        std::string str() const { return std::string(buf, len); }

        // Implicit for string_view contexts (select, sinks, etc.)
        operator std::string_view() const noexcept { return view(); }
    };

    template <
        bool UseTimestamps = true,
        size_t MaxTypeLength     = 6,
        size_t MaxCategoryLength = 20,
        size_t MaxPayloadLength  = 80,
        size_t IntMetrics = 9,
        size_t DblMetrics = 6,
        bool EnableMetrics = false,
        bool DefaultInteractive = false,
        bool DefaultColor = false,
        bool DebugMode = false
    >
    struct ts_store_config {

        static_assert(MaxTypeLength     >=  5, "MaxTypeLength must be at least 5");
        static_assert(MaxCategoryLength >=  8, "MaxCategoryLength must be at least 8");
        static_assert(MaxPayloadLength  >= 43, "MaxPayloadLength must be at least 43");

        using ValueT     = bounded_string<MaxPayloadLength>;
        using TypeT      = std::string;   // legacy / not used for row storage
        using CategoryT  = bounded_string<MaxCategoryLength>;

        static constexpr bool use_timestamps = UseTimestamps;
        static constexpr bool enable_metrics = EnableMetrics;
        static constexpr bool default_interactive = DefaultInteractive;
        static constexpr bool default_color = DefaultColor;
        static constexpr bool debug_mode = DebugMode;

        static constexpr size_t max_payload_length = MaxPayloadLength;
        static constexpr size_t max_type_length    = MaxTypeLength;
        static constexpr size_t max_category_length = MaxCategoryLength;
        static constexpr size_t the_IntMetrics = IntMetrics;
        static constexpr size_t the_DblMetrics = DblMetrics;

        static constexpr size_t utf8_length(std::string_view sv) noexcept {
            size_t len = 0;
            for (size_t i = 0; i < sv.size(); ) {
                uint8_t byte = static_cast<uint8_t>(sv[i]);
                if (byte < 0x80) ++i;
                else if ((byte & 0xE0) == 0xC0) i += 2;
                else if ((byte & 0xF0) == 0xE0) i += 3;
                else if ((byte & 0xF8) == 0xF0) i += 4;
                else ++i;  // Invalid, count as 1
                ++len;
            }
            return len;
        }

        static std::string utf8_truncate(std::string_view sv, size_t max_chars) {
            // Non-hot path (verify, test setup, etc.). Uses bounded internally then converts.
            // (std::string-based into removed as part of dropping std::string for hot-path storage.)
            bounded_string<256> tmp;  // sufficient for any configured max
            tmp.assign_truncated(sv, max_chars);
            return tmp.str();
        }
    };

#ifndef JAC313_STORE_IMPORT_STD
}  // namespace jac313::Store::v002
#endif