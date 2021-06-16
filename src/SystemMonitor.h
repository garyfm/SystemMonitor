#ifndef __SYSTEM_MONITOR_H__
#define __SYSTEM_MONITOR_H__

#include <string>
#include <vector>

#include "Process.h"

enum class SORT_ORDER {
    ASCENDING,
    DESCENDING,
};

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
    std::string loadavg;
    int total_cpu_jiffies = 0;
    const int kernal_frequency = 100;

    memory_t physical_memory;
    memory_t swap_memory;
    process_count_t process_count = {0};


    std::vector<Process> process_list;

    void init();
    void update();
    double calc_process_cpu_usage(const int starttime, const int ticks);
    double calc_process_memory_usage(const int memory_used);
    void sort_process_list(const PROCESS_FIELD sort_by, const SORT_ORDER order_by);

private:
    bool read();
    void populate_process_list();
    bool parse_uptime();
    bool parse_meminfo();
    bool parse_loadavg();
    bool parse_stat();
};

#endif /* __SYSTEM_MONITOR_H__ */