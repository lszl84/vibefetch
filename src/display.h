#pragma once
#include "agents.h"
#include <string>
#include <vector>

namespace display {

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
    const std::vector<agents::Agent>& agents
);

} // namespace display
