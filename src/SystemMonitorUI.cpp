
#include <array>
#include <sstream>
#include <iomanip>
#include <cmath>


#include "SystemMonitorUI.h"
#include "Process.h"

#define HEADER_SIZE (6 + 1)
#define START_OF_PROCESS_INFO_ROW (HEADER_SIZE + 2)

static std::array<std::string, (size_t) PROCESS_FIELD::END> process_field_names = {"Name","Pid", "User", "State", "Threads", "CPU Time (M:S)", "CPU load", "Memory (KB, %)", "Command"};

static std::string format_time(int time_ms) {
       float seconds = (float) time_ms / 1000;
       int minutes = (int) seconds / 60;
       seconds = fmod(seconds , 60);

       std::stringstream formatted_time_ss;
       formatted_time_ss << std::to_string(minutes) << ":" << std::to_string(seconds);
       std::string formatted_time = formatted_time_ss.str();
       return formatted_time;
};

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
    
    process_info_w = newpad(system_monitor.process_count.total + 1, COLS);
    keypad(process_info_w, TRUE);
    nodelay(process_info_w, TRUE);
}

void SystemMonitorUI::draw() {
    wrefresh(header_w);
    prefresh(process_info_w, pad_y, 0, START_OF_PROCESS_INFO_ROW, 1, LINES -1, COLS - 1);
}

void SystemMonitorUI::print_header_info(const SystemMonitor& system_monitor) {
    mvwprintw(header_w, 2, 1, "Uptime(M:S) : %s Ideltime: %s", format_time(system_monitor.uptime).c_str(), format_time(system_monitor.idletime).c_str());

    mvwprintw(header_w, 3, 1, "Process Count: %d Total, %d Running, %d Sleeping, %d Idle, %d Zombie" ,  system_monitor.process_count.total, system_monitor.process_count.running, system_monitor.process_count.sleeping, system_monitor.process_count.idle, system_monitor.process_count.zombie);

    auto bytes_to_megabytes = [](int bytes) {
        float megabytes = (float) bytes / 1000;
        return megabytes;
    };

    mvwprintw(header_w, 4, 1, "Memory(MB) : %.2f Total, %.2f Free, %.2f Used", bytes_to_megabytes(system_monitor.physical_memory.total), bytes_to_megabytes(system_monitor.physical_memory.free), bytes_to_megabytes(system_monitor.physical_memory.used));
    mvwprintw(header_w, 5, 1, "Swap(MB) : %.2f Total, %.2f Free, %.2f Used", bytes_to_megabytes(system_monitor.swap_memory.total), bytes_to_megabytes(system_monitor.swap_memory.free), bytes_to_megabytes(system_monitor.swap_memory.used));
    //mvwprintw(header_w, 4, 1, "[DEBUG] Input X: %d, Input Y: %d Pad Y: %d", input_curser_x, input_curser_y, pad_y);
}

void SystemMonitorUI::print_process_info(const Process& process, const SystemMonitor& system_monitor) {
    int field_index = 0;
    
    auto process_running_state_to_char = [] (const PROCESS_STATE state) {
        if (state == PROCESS_STATE::RUNNING) return 'R';
        else if (state == PROCESS_STATE::IDLE) return 'I';
        else if (state == PROCESS_STATE::SLEEPING) return 'S';
        else if (state == PROCESS_STATE::ZOMBIE) return 'Z';

        return 'X'; // TODO: Better way of handling this 
    };

    wprintw(process_info_w, process.name.second.c_str());

    field_index = SystemMonitorUI::move_curser_to_next_process_field(field_index);
    wprintw(process_info_w, "%d", process.pid.second);

    field_index = SystemMonitorUI::move_curser_to_next_process_field(field_index);
    wprintw(process_info_w, "%s", process.user.second.c_str());

    field_index = SystemMonitorUI::move_curser_to_next_process_field(field_index);
    wprintw(process_info_w, "%c", process_running_state_to_char(process.state.second));

    field_index = SystemMonitorUI::move_curser_to_next_process_field(field_index);
    wprintw(process_info_w, "%d", process.num_of_threads.second);

    field_index = SystemMonitorUI::move_curser_to_next_process_field(field_index);
    wprintw(process_info_w, "%s", format_time(process.cpu_time.second).c_str());

    field_index = SystemMonitorUI::move_curser_to_next_process_field(field_index);
    wprintw(process_info_w, "%d", process.cpu_load_avg.second);

    field_index = SystemMonitorUI::move_curser_to_next_process_field(field_index);
    float percentage_used = 0.0;
    if (process.memory_used.second != 0)
        percentage_used = ((float) process.memory_used.second / (float) system_monitor.physical_memory.total) * 100;

    wprintw(process_info_w, "%d, %.2f", process.memory_used.second, percentage_used);

    field_index = SystemMonitorUI::move_curser_to_next_process_field(field_index);
    wprintw(process_info_w, "%s", process.command.second.c_str());
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
    //FIXME: Ensure it doesnt go past the screen size
    //FIXME: Allow screen panning to the right
}

void SystemMonitorUI::key_left() {
    field_under_curser = move_curser_to_previous_process_field(field_under_curser);
    getyx(process_info_w , input_curser_y, input_curser_x);
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