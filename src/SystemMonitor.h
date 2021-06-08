#ifndef __SYSTEM_MONITOR_H__
#define __SYSTEM_MONITOR_H__

#include <string>
#include <vector>

#include "Process.h"

struct process_count_t {
    int total;
    int running;
    int sleeping;
    int idle;
    int zombie;
};

struct memory_t {
    int total;
    int used;
    int free;
};

class SystemMonitor {
public:
        
    int uptime;
    int idletime;
    int users_count;
    std::string load;

    memory_t physical_memory;
    memory_t swap_memory;
    process_count_t process_count = {0};

    std::vector<Process> process_list;

    SystemMonitor();
    void update();

private:
    bool read();
    void populate_process_list();
    bool parse_uptime();
    bool parse_mem_info();
};

#endif /* __SYSTEM_MONITOR_H__ */