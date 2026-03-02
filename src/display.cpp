#include "display.h"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

namespace display {

// ANSI color helpers
static const char* RESET  = "\033[0m";
static const char* BOLD   = "\033[1m";
static const char* CYAN   = "\033[36m";
static const char* GREEN  = "\033[32m";
static const char* YELLOW = "\033[33m";
static const char* RED    = "\033[31m";
static const char* MAGENTA = "\033[35m";
static const char* DIM    = "\033[2m";

// Visible string length (strips ANSI escape sequences)
static size_t visible_len(const std::string& s) {
    size_t len = 0;
    bool in_esc = false;
    for (char c : s) {
        if (c == '\033') { in_esc = true; continue; }
        if (in_esc) { if (c == 'm') in_esc = false; continue; }
        ++len;
    }
    return len;
}

// Pad a string (accounting for ANSI escapes) to a given visible width
static std::string pad(const std::string& s, size_t width) {
    size_t vlen = visible_len(s);
    if (vlen >= width) return s;
    return s + std::string(width - vlen, ' ');
}

void render(
    const std::string& username,
    const std::string& hostname,
    const std::string& os,
    const std::string& kernel,
    const std::string& uptime,
    const std::string& shell,
    const std::string& cpu,
    const std::string& memory,
    const std::string& packages,
    const std::vector<agents::Agent>& agent_list
) {
    // ASCII art (12 lines)
    std::vector<std::string> art = {
        "   .--.   ",
        "  |o  o|  ",
        "  | \\/ |  ",
        "  |    |  ",
        " /|----|\\ ",
        "/ |~~~~| \\",
        "  |~~~~|  ",
        "  |~~~~|  ",
        "  '----'  ",
        " vibefetch",
        " ~~~~~~~~~~",
        "           ",
    };

    const size_t art_width = 11; // visible width of art column
    const std::string gap = "    "; // gap between art and info

    // Build info lines
    std::vector<std::string> info;

    // Title line
    std::string title = std::string(BOLD) + CYAN + username + "@" + hostname + RESET;
    info.push_back(title);

    // Separator
    std::string sep(username.size() + 1 + hostname.size(), '-');
    info.push_back(sep);

    auto add_field = [&](const char* label, const std::string& value) {
        info.push_back(std::string(BOLD) + CYAN + label + RESET + value);
    };

    add_field("OS: ", os);
    add_field("Kernel: ", kernel);
    add_field("Uptime: ", uptime);
    add_field("Shell: ", shell);
    add_field("CPU: ", cpu);
    add_field("Memory: ", memory);
    add_field("Packages: ", packages);

    // Blank line before agents
    info.push_back("");
    info.push_back(std::string(BOLD) + MAGENTA + "AI Agents:" + RESET);

    // Find max agent name length for alignment
    size_t max_name = 0;
    for (const auto& a : agent_list)
        max_name = std::max(max_name, a.name.size());

    for (const auto& a : agent_list) {
        std::string line = "  ";
        if (a.installed) {
            line += std::string(GREEN) + "\u25cf" + RESET + " ";
            line += pad(a.name, max_name);
            if (a.uses > 0) {
                line += std::string("  ") + DIM + "(" + std::to_string(a.uses) + " uses)" + RESET;
                if (a.most_vibed)
                    line += " " + std::string(BOLD) + YELLOW + "<<< most vibed" + RESET;
            }
        } else {
            line += std::string(DIM) + "\u25cb" + " ";
            line += pad(a.name, max_name);
            line += "  not installed" + std::string(RESET);
        }
        info.push_back(line);
    }

    // Color blocks
    info.push_back("");
    std::string blocks;
    const char* colors[] = {
        "\033[40m", "\033[41m", "\033[42m", "\033[43m",
        "\033[44m", "\033[45m", "\033[46m", "\033[47m"
    };
    for (const auto& c : colors)
        blocks += std::string(c) + "   " + RESET;
    info.push_back(blocks);

    // Bright color blocks
    std::string bright_blocks;
    const char* bright_colors[] = {
        "\033[100m", "\033[101m", "\033[102m", "\033[103m",
        "\033[104m", "\033[105m", "\033[106m", "\033[107m"
    };
    for (const auto& c : bright_colors)
        bright_blocks += std::string(c) + "   " + RESET;
    info.push_back(bright_blocks);

    // Print side by side
    size_t total = std::max(art.size(), info.size());
    std::cout << "\n";
    for (size_t i = 0; i < total; ++i) {
        std::string left;
        if (i < art.size())
            left = std::string(CYAN) + pad(art[i], art_width) + RESET;
        else
            left = std::string(art_width, ' ');

        std::string right;
        if (i < info.size())
            right = info[i];

        std::cout << left << gap << right << "\n";
    }
    std::cout << "\n";
}

} // namespace display
