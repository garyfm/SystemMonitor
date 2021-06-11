#ifndef __PROCESS_H__
#define __PROCESS_H__

#include <string>
#include <map>

enum class PROCESS_FIELD {
    NAME,
    PID,
    UID,
    STATE,
    THREADS,
    CPU_TIME,
    CPU_LOAD,
    MEM_USAGE,
    COMMAND,
    END
};

enum class PROCESS_STATUS {
    OK,
    ERROR,
    FAILED_TO_OPEN_FILE,
    FAILED_TO_PARSE_FILE
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
    std::string process_path;
    std::pair<std::string, PROCESS_STATE> state;
    std::pair<std::string, int> pid;
    std::pair<std::string, int> uid;
    std::pair<std::string, std::string> name;
    std::pair<std::string, std::string> user;
    std::pair<std::string, int> memory_used;
    std::pair<std::string, int> cpu_load_avg;
    std::pair<std::string, int> num_of_threads;
    std::pair<std::string, int> cpu_time;
    std::pair<std::string, int> ticks_running_on_cpu;
    std::pair<std::string, int> starttime;
    std::pair<std::string, std::string> command;

    Process(){};
    Process(std::string process_path);

    PROCESS_STATUS read();

private:
    bool parse_proc_status(); 
    bool parse_proc_commandline(); 
    bool parse_proc_sched(); 
    bool parse_proc_stat(); 
};

#endif /* __PROCESS_H__ */