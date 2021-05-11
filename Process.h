#include <string>
#include <unordered_map>
enum class PROCESS_STATUS {
    OK,
    ERROR,
    FAILED_TO_OPEN_FILE,
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
    Process(const std::string& process_path);
    ~Process(){/* close the proc file */};

    PROCESS_STATUS read(const std::string& process_path);
    void print();

private:
    std::string pid;
    std::string name;
    std::string user;
    std::string cpu_usage;
    std::string mem_usage;
    std::string num_of_threads;
    std::string state;
    std::string time;
    std::string process_path;
    std::string command;

    int parse_pid();
};