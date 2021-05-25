#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
#include <filesystem>
#include <chrono>
#include <thread>

#include "SystemMonitor.h"

#include <ncurses.h>

#define NUM_FIELDS (10)

std::array<std::string, NUM_FIELDS> process_fields = {"Name","Pid", "User", "State", "Threads", "Start Time", "CPU Time", "CPU load", "Mem Usage", "Command"};
int field_spacing = 0;

static WINDOW *nc_create_header(const SystemMonitor& system_monitor) {
    WINDOW *header_w;
    header_w = newwin(5, COLS, 0, 0);
    box(header_w, 0, 0);

    wattron(header_w, A_BOLD | A_UNDERLINE);
    mvwprintw(header_w, 1, COLS / 2, "System Montior");
    wstandend(header_w);

    return header_w;
}

static void nc_create_process_field_names() {
    WINDOW *process_field_w;
    process_field_w = newwin(LINES, COLS, 5, 0);
    std::string spacing(field_spacing, ' ');
    
    wmove(process_field_w, 1,1);
    wattron(process_field_w, A_BOLD);
    for (auto field_name : process_fields) {
        waddstr(process_field_w, field_name.c_str());
        waddstr(process_field_w, spacing.c_str());
    }
    wstandend(process_field_w);

    wrefresh(process_field_w);
}

static int nc_move_curser_to_next_field(WINDOW *win, int field_index, const int y_pos) {

    int field_pos = 0;

    /* Calulate the sum of the characters 
     * in the previous fields */
    for (int i = 0; i <= field_index; ++i) {
        field_pos += process_fields[i].size();
    }

    /* Calulate the sume of the spacing between the 
     * previous fields */
    field_pos += field_spacing * (field_index + 1) + 1;

    wmove(win, y_pos, field_pos);
    field_index++;

    return field_index;
}

static void nc_print_header_info(WINDOW *header_w, const SystemMonitor& system_monitor) {
    mvwprintw(header_w, 2, 1, "Uptime: %s Ideltime: %s", system_monitor.uptime.c_str(), system_monitor.idletime.c_str());
    mvwprintw(header_w, 3, 1, "Process Count:%d Running:%d Sleeping:%d Idle:%d Zombie: %d" ,  system_monitor.process_count.total, system_monitor.process_count.running, system_monitor.process_count.sleeping, system_monitor.process_count.idle, system_monitor.process_count.zombie);
}

static void nc_print_process_info(WINDOW *process_info_w, Process& process, const int y_pos) {
    int field_index = 0;

    wprintw(process_info_w, process.name.second.c_str());

    field_index = nc_move_curser_to_next_field(process_info_w, field_index, y_pos);
    wprintw(process_info_w, "%d", process.pid.second);

    field_index = nc_move_curser_to_next_field(process_info_w, field_index, y_pos);
    wprintw(process_info_w, "%s", process.user.second.c_str());

    field_index = nc_move_curser_to_next_field(process_info_w, field_index, y_pos);
    wprintw(process_info_w, "%c", process.print_proc_running_state());

    field_index = nc_move_curser_to_next_field(process_info_w, field_index, y_pos);
    wprintw(process_info_w, "%d", process.num_of_threads.second);

    field_index = nc_move_curser_to_next_field(process_info_w, field_index, y_pos);
    wprintw(process_info_w, "%d", process.start_time.second);

    field_index = nc_move_curser_to_next_field(process_info_w, field_index, y_pos);
    wprintw(process_info_w, "%d", process.cpu_time.second);

    field_index = nc_move_curser_to_next_field(process_info_w, field_index, y_pos);
    wprintw(process_info_w, "%d", process.cpu_load_avg.second);

    field_index = nc_move_curser_to_next_field(process_info_w, field_index, y_pos);
    wprintw(process_info_w, "%d", process.mem_usage.second);

    field_index = nc_move_curser_to_next_field(process_info_w, field_index, y_pos);
    wprintw(process_info_w, "%s", process.command.second.c_str());
}

static void nc_init() {
    initscr();
    cbreak();
    keypad(stdscr, TRUE);
    noecho();

    int process_field_char_count = 0;

    for (auto field_name : process_fields) {
        process_field_char_count += field_name.size();
    }

    field_spacing = (COLS - process_field_char_count) / NUM_FIELDS; 
}

int main() {
    
    
    SystemMonitor system_monitor;
    
    nc_init();

    WINDOW *header_w = nc_create_header(system_monitor);
    nc_create_process_field_names();
    
    WINDOW *process_info_w = newwin(system_monitor.process_count.total, COLS, 7, 1);

    while (1) {
        system_monitor.update();
        nc_print_header_info(header_w, system_monitor);

        int y_pos = 0;
        for (auto process : system_monitor.process_list) {
            nc_print_process_info(process_info_w, process, y_pos);
            y_pos++;
            wmove(process_info_w, y_pos, 0);
            wrefresh(process_info_w);
        }
    
        wrefresh(header_w);
        wrefresh(process_info_w);
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    endwin();
    return 0;
}

