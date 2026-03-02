#pragma once
#include <string>
#include <vector>

namespace agents {

struct Agent {
    std::string name;
    bool installed;
    int uses;       // from shell history
    bool most_vibed;
};

std::vector<Agent> detect_agents();

} // namespace agents
