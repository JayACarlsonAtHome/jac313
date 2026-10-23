#pragma once

#include <iostream>
#include <limits>
#include <cctype>
#include <unistd.h>
#include <cstdlib>   // std::getenv
#include <string>

bool is_interactive() const {
    if (const char* env = std::getenv("TS_STORE_INTERACTIVE")) {
        std::string v(env);
        if (v == "1" || v == "true" || v == "yes" || v == "on") return true;
        if (v == "0" || v == "false" || v == "no" || v == "off") return false;
        // unknown value: fall back to Config default + isatty below
    }
    if (!Config::default_interactive) {
        return false;
    }
    return isatty(STDIN_FILENO) && isatty(STDOUT_FILENO);
}

char press_any_key() const {
    if (!is_interactive()) {
        return ' ';
    }

    std::print("\nPress ENTER to continue, Q to quit, E to jump to end Your response? ");

    while (std::cin.peek() != EOF) {
        char ch = static_cast<char>(std::cin.get());
        ch = static_cast<char>(std::toupper(ch));
        if (ch == 'Q' || ch == 'E' ) {
            return ch;
        }
        if (ch == '\n') return ' ';
    }
    return ' ';
}


