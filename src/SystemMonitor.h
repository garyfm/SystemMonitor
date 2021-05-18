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
    void print();
    bool read();

private:
    std::vector<Process> process_list;

    std::string uptime;
    std::string idletime;
    std::string mem_usage;
    std::string cpu_usage;
    std::string load;
    process_count_t process_count = {0};
    int users_count;

};
#endif /* __SYSTEM_MONITOR_H__ */