#include <iostream>
#include <fstream>
#include <algorithm>

#include "Process.h"

Process::Process(const std::string& process_path) {
    std::cout << "Populating Process from: " << process_path<< "\n";
    read(process_path);
}

PROCESS_STATUS Process::read(const std::string& process_path) {
    std::unordered_map<std::string, std::string> proc_status; 
    {

        std::string key;
        std::string value;
    
    
        std::ifstream process_file {process_path + "/status"};    
        if (!process_file.is_open()) {
            return PROCESS_STATUS::FAILED_TO_OPEN_FILE; 
        }
    
        while(std::getline(process_file, key, ':')) {
            std::getline(process_file, value, '\n');
            value.erase(std::remove(value.begin(), value.end(), '\t'), value.end());
            proc_status.insert({key, value});
        }
    }
    name = proc_status.find("Name")->second;
    pid = proc_status.find("Pid")->second;
    state = proc_status.find("State")->second;
    mem_usage = proc_status.find("VmRSS")->second;
    num_of_threads = proc_status.find("Threads")->second;

    return PROCESS_STATUS::OK;
}

void Process::print() {

    std::cout << name << "\t" << pid << "\t" << user << "\t" << state << "\t" << num_of_threads << "\t" << mem_usage << "\t" <<  command << "\t" << "\n";
}