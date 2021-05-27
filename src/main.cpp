#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>
#include <filesystem>
#include <chrono>
#include <thread>
#include <mutex>

#include "SystemMonitor.h"

#include <ncurses.h>

#define NUM_FIELDS (10)

std::array<std::string, NUM_FIELDS> process_field_names = {"Name","Pid", "User", "State", "Threads", "Start Time", "CPU Time", "CPU load", "Mem Usage", "Command"};

std::mutex curser_lock;

static int field_spacing = 0;
int input_curser_x, input_curser_y;
int pad_y = 0;

static char get_proc_running_state(const PROCESS_STATE state) {
    if (state == PROCESS_STATE::RUNNING) return 'R';
    else if (state == PROCESS_STATE::IDLE) return 'I';
    else if (state == PROCESS_STATE::SLEEPING) return 'S';
    else if (state == PROCESS_STATE::ZOMBIE) return 'Z';

    return 'X'; // TODO: Better way of handling this 
}

static WINDOW *nc_create_header(const SystemMonitor& system_monitor) {
    WINDOW *header_w;
    header_w = newwin(5, COLS, 0, 0);
    box(header_w, 0, 0);

    wattron(header_w, A_BOLD | A_UNDERLINE);
    mvwprintw(header_w, 1, COLS / 2, "System Montior");
    wstandend(header_w);
    wrefresh(header_w);

    return header_w;
}

static void nc_create_process_field_names() {
    WINDOW *process_field_w;
    process_field_w = newwin(LINES, COLS, 5, 0);
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

static int nc_move_curser_to_next_field(WINDOW *win, int field_index, const int y_pos) {

    int field_pos = 0;

    /* Calulate the sum of the characters 
     * in the previous fields */
    for (int i = 0; i <= field_index; ++i) {
        field_pos += process_field_names[i].size();
    }

    /* Calulate the sume of the spacing between the 
     * previous fields */
    field_pos += field_spacing * (field_index + 1);

    wmove(win, y_pos, field_pos);
    field_index++;

    return field_index;
}

static int nc_move_curser_to_previous_field(WINDOW *win, int field_index, const int y_pos) {

    int field_pos = 0;
    int current_x, current_y;


    getyx(win, current_y, current_x);
    
    field_pos = current_x;
    field_pos -= field_spacing;
    field_pos -= process_field_names[field_index-1].size();

    wmove(win, y_pos, field_pos);

    field_index--;
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
    wprintw(process_info_w, "%c", get_proc_running_state(process.state.second));

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
    clear();
    cbreak();
    keypad(stdscr, TRUE);
    noecho();

    int process_field_char_count = 0;

    for (auto field_name : process_field_names) {
        process_field_char_count += field_name.size();
    }

    field_spacing = (COLS - process_field_char_count) / NUM_FIELDS; 
}

void system_monitor_update(SystemMonitor& system_monitor, WINDOW* header_w, WINDOW* process_info_w) {
    int current_curser_y, current_curser_x;
    while(1) {
        system_monitor.update();
        nc_print_header_info(header_w, system_monitor);

        int y_pos = 0;
        curser_lock.lock();
        wmove(process_info_w, 0, 0);
        for (auto process : system_monitor.process_list) {
            nc_print_process_info(process_info_w, process, y_pos);
            y_pos++;
            wmove(process_info_w, y_pos, 0);
        }
        // Highlight the current line under the input curser
        mvwchgat(process_info_w, input_curser_y, 0, -1, A_STANDOUT, 0, NULL);
        // Restore the curser to the input curser
        wmove(process_info_w, input_curser_y, input_curser_x);
        curser_lock.unlock();

        wrefresh(header_w);
        prefresh(process_info_w, pad_y, 0, 7, 1, LINES -1, COLS - 1);

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main() {
    
    
    SystemMonitor system_monitor;
    
    nc_init();

    WINDOW *header_w = nc_create_header(system_monitor);
    nc_create_process_field_names();
    
    WINDOW *process_info_w = newpad(system_monitor.process_count.total, COLS);
    keypad(process_info_w, TRUE);

    std::thread system_monitor_thread { system_monitor_update, std::ref(system_monitor), header_w, process_info_w};

    int field_index = 0;
    while (1) {
        int key; 
        key = wgetch(process_info_w);

        curser_lock.lock();
        getyx(process_info_w , input_curser_y, input_curser_x);
        switch (key) {
        case KEY_UP:
            if (input_curser_y <= pad_y)
                pad_y--;
            // Move up so clear current line highlighing
            mvwchgat(process_info_w, input_curser_y, 0, -1, A_NORMAL, 0, NULL);
            wmove(process_info_w, --input_curser_y, input_curser_x);
            break;
        case KEY_DOWN:
            if (input_curser_y >= LINES - 8)
                pad_y++;
            // Move down so clear current line highlighing
            mvwchgat(process_info_w, input_curser_y, 0, -1, A_NORMAL, 0, NULL);
            wmove(process_info_w, ++input_curser_y, input_curser_x);
            break;
        case KEY_RIGHT:
            field_index = nc_move_curser_to_next_field(process_info_w, field_index, input_curser_y);
            getyx(process_info_w , input_curser_y, input_curser_x);
            break;
        case KEY_LEFT:
            field_index = nc_move_curser_to_previous_field(process_info_w, field_index, input_curser_y);
            getyx(process_info_w , input_curser_y, input_curser_x);
            break;
        default:
            break;
        }

        prefresh(process_info_w, pad_y, 0, 7, 1, LINES -1, COLS - 1);
        curser_lock.unlock();
    }

    return 0;
}

