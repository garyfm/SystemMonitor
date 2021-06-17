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
    STOPPED,
    END,
};

class Process {

public:
    int pid;
    int uid;
    int memory_used;
    int cpu_load_avg;
    int num_of_threads;
    int cpu_time;
    int ticks_running_on_cpu;
    int starttime;
    std::string process_path;
    std::string name;
    std::string user;
    std::string command;
    PROCESS_STATE state;

    Process(){};
    Process(std::string process_path);
    PROCESS_STATUS read();
    void kill_process();
    void stop_resume_process();

private:
    bool parse_proc_status(); 
    bool parse_proc_commandline(); 
    bool parse_proc_sched(); 
    bool parse_proc_stat(); 
};

#endif /* __PROCESS_H__ */