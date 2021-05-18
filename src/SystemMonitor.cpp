#include <vector>
#include <iostream>
#include <filesystem>
#include <fstream>

#include "SystemMonitor.h"

SystemMonitor::SystemMonitor() {


    std::filesystem::directory_iterator proc_directory {"/proc"};
    for (auto directory : std::filesystem::directory_iterator(proc_directory)) {

        bool is_process_direcory = std::filesystem::exists(directory.path().string() + "/cmdline");
        if (is_process_direcory) {
            process_list.push_back(Process(directory.path()));
        }
    }
    read();
}

bool SystemMonitor::read() {

    {
        std::ifstream  uptime_file {"/proc/uptime"};
        
        if (!uptime_file.is_open()) {
            std::cout << "Failed to open /proc/uptime\n";
            return false;
        }

        std::string line;
        std::getline(uptime_file, line);
        uptime = line.substr(0, line.find(' '));
        idletime = line.substr(0 + line.find(' '), line.size());

    }
    
    {
        process_count.total = process_list.size();
        for (auto &process : process_list) {
            if (process.state == "R (running)")            
                process_count.running++;
            else if (process.state == "S (sleeping)")            
                process_count.sleeping++;
            else if (process.state == "I (idle)")            
                process_count.idle++;
            else if (process.state == "Z (zombie)")            
                process_count.sleeping++;
        }
    }
    return true;
}

void SystemMonitor::print() {

    std::cout << "\nSystem Montior\n";
    std::cout << "Uptime:" << uptime << " Ideltime:" << idletime << "\n";
    std::cout << "Process Count:" << process_count.total << " Running:" << process_count.running << " Sleeping:" << process_count.sleeping << " Idel:" << process_count.idle << " Zombie:" << process_count.zombie << "\n";
    std::cout << "\n";

    std::cout << "Name\t" << "Pid\t" << "User\t" << "State\t\t" << "Threads\t\t" << "Start Time\t" << "CPU Time\t" << "CPU load\t" << "Mem Usage\t" <<  "Command\t" << "\n";
    
    for (auto process : process_list) {
            //process.print();
    }
}