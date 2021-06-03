#include <iostream>
#include <fstream>
#include <algorithm>
#include <memory>

#include <pwd.h>
#include "Process.h"

static std::map<std::string, PROCESS_FIELD> process_info_fields = {{"Name", PROCESS_FIELD::NAME}, {"Pid", PROCESS_FIELD::PID}, {"Uid", PROCESS_FIELD::UID}, {"State", PROCESS_FIELD::STATE}, {"Threads", PROCESS_FIELD::THREADS}, {"se.exec_start", PROCESS_FIELD::START_TIME}, {"se.sum_exec_runtime", PROCESS_FIELD::CPU_TIME}, {"se.avg.runnable_avg", PROCESS_FIELD::CPU_LOAD}, {"VmRSS", PROCESS_FIELD::MEM_USAGE}, {"Command", PROCESS_FIELD::COMMAND}};

static PROCESS_STATE parse_proc_running_state(const std::string& raw_state_string) {
    if (raw_state_string == "R (running)") return PROCESS_STATE::RUNNING;
    else if (raw_state_string == "I (idle)") return PROCESS_STATE::IDLE;
    else if (raw_state_string == "S (sleeping)") return PROCESS_STATE::SLEEPING;
    else if (raw_state_string == "Z (zombie)") return PROCESS_STATE::ZOMBIE;

    return PROCESS_STATE::END;
}

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
    std::string key;
    std::string value;

    std::ifstream process_file {process_path + "/status"};    
    if (!process_file.is_open())
        return false;

    while(std::getline(process_file, key, ':')) {
        std::getline(process_file, value, '\n');

        auto valid_key = process_info_fields.find(key);
        if (valid_key == process_info_fields.end())
            continue;

        switch (process_info_fields[key]) {
        case PROCESS_FIELD::NAME:
            value.erase(std::remove(value.begin(), value.end(), '\t'), value.end());
            name = {key, value};
            break;
        case PROCESS_FIELD::PID:
            value.erase(std::remove(value.begin(), value.end(), '\t'), value.end());
            pid = {key, std::stoi(value)};
            break;
        case PROCESS_FIELD::UID: {
            struct passwd *pws = getpwuid(std::stoi(value));
            user = {key, pws->pw_name};
            break;
        }
        case PROCESS_FIELD::STATE:
            value.erase(std::remove(value.begin(), value.end(), '\t'), value.end());
            state = {key, parse_proc_running_state(value)};
            break;
        case PROCESS_FIELD::THREADS:
            value.erase(std::remove(value.begin(), value.end(), '\t'), value.end());
            num_of_threads = {key, std::stoi(value)};
            break;
        case PROCESS_FIELD::START_TIME:
            value.erase(std::remove(value.begin(), value.end(), '\t'), value.end());
            start_time = {key, std::stoi(value)};
            break;
        case PROCESS_FIELD::CPU_TIME:
            value.erase(std::remove(value.begin(), value.end(), '\t'), value.end());
            cpu_time = {key, std::stoi(value)};
            break;
        case PROCESS_FIELD::CPU_LOAD:
            value.erase(std::remove(value.begin(), value.end(), '\t'), value.end());
            cpu_load_avg = {key, std::stoi(value)};
            break;
        case PROCESS_FIELD::MEM_USAGE:
            value.erase(std::remove(value.begin(), value.end(), '\t'), value.end());
            mem_usage = {key, std::stoi(value)};
            break;
        case PROCESS_FIELD::COMMAND:
            value.erase(std::remove(value.begin(), value.end(), '\t'), value.end());
            command = {key, value};
            break;
        default:
            break;
        }
    }

    return true;
} 

bool Process::parse_proc_commandline() {

    std::ifstream cmdline_file {process_path + "/cmdline", std::ios::binary};
    if (!cmdline_file.is_open())
        return false;

    std::string line;
    getline(cmdline_file, line);

    std::replace(line.begin(), line.end(), '\000', ' ');
    command = {"Command", line};

    return true;
}

bool Process::parse_proc_sched() {
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

        auto valid_key = process_info_fields.find(key);
        if (valid_key == process_info_fields.end())
            continue;

        switch (process_info_fields[key]) {
        case PROCESS_FIELD::CPU_TIME:
            cpu_time = {"CPU Time", std::stoi(value)};
            break;
        case PROCESS_FIELD::START_TIME:
            start_time = {"Start Time", std::stoi(value)};
            break;
        case PROCESS_FIELD::CPU_LOAD:
            cpu_load_avg = {"CPU Load", std::stoi(value)};
            break;
        default:
            break;
        }

    }

    return true;
}