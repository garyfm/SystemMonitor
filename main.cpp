#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>
#include <filesystem>

#include "SystemMonitor.h"

int main() {

    { // CPU Uptime
            }

    {
        //Process p1 {"/proc/1"};
        //p1.print();
    }

    SystemMonitor system_monitor;
    system_monitor.print();
    return 0;
}
