#ifndef __PROCESS_H__
#define __PROCESS_H__

#include <string>
#include <unordered_map>

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

class Process {

public:
    Process(){};
    Process(std::string process_path);
    ~Process(){/* close the proc file */};

    PROCESS_STATUS read();
    void print();

    std::string process_path;
    std::string state;

private:
    std::string pid;
    std::string uid;
    std::string name;
    std::string user;
    std::string mem_usage;
    std::string num_of_threads;
    std::string cpu_time;
    std::string cpu_load_avg;
    std::string start_time;
    std::string command;

    bool parse_proc_status(); 
    bool parse_proc_commandline(); 
    bool parse_proc_sched(); 
};

#endif /* __PROCESS_H__ */