#ifndef __PROCESS_H__
#define __PROCESS_H__

#include <string>
#include <unordered_map>
#include <utility>
#include <chrono>

enum class PROCESS_STATUS {
    OK,
    ERROR,
    FAILED_TO_OPEN_FILE,
    FAILED_TO_PARSE_FILE,
};

enum class PROCESS_FEILD {
    NAME,
    PID,
    USER,
    CPU_USAGE,
    MEM_USAGE,
};

enum class PROCESS_STATE {
    RUNNING,
    IDLE,
    SLEEPING,
    ZOMBIE,
};

class Process {

public:
    Process(){};
    Process(std::string process_path);

    PROCESS_STATUS read();
    void print();

    std::string process_path;
    //std::pair<std::string, PROCESS_STATE> state;
    std::pair<std::string, std::string> state;
    std::pair<std::string, int> pid;
    std::pair<std::string, int> uid;
    std::pair<std::string, std::string> name;
    std::pair<std::string, std::string> user;
    std::pair<std::string, float> mem_usage;
    std::pair<std::string, int> cpu_load_avg;
    std::pair<std::string, int> num_of_threads;
    //std::pair<std::string, std::chrono::duration> cpu_time;
    //std::pair<std::string, std::chrono::duration> start_time;
    std::pair<std::string, float> cpu_time;
    std::pair<std::string, float> start_time;
    std::pair<std::string, std::string> command;


private:
    bool parse_proc_status(); 
    bool parse_proc_commandline(); 
    bool parse_proc_sched(); 
};

#endif /* __PROCESS_H__ */