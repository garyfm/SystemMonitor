#include <iostream>
#include <fstream>
#include <algorithm>
#include <memory>

#include <pwd.h>
#include "Process.h"

Process::Process(std::string process_path)
    : process_path { process_path } {
    read();
}

PROCESS_STATUS Process::read() {
    if (!parse_proc_status()) return PROCESS_STATUS::FAILED_TO_PARSE_FILE;
    if (!parse_proc_commandline()) return PROCESS_STATUS::FAILED_TO_PARSE_FILE;
    if (!parse_proc_sched()) return PROCESS_STATUS::FAILED_TO_PARSE_FILE;
    return PROCESS_STATUS::OK;
}

bool Process::parse_proc_status() {
    std::unordered_map<std::string, std::string> proc_status; 

    std::string key;
    std::string value;

    std::ifstream process_file {process_path + "/status"};    
    if (!process_file.is_open())
        return false;

    // TODO: Parse process memembers here rather than using multiple find()'s
    while(std::getline(process_file, key, ':')) {
        std::getline(process_file, value, '\n');

        if (key != "Uid")
            value.erase(std::remove(value.begin(), value.end(), '\t'), value.end());

        proc_status.insert({key, value});
    }

    name = proc_status.find("Name")->second;
    pid = proc_status.find("Pid")->second;
    uid = proc_status.find("Uid")->second;
    uid = uid.substr(1, uid.find('\t', 1) - 1);
    state = proc_status.find("State")->second;
    auto mem_usage_found = proc_status.find("VmRSS");
    if (mem_usage_found != proc_status.end())
        mem_usage = mem_usage_found->second;
    num_of_threads = proc_status.find("Threads")->second;
    
    struct passwd *pws = getpwuid(stoi(uid));
    user = pws->pw_name;
    
    return true;
} 

bool Process::parse_proc_commandline() {

    std::ifstream cmdline_file {process_path + "/cmdline", std::ios::binary};
    if (!cmdline_file.is_open())
        return false;

    std::string line;
    getline(cmdline_file, line);

    std::replace(line.begin(), line.end(), '\000', ' ');
    command = line;

    return true;
}

bool Process::parse_proc_sched() {
    std::unordered_map<std::string, std::string> process_sched;     
    std::string key;
    std::string value;

    std::ifstream process_sched_file {process_path + "/sched"};    
    if (!process_sched_file.is_open())
        return false;

    // Skip the first two line 
    std::getline(process_sched_file, value);
    std::getline(process_sched_file, value);
    while(std::getline(process_sched_file, key, ':')) {
        std::getline(process_sched_file, value, '\n');
        key.erase(std::remove(key.begin(), key.end(), ' '), key.end());
        value.erase(std::remove(value.begin(), value.end(), ' '), value.end());
        process_sched.insert({key, value});
    }
    cpu_time = process_sched.find("se.sum_exec_runtime")->second;
    cpu_load_avg = process_sched.find("se.avg.runnable_avg")->second;
    start_time = process_sched.find("se.exec_start")->second;

    return true;
}



void Process::print() {
    std::cout << name << "\t" << pid << "\t" << user << "\t" << state << "\t" << num_of_threads << "\t" << start_time << "\t" << cpu_time << "\t" << cpu_load_avg << "\t\t" << mem_usage << "\t" <<  command << "\t" << "\n";
}