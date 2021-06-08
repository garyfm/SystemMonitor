#include <vector>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>

#include "SystemMonitor.h"

enum class SYS_INFO_FIELDS {
    MEM_TOTAL,
    MEM_FREE,
    MEM_SWAPED_TOTAL,
    MEM_SWAPED_FREE,
    END
};

static std::map<std::string, SYS_INFO_FIELDS> system_info_fields = {{"MemTotal", SYS_INFO_FIELDS::MEM_TOTAL}, {"MemFree", SYS_INFO_FIELDS::MEM_FREE}, {"SwapTotal", SYS_INFO_FIELDS::MEM_SWAPED_TOTAL}, {"SwapFree", SYS_INFO_FIELDS::MEM_SWAPED_FREE}};

SystemMonitor::SystemMonitor() {
    populate_process_list(); 
    read();
}



void SystemMonitor::update() {
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

bool SystemMonitor::read() {

    if (!parse_uptime()) return false;
    if (!parse_mem_info()) return false;
    
    process_count.total = process_list.size();
    process_count.running = 0;
    process_count.idle = 0;
    process_count.sleeping = 0;
    process_count.zombie = 0;
    
    for (auto &process : process_list) {
        if (process.state.second == PROCESS_STATE::RUNNING)            
            process_count.running++;
        else if (process.state.second == PROCESS_STATE::SLEEPING)            
            process_count.sleeping++;
        else if (process.state.second == PROCESS_STATE::IDLE)            
            process_count.idle++;
        else if (process.state.second == PROCESS_STATE::ZOMBIE)             
            process_count.zombie++;
    }
    
    return true;
}

bool SystemMonitor::parse_uptime() {
    std::string line;

    std::ifstream  uptime_file {"/proc/uptime"};
    
    if (!uptime_file.is_open()) {
        std::cout << "Failed to open /proc/uptime\n";
        return false;
    }

    std::getline(uptime_file, line);
    uptime = std::stoi(line.substr(0, line.find(' ')));
    idletime = std::stoi(line.substr(0 + line.find(' '), line.size()));
    
    return true;
}

bool SystemMonitor::parse_mem_info() {
    std::string key;
    std::string value;
        
    std::ifstream meminfo_file {"/proc/meminfo"};
    if (!meminfo_file.is_open()) {
        std::cout << "Failed to open /proc/meminfo\n";
        return false;
    }
    
    while(std::getline(meminfo_file, key, ':')) {
        std::getline(meminfo_file, value, '\n'); 
        
        auto valid_key = system_info_fields.find(key);
        if (valid_key == system_info_fields.end())
            continue;

        switch (system_info_fields[key]) {
        case SYS_INFO_FIELDS::MEM_TOTAL:
            physical_memory.total = std::stoi(value);
            break;
        case SYS_INFO_FIELDS::MEM_FREE:
            physical_memory.free = std::stoi(value);
            break;
        case SYS_INFO_FIELDS::MEM_SWAPED_TOTAL:
            swap_memory.total = std::stoi(value);
            break;
        case SYS_INFO_FIELDS::MEM_SWAPED_FREE:
            swap_memory.free = std::stoi(value);
            break;
        default:
            break;
        }
    }
    physical_memory.used = physical_memory.total - physical_memory.free;
    swap_memory.used = swap_memory.total - swap_memory.free;

    return true;
}