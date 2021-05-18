#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>
#include <filesystem>

#include "SystemMonitor.h"

int main() {

    SystemMonitor system_monitor;
    system_monitor.print();

    return 0;
}
