#include "agents.h"
#include "sysinfo.h"

#include <algorithm>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <regex>
#include <string>

namespace fs = std::filesystem;

namespace agents {

static bool binary_exists(const std::string& name) {
    return !sysinfo::exec_cmd("which " + name + " 2>/dev/null").empty();
}

static bool dir_exists(const std::string& path) {
    return fs::is_directory(path);
}

static std::string expand_home(const std::string& path) {
    const char* home = std::getenv("HOME");
    if (!home) return path;
    if (path.rfind("~/", 0) == 0)
        return std::string(home) + path.substr(1);
    return path;
}

// Count how many times a command appears in shell history files
static int count_history(const std::string& cmd) {
    int total = 0;
    std::string home = std::getenv("HOME") ? std::getenv("HOME") : "";
    if (home.empty()) return 0;

    std::vector<std::string> history_files = {
        home + "/.bash_history",
        home + "/.zsh_history",
    };

    // Match the command at the start of a line (possibly after zsh timestamp prefix)
    // zsh extended history format: ": 1234567890:0;command args..."
    // bash format: "command args..."
    std::regex pattern("^(: [0-9]+:[0-9]+;)?" + cmd + "(\\s|$)");

    for (const auto& file : history_files) {
        std::ifstream f(file);
        std::string line;
        while (std::getline(f, line)) {
            if (std::regex_search(line, pattern))
                ++total;
        }
    }
    return total;
}

static bool has_vscode_extension(const std::string& prefix) {
    std::string home = std::getenv("HOME") ? std::getenv("HOME") : "";
    if (home.empty()) return false;

    std::string ext_dir = home + "/.vscode/extensions";
    if (!fs::is_directory(ext_dir)) return false;

    for (const auto& entry : fs::directory_iterator(ext_dir)) {
        if (entry.is_directory()) {
            std::string name = entry.path().filename().string();
            if (name.rfind(prefix, 0) == 0)
                return true;
        }
    }
    return false;
}

std::vector<Agent> detect_agents() {
    struct AgentDef {
        std::string name;
        std::string binary;          // empty = skip binary check
        std::vector<std::string> dirs; // directories to check
        std::string vscode_ext;      // VS Code extension prefix
        std::string history_cmd;     // command to search in history
    };

    std::vector<AgentDef> defs = {
        {"Claude Code",     "claude",   {"~/.claude/"},                                         "",                        "claude"},
        {"OpenCode",        "opencode", {"~/.opencode/"},                                       "",                        "opencode"},
        {"Cursor",          "",         {"~/.cursor/", "/opt/Cursor"},                          "",                        ""},
        {"Gemini CLI",      "gemini",   {},                                                     "",                        "gemini"},
        {"Codex",           "codex",    {},                                                     "",                        "codex"},
        {"GitHub Copilot",  "",         {"~/.local/share/gh/extensions/gh-copilot/"},            "",                        "gh copilot"},
        {"Aider",           "aider",    {},                                                     "",                        "aider"},
        {"Continue",        "",         {"~/.continue/"},                                       "",                        ""},
        {"Cline",           "",         {},                                                     "saoudrizwan.claude-dev",  ""},
        {"Windsurf",        "",         {"~/.codeium/", "/opt/Windsurf"},                       "",                        ""},
    };

    std::vector<Agent> results;
    int max_uses = 0;
    int max_idx = -1;

    for (size_t i = 0; i < defs.size(); ++i) {
        const auto& d = defs[i];
        bool installed = false;

        // Check binary
        if (!d.binary.empty() && binary_exists(d.binary))
            installed = true;

        // Check directories
        if (!installed) {
            for (const auto& dir : d.dirs) {
                if (dir_exists(expand_home(dir))) {
                    installed = true;
                    break;
                }
            }
        }

        // Check VS Code extension
        if (!installed && !d.vscode_ext.empty())
            installed = has_vscode_extension(d.vscode_ext);

        // Count history uses
        int uses = 0;
        if (!d.history_cmd.empty())
            uses = count_history(d.history_cmd);

        if (uses > max_uses) {
            max_uses = uses;
            max_idx = static_cast<int>(results.size());
        }

        results.push_back({d.name, installed, uses, false});
    }

    // Crown the most vibed
    if (max_idx >= 0 && max_uses > 0)
        results[max_idx].most_vibed = true;

    return results;
}

} // namespace agents
