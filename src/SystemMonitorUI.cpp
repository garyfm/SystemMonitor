
#include <array>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <iostream> //remove
#include <chrono>
#include <thread>

#include "SystemMonitorUI.h"
#include "Process.h"

#define HEADER_SIZE (7 + 1)
#define START_OF_PROCESS_INFO_ROW (HEADER_SIZE + 2)

static std::array<std::string, (size_t) PROCESS_FIELD::END> process_field_names = {"Name","Pid", "User", "State", "Threads", "CPU Time(M:S)", "CPU Usage(%)", "Memory (KB, %)", "Command"};

void SystemMonitorUI::init(const SystemMonitor& system_monitor) {
    initscr();
    clear();
    cbreak();
    keypad(stdscr, TRUE);
    noecho();

    int process_field_char_count = 0;

    for (auto field_name : process_field_names) {
        process_field_char_count += field_name.size();
    }
    field_spacing = (COLS - process_field_char_count) / (int) PROCESS_FIELD::END; 

    header_w = create_header(system_monitor);
    create_process_field_names();
    
    process_info_w = newpad(system_monitor.process_count.total + 1, COLS * 10); // times 10 here to account for command being large
    keypad(process_info_w, TRUE);
    nodelay(process_info_w, TRUE);
}

void SystemMonitorUI::draw() {
    wrefresh(header_w);
    prefresh(process_info_w, pad_y, 0, START_OF_PROCESS_INFO_ROW, 1, LINES -1, COLS - 1);
}

void SystemMonitorUI::print_header_info(const SystemMonitor& system_monitor) {
    
    auto format_uptime = [] (int time_s) {
       int minutes = time_s / 60;
       int hours = minutes / 60;

       int seconds = time_s % 60;
       minutes = minutes % 60;

       std::stringstream formatted_time_ss;
       formatted_time_ss << std::setw(2) << std::setfill('0') << std::to_string(hours)<< ":" << std::setw(2) << std::setfill('0') << std::to_string(minutes) << ":"  << std::setw(2) << std::setfill('0') << std::to_string(seconds);
       std::string formatted_time = formatted_time_ss.str();
       return formatted_time;
    };
    
    mvwprintw(header_w, 2, 1, "Load Average: %s", system_monitor.loadavg.c_str());
    mvwprintw(header_w, 3, 1, "Uptime: %s Ideltime: %s", format_uptime(system_monitor.uptime).c_str(), format_uptime(system_monitor.idletime).c_str());

    mvwprintw(header_w, 4, 1, "Process Count: %d Total, %d Running, %d Sleeping, %d Idle, %d Zombie" ,  system_monitor.process_count.total, system_monitor.process_count.running, system_monitor.process_count.sleeping, system_monitor.process_count.idle, system_monitor.process_count.zombie);

    auto bytes_to_megabytes = [](int bytes) {
        float megabytes = (float) bytes / 1000;
        return megabytes;
    };

    mvwprintw(header_w, 5, 1, "Memory(MB): %.2f Total, %.2f Free, %.2f Used", bytes_to_megabytes(system_monitor.physical_memory.total), bytes_to_megabytes(system_monitor.physical_memory.free), bytes_to_megabytes(system_monitor.physical_memory.used));
    mvwprintw(header_w, 6, 1, "Swap(MB): %.2f Total, %.2f Free, %.2f Used", bytes_to_megabytes(system_monitor.swap_memory.total), bytes_to_megabytes(system_monitor.swap_memory.free), bytes_to_megabytes(system_monitor.swap_memory.used));
    //mvwprintw(header_w, 7, 1, "[DEBUG] Input X: %d, Input Y: %d Pad Y: %d", input_curser_x, input_curser_y, pad_y);
    mvwprintw(header_w, 2, COLS - 40 , "F1 - Exit");
    mvwprintw(header_w, 3, COLS - 40, "F2 - Sort Ascending/Descending");
    mvwprintw(header_w, 4, COLS - 40, "F3 - Stop/Resume Process");
    mvwprintw(header_w, 5, COLS - 40, "F4 - Kill Process");
}

void SystemMonitorUI::print_process_info(const Process& process, SystemMonitor& system_monitor) {
    int field_index = 0;
    
    auto process_running_state_to_char = [] (const PROCESS_STATE state) {
        if (state == PROCESS_STATE::RUNNING) return 'R';
        else if (state == PROCESS_STATE::IDLE) return 'I';
        else if (state == PROCESS_STATE::SLEEPING) return 'S';
        else if (state == PROCESS_STATE::ZOMBIE) return 'Z';
        else if (state == PROCESS_STATE::STOPPED) return 'T';

        return 'X'; // TODO: Better way of handling this 
    };

    auto format_time_ms = [] (int time_ms) {
        float seconds = (float) time_ms / 1000;
        int minutes = (int) seconds / 60;
        seconds = fmod(seconds , 60);

        std::stringstream formatted_time_ss;
        formatted_time_ss << std::to_string(minutes) << ":" << std::setw(3) << std::to_string(seconds);
        std::string formatted_time = formatted_time_ss.str();
        return formatted_time;
    };
    wclrtoeol(process_info_w);
    wprintw(process_info_w, process.name.c_str());

    field_index = SystemMonitorUI::move_curser_to_next_process_field(field_index);
    wprintw(process_info_w, "%d", process.pid);

    field_index = SystemMonitorUI::move_curser_to_next_process_field(field_index);
    wprintw(process_info_w, "%s", process.user.c_str());

    field_index = SystemMonitorUI::move_curser_to_next_process_field(field_index);
    wprintw(process_info_w, "%c", process_running_state_to_char(process.state));

    field_index = SystemMonitorUI::move_curser_to_next_process_field(field_index);
    wprintw(process_info_w, "%d", process.num_of_threads);

    field_index = SystemMonitorUI::move_curser_to_next_process_field(field_index);
    wprintw(process_info_w, "%s", format_time_ms(process.cpu_time).c_str());

    field_index = SystemMonitorUI::move_curser_to_next_process_field(field_index);
    wprintw(process_info_w, "%.2f", system_monitor.calc_process_cpu_usage(process.starttime, process.ticks_running_on_cpu));

    field_index = SystemMonitorUI::move_curser_to_next_process_field(field_index);
    wprintw(process_info_w, "%d, %.2f", process.memory_used, system_monitor.calc_process_memory_usage(process.memory_used));
    
    field_index = SystemMonitorUI::move_curser_to_next_process_field(field_index);
    wprintw(process_info_w, "%s", process.command.c_str());
}

void SystemMonitorUI::key_down() {
    /* Increament the pad when the curser goes past the screen size 
    * Account for the screen size moveing up by pad_y */
    int bottom_screen_boundary = LINES + pad_y - (START_OF_PROCESS_INFO_ROW + 1);
    if (input_curser_y >= bottom_screen_boundary )
        pad_y++;
    // FIXME: Dont scroll past the last process

    highlight_row_under_input_curser(HIGHLIGHT_ROW::UNSET);
    wmove(process_info_w, ++input_curser_y, input_curser_x);
    highlight_row_under_input_curser(HIGHLIGHT_ROW::SET);

}

void SystemMonitorUI::key_up() {
    if(input_curser_y <= pad_y)
        pad_y--;
    
    highlight_row_under_input_curser(HIGHLIGHT_ROW::UNSET);
    wmove(process_info_w, --input_curser_y, input_curser_x);
    highlight_row_under_input_curser(HIGHLIGHT_ROW::SET);
   
    // Keep pad_y and input_curser_y positive
    if (pad_y < 0 || input_curser_y < 0) {
        pad_y = 0;
        input_curser_y = 0;
    }
}

void SystemMonitorUI::key_right() {
    field_under_curser = move_curser_to_next_process_field(field_under_curser);
    getyx(process_info_w , input_curser_y, input_curser_x);
    proces_field_under_curser = static_cast<PROCESS_FIELD>(static_cast<int>(proces_field_under_curser) + 1);
    //FIXME: Ensure it doesnt go past the screen size
    //FIXME: Allow screen panning to the right
}

void SystemMonitorUI::key_left() {
    field_under_curser = move_curser_to_previous_process_field(field_under_curser);
    getyx(process_info_w , input_curser_y, input_curser_x);
    proces_field_under_curser = static_cast<PROCESS_FIELD>(static_cast<int>(proces_field_under_curser) - 1);
    //FIXME: Ensure it doesnt go past the screen size
    //FIXME: Allow screen panning back to the left
}

void SystemMonitorUI::reposition_curser_to_input_curser() {
    wmove(process_info_w, input_curser_y, input_curser_x);
}
void SystemMonitorUI::highlight_row_under_input_curser(const HIGHLIGHT_ROW set_unset) {
    if (set_unset == HIGHLIGHT_ROW::SET) {
        mvwchgat(process_info_w, input_curser_y, 0, -1, A_STANDOUT, 0, NULL);
    } else if (set_unset == HIGHLIGHT_ROW::UNSET) {
        mvwchgat(process_info_w, input_curser_y, 0, -1, A_NORMAL, 0, NULL);
    }
}

void SystemMonitorUI::update_sort_to_current_col() {
    if (sort_by == proces_field_under_curser)
        order_by = !order_by;
    else
        sort_by = proces_field_under_curser;
}

void SystemMonitorUI::sort_by_current_col(SystemMonitor& system_monitor) {
    system_monitor.sort_process_list(sort_by, static_cast<SORT_ORDER>(order_by)); 
}

void SystemMonitorUI::kill_process_under_curser(SystemMonitor& system_monitor) {
    int process_under_curser = input_curser_y;
    system_monitor.process_list[process_under_curser].kill_process();
}

void SystemMonitorUI::stop_resume_process_under_curser(SystemMonitor& system_monitor) {
    int process_under_curser = input_curser_y;
    system_monitor.process_list[process_under_curser].stop_resume_process();
}

WINDOW* SystemMonitorUI::create_header(const SystemMonitor& system_monitor) {
    WINDOW *header_w;
    header_w = newwin(HEADER_SIZE, COLS, 0, 0);
    box(header_w, 0, 0);

    wattron(header_w, A_BOLD | A_UNDERLINE);
    mvwprintw(header_w, 1, COLS / 2, "System Montior");
    wstandend(header_w);
    wrefresh(header_w);

    return header_w;
}

void SystemMonitorUI::create_process_field_names() {
    WINDOW *process_field_w;
    process_field_w = newwin(LINES, COLS, HEADER_SIZE, 0);
    std::string spacing(field_spacing, ' ');
    
    wmove(process_field_w, 1,1);
    wattron(process_field_w, A_BOLD);
    for (auto field_name : process_field_names) {
        waddstr(process_field_w, field_name.c_str());
        waddstr(process_field_w, spacing.c_str());
    }
    wstandend(process_field_w);

    wrefresh(process_field_w);
}

int SystemMonitorUI::move_curser_to_next_process_field(int field_index) {

    int field_pos = 0;
    int current_x, current_y;

    getyx(process_info_w, current_y, current_x);

    /* Calulate the sum of the characters 
     * in the previous fields */
    for (int i = 0; i <= field_index; ++i) {
        field_pos += process_field_names[i].size();
    }

    /* Calulate the sume of the spacing between the 
     * previous fields */
    field_pos += field_spacing * (field_index + 1);

    wmove(process_info_w, current_y, field_pos);
    field_index++;

    return field_index;
}

int SystemMonitorUI::move_curser_to_previous_process_field(int field_index) {

    int field_pos = 0;
    int current_x, current_y;

    getyx(process_info_w, current_y, current_x);
    
    field_pos = current_x;
    field_pos -= field_spacing;
    field_pos -= process_field_names[field_index-1].size();

    wmove(process_info_w, current_y, field_pos);

    field_index--;

    return field_index;
}