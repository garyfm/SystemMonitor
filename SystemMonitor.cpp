#include <vector>
#include <iostream>
#include <filesystem>
#include <fstream>

#include "SystemMonitor.h"

SystemMonitor::SystemMonitor() {

    read();

    std::filesystem::directory_iterator proc_directory {"/proc"};
    for (auto directory : std::filesystem::directory_iterator(proc_directory)) {

        bool is_process_direcory = std::filesystem::exists(directory.path().string() + "/cmdline");
        if (is_process_direcory) {
            process_list.push_back(Process(directory.path()));
        }
    }
}

bool SystemMonitor::read() {

    std::ifstream  uptime_file {"/proc/uptime"};
    
    if (!uptime_file.is_open()) {
        std::cout << "Failed to open /proc/uptime\n";
        return false;
    }

    std::string line;
    std::getline(uptime_file, line);
    uptime = line.substr(0, line.find(' '));
    idletime = line.substr(0 + line.find(' '), line.size());

    return true;
}

void SystemMonitor::print() {

    std::cout << "System Montior\n";
    std::cout << "Uptime: " << uptime << " Ideltime: " << idletime << "\n\n";

    std::cout << "Name\t" << "Pid\t" << "User\t" << "State\t\t" << "Threads\t\t" << "Start Time\t" << "CPU Time\t" << "CPU load\t" << "Mem Usage\t" <<  "Command\t" << "\n";
    
    for (auto process : process_list) {
            process.print();
    }
}