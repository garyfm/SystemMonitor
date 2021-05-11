#include <iostream>
#include <fstream>
#include <string>
#include <string_view>

#include "Process.h"

int main() {
    std::cout << "System Montior\n";

    { // CPU Uptime
        std::ifstream  uptime_file {"/proc/uptime"};
        if (!uptime_file.is_open()) {
            std::cout << "Failed to open /proc/uptime\n";
            return 0;
        }
    
        std::string line;
        std::getline(uptime_file, line);
        std::string_view uptime (line.c_str(), line.find(' '));
        std::string_view idletime (line.c_str() + line.find(' '), line.size());
        std::cout << "Uptime: " << uptime << " Ideltime: " << idletime << "\n";
    }

    {
        Process p1 {"/proc/1"};
        std::cout << "Name\t" << "Pid\t" << "User\t" << "State\t\t" << "Threads\t" << "\tMEM Usage\t" <<  "Command\t" << "\n";
        p1.print();
    }

    return 0;
}
