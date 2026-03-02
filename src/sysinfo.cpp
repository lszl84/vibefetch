#include "sysinfo.h"

#include <array>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <sstream>
#include <unistd.h>
#include <sys/utsname.h>

namespace sysinfo {

std::string exec_cmd(const std::string& cmd) {
    std::array<char, 256> buf;
    std::string result;
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return "";
    while (fgets(buf.data(), buf.size(), pipe))
        result += buf.data();
    pclose(pipe);
    while (!result.empty() && (result.back() == '\n' || result.back() == '\r'))
        result.pop_back();
    return result;
}

std::string get_username() {
    const char* user = std::getenv("USER");
    return user ? user : "unknown";
}

std::string get_hostname() {
    char buf[256];
    if (gethostname(buf, sizeof(buf)) == 0)
        return buf;
    return "unknown";
}

std::string get_os() {
    std::ifstream f("/etc/os-release");
    std::string line;
    while (std::getline(f, line)) {
        if (line.rfind("PRETTY_NAME=", 0) == 0) {
            std::string val = line.substr(12);
            if (val.size() >= 2 && val.front() == '"' && val.back() == '"')
                val = val.substr(1, val.size() - 2);
            return val;
        }
    }
    return "Linux";
}

std::string get_kernel() {
    struct utsname u;
    if (uname(&u) == 0)
        return std::string(u.sysname) + " " + u.release;
    return "unknown";
}

std::string get_uptime() {
    std::ifstream f("/proc/uptime");
    double secs = 0;
    f >> secs;
    int total = static_cast<int>(secs);
    int days = total / 86400;
    int hours = (total % 86400) / 3600;
    int mins = (total % 3600) / 60;

    std::string result;
    if (days > 0) result += std::to_string(days) + "d ";
    if (hours > 0 || days > 0) result += std::to_string(hours) + "h ";
    result += std::to_string(mins) + "m";
    return result;
}

std::string get_shell() {
    const char* shell = std::getenv("SHELL");
    if (!shell) return "unknown";

    // Get basename
    std::string s(shell);
    auto pos = s.rfind('/');
    std::string name = (pos != std::string::npos) ? s.substr(pos + 1) : s;

    // Try to get version
    std::string ver = exec_cmd(std::string(shell) + " --version 2>/dev/null | head -1");
    // Extract version number (look for X.Y or X.Y.Z pattern)
    for (size_t i = 0; i < ver.size(); ++i) {
        if (std::isdigit(ver[i])) {
            size_t end = i;
            while (end < ver.size() && (std::isdigit(ver[end]) || ver[end] == '.'))
                ++end;
            if (end > i + 1 && ver.substr(i, end - i).find('.') != std::string::npos)
                return name + " " + ver.substr(i, end - i);
        }
    }
    return name;
}

std::string get_cpu() {
    std::ifstream f("/proc/cpuinfo");
    std::string line;
    while (std::getline(f, line)) {
        if (line.rfind("model name", 0) == 0) {
            auto pos = line.find(':');
            if (pos != std::string::npos) {
                std::string name = line.substr(pos + 1);
                // Trim leading whitespace
                size_t start = name.find_first_not_of(" \t");
                if (start != std::string::npos)
                    name = name.substr(start);
                return name;
            }
        }
    }
    return "unknown";
}

std::string get_memory() {
    std::ifstream f("/proc/meminfo");
    std::string line;
    long total_kb = 0, available_kb = 0;
    while (std::getline(f, line)) {
        if (line.rfind("MemTotal:", 0) == 0)
            std::sscanf(line.c_str(), "MemTotal: %ld", &total_kb);
        else if (line.rfind("MemAvailable:", 0) == 0)
            std::sscanf(line.c_str(), "MemAvailable: %ld", &available_kb);
    }
    long used_mib = (total_kb - available_kb) / 1024;
    long total_mib = total_kb / 1024;
    return std::to_string(used_mib) + " MiB / " + std::to_string(total_mib) + " MiB";
}

std::string get_packages() {
    // Try pacman first
    std::string count = exec_cmd("pacman -Qq 2>/dev/null | wc -l");
    if (!count.empty() && count != "0")
        return count + " (pacman)";

    // dpkg
    count = exec_cmd("dpkg-query -f '\\n' -W 2>/dev/null | wc -l");
    if (!count.empty() && count != "0")
        return count + " (dpkg)";

    // rpm
    count = exec_cmd("rpm -qa 2>/dev/null | wc -l");
    if (!count.empty() && count != "0")
        return count + " (rpm)";

    return "unknown";
}

} // namespace sysinfo
