#ifndef __SYSTEM_MONTOR_UI_H__
#define __SYSTEM_MONTOR_UI_H__

#include <mutex>

#include "Process.h"
#include "SystemMonitor.h"

#include <ncurses.h>

enum class HIGHLIGHT_ROW {
    SET,
    UNSET
};

class SystemMonitorUI {
public:
    std::mutex curser_lock;
    
    WINDOW *header_w;
    WINDOW *process_info_w;

    void init(const SystemMonitor& System_monitor);
    void draw();
    void key_down();
    void key_up();
    void key_right();
    void key_left();
    void print_header_info(const SystemMonitor& system_monitor);
    void print_process_info(Process& process, const int y_pos);
    void reposition_curser_to_input_curser();
    void highlight_row_under_input_curser(HIGHLIGHT_ROW set_unset);

private:
    int field_spacing = 0;
    int field_under_curser = 0;
    int input_curser_x = 0, input_curser_y = 0;
    int pad_y = 0;
    
    WINDOW* create_header(const SystemMonitor& system_monitor);
    void create_process_field_names();
    int move_curser_to_next_process_field(int field_index, const int y_pos);
    int move_curser_to_previous_process_field(int field_index, const int y_pos);
};


#endif /* __SYSTEM_MONTOR_UI_H__ */