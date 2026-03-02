#pragma once
#include <string>

namespace sysinfo {

// Run a shell command and return trimmed stdout
std::string exec_cmd(const std::string& cmd);

std::string get_username();
std::string get_hostname();
std::string get_os();
std::string get_kernel();
std::string get_uptime();
std::string get_shell();
std::string get_cpu();
std::string get_memory();
std::string get_packages();

} // namespace sysinfo
