//ts_store/ts_store_headers/impl_details/ansi_colors.hpp
#pragma once

#include <cstdlib>
#include <iostream>
#include <string>
#include <string_view>
#include <unistd.h>

inline std::ostream& operator<<(std::ostream& os, std::string_view sv) {
    return os.write(sv.data(), static_cast<std::streamsize>(sv.size()));
}

namespace ansi {

    inline bool colors_enabled() {
        static bool decided = false;
        static bool enabled = true;
        if (!decided) {
            decided = true;
            const char* nc = std::getenv("NO_COLOR");
            const char* c = std::getenv("TS_STORE_COLOR");
            if (nc && *nc) {
                enabled = false;
            } else if (c) {
                std::string v(c);
                if (v == "0" || v == "false" || v == "no" || v == "off") enabled = false;
                else if (v == "1" || v == "true" || v == "yes" || v == "on") enabled = true;
                else enabled = false;
            } else {
                enabled = false;  // default to no colors if no env/CLI (for test programs etc.)
            }
        }
        return enabled;
    }

    inline std::string_view reset()          { return colors_enabled() ? "\033[0m" : ""; }
    inline std::string_view bold()           { return colors_enabled() ? "\033[1m" : ""; }
    inline std::string_view dim()            { return colors_enabled() ? "\033[2m" : ""; }

    inline std::string_view red()            { return colors_enabled() ? "\033[31m" : ""; }
    inline std::string_view green()          { return colors_enabled() ? "\033[32m" : ""; }
    inline std::string_view yellow()         { return colors_enabled() ? "\033[33m" : ""; }
    inline std::string_view blue()           { return colors_enabled() ? "\033[34m" : ""; }
    inline std::string_view magenta()        { return colors_enabled() ? "\033[35m" : ""; }
    inline std::string_view cyan()           { return colors_enabled() ? "\033[36m" : ""; }
    inline std::string_view white()          { return colors_enabled() ? "\033[37m" : ""; }
    inline std::string_view gray()           { return colors_enabled() ? "\033[90m" : ""; }

    inline std::string_view bold_red()       { return colors_enabled() ? "\033[1;31m" : ""; }
    inline std::string_view bold_green()     { return colors_enabled() ? "\033[1;32m" : ""; }
    inline std::string_view bold_yellow()    { return colors_enabled() ? "\033[1;33m" : ""; }
    inline std::string_view bold_blue()      { return colors_enabled() ? "\033[1;34m" : ""; }
    inline std::string_view bold_magenta()   { return colors_enabled() ? "\033[1;35m" : ""; }
    inline std::string_view bold_cyan()      { return colors_enabled() ? "\033[1;36m" : ""; }
    inline std::string_view bold_white()     { return colors_enabled() ? "\033[1;37m" : ""; }

    inline std::string_view bright_red()     { return colors_enabled() ? "\033[91m" : ""; }
    inline std::string_view bright_green()   { return colors_enabled() ? "\033[92m" : ""; }
    inline std::string_view bright_yellow()  { return colors_enabled() ? "\033[93m" : ""; }
    inline std::string_view bright_blue()    { return colors_enabled() ? "\033[94m" : ""; }
    inline std::string_view bright_magenta() { return colors_enabled() ? "\033[95m" : ""; }
    inline std::string_view bright_cyan()    { return colors_enabled() ? "\033[96m" : ""; }
}