#ifndef __SYSTEM_MONITOR_H__
#define __SYSTEM_MONITOR_H__
#include <string>

#include "Process.h"

struct process_count_t {
    int total;
    int running;
    int sleeping;
    int idle;
    int zombie;
};

class SystemMonitor {

public:
    SystemMonitor();
    bool read();
    void update();
    void populate_process_list();
    
    void print();

    std::string uptime;
    std::string idletime;
    std::string mem_usage;
    std::string cpu_usage;
    std::string load;
    process_count_t process_count = {0};
    int users_count;
    std::vector<Process> process_list;

private:

    
};
#endif /* __SYSTEM_MONITOR_H__ */