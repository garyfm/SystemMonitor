#include <vector>
#include <iostream>
#include <filesystem>
#include <fstream>

#include "SystemMonitor.h"

SystemMonitor::SystemMonitor() {
    read();
    populate_process_list(); 
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
        process_count.running = 0;
        process_count.idle = 0;
        process_count.sleeping = 0;
        process_count.zombie = 0;
        
        for (auto &process : process_list) {
            if (process.state.second == "R (running)")            
                process_count.running++;
            else if (process.state.second == "S (sleeping)")            
                process_count.sleeping++;
            else if (process.state.second == "I (idle)")            
                process_count.idle++;
            else if (process.state.second == "Z (zombie)")            
                process_count.zombie++;
        }
    }
    return true;
}

void SystemMonitor::update() {
    read();
    process_list.clear(); 
    populate_process_list(); 
    read();

}

void SystemMonitor::populate_process_list () {
    std::filesystem::directory_iterator proc_directory {"/proc"};

    for (auto directory : std::filesystem::directory_iterator(proc_directory)) {
        bool is_process_direcory = std::filesystem::exists(directory.path().string() + "/cmdline");
        if (is_process_direcory) {
            process_list.push_back(Process(directory.path()));
        }
    }
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