#ifndef __PROCESS_H__
#define __PROCESS_H__

#include <string>
#include <unordered_map>
#include <utility>
#include <chrono>
#include <map>

enum class PROCESS_STATUS {
    OK,
    ERROR,
    FAILED_TO_OPEN_FILE,
    FAILED_TO_PARSE_FILE,
};

enum class PROCESS_FIELD {
    NAME,
    PID,
    UID,
    STATE,
    THREADS,
    START_TIME,
    CPU_TIME,
    CPU_LOAD,
    MEM_USAGE,
    COMMAND
};

enum class PROCESS_STATE {
    RUNNING,
    IDLE,
    SLEEPING,
    ZOMBIE,
    END,
};

class Process {

public:
    Process(){};
    Process(std::string process_path);

    PROCESS_STATUS read();
    char print_proc_running_state();

    void print();

    std::string process_path;
    std::pair<std::string, PROCESS_STATE> state;
    std::pair<std::string, int> pid;
    std::pair<std::string, int> uid;
    std::pair<std::string, std::string> name;
    std::pair<std::string, std::string> user;
    std::pair<std::string, int> mem_usage;
    std::pair<std::string, int> cpu_load_avg;
    std::pair<std::string, int> num_of_threads;
    std::pair<std::string, int> cpu_time;
    std::pair<std::string, int> start_time;
    std::pair<std::string, std::string> command;

    std::map<std::string, PROCESS_FIELD> process_info_fields = {{"Name", PROCESS_FIELD::NAME}, {"Pid", PROCESS_FIELD::PID}, {"Uid", PROCESS_FIELD::UID}, {"State", PROCESS_FIELD::STATE}, {"Threads", PROCESS_FIELD::THREADS}, {"se.exec_start", PROCESS_FIELD::START_TIME}, {"se.sum_exec_runtime", PROCESS_FIELD::CPU_TIME}, {"se.avg.runnable_avg", PROCESS_FIELD::CPU_LOAD}, {"VmRSS", PROCESS_FIELD::MEM_USAGE}, {"Command", PROCESS_FIELD::COMMAND}};

private:
    bool parse_proc_status(); 
    bool parse_proc_commandline(); 
    bool parse_proc_sched(); 
    PROCESS_STATE parse_proc_running_state(const std::string& state); 

};

#endif /* __PROCESS_H__ */