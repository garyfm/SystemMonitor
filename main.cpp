#include <iostream>
#include <fstream>
#include <string>
#include <string_view>
#include <vector>
#include <filesystem>

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
        std::cout << "Uptime: " << uptime << " Ideltime: " << idletime << "\n\n";
    }

    {
        //Process p1 {"/proc/1"};
        //p1.print();
    }

    {
        std::vector<Process> process_list;

        std::filesystem::directory_iterator proc_directory {"/proc"};
        for (auto directory : std::filesystem::directory_iterator(proc_directory)) {

            bool is_process_direcory = std::filesystem::exists(directory.path().string() + "/cmdline");
            if (is_process_direcory) {
                process_list.push_back(Process(directory.path()));
            }
        }

        std::cout << "Name\t" << "Pid\t" << "User\t" << "State\t\t" << "Threads\t\t" << "Start Time\t" << "CPU Time\t" << "CPU load\t" << "Mem Usage\t" <<  "Command\t" << "\n";
        for (auto process : process_list) {
            process.print();
        }
    }
    return 0;
}
