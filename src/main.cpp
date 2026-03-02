#include "sysinfo.h"
#include "agents.h"
#include "display.h"

int main() {
    display::render(
        sysinfo::get_username(),
        sysinfo::get_hostname(),
        sysinfo::get_os(),
        sysinfo::get_kernel(),
        sysinfo::get_uptime(),
        sysinfo::get_shell(),
        sysinfo::get_cpu(),
        sysinfo::get_memory(),
        sysinfo::get_packages(),
        agents::detect_agents()
    );
    return 0;
}
