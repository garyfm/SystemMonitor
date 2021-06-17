#include <chrono>
#include <thread>

#include "SystemMonitor.h"
#include "SystemMonitorUI.h"

#include <ncurses.h> // Must be places after my headers ??

int main() {
    int key; 
    SystemMonitor system_monitor;
    SystemMonitorUI ui;

    system_monitor.update();
    ui.init(system_monitor);

    while (1) {
        
        key = wgetch(ui.process_info_w); 
        if (key != ERR) {
            switch (key) {
            case KEY_UP:
                ui.key_up();
                break;
            case KEY_DOWN: 
                ui.key_down();
                break;
            case KEY_RIGHT:
                ui.key_right();
                break;
            case KEY_LEFT:
                ui.key_left();
                break;
            case KEY_F(1):
                endwin();
                return 0;
                break;
            case KEY_F(2):
                ui.update_sort_to_current_col();
                break;
            case KEY_F(3):
                ui.stop_resume_process_under_curser(system_monitor);
                break;
            case KEY_F(4):
                ui.kill_process_under_curser(system_monitor);
                break;
            default:
                break;
            }
        }

        system_monitor.update();
        ui.sort_by_current_col(system_monitor);
        ui.print_header_info(system_monitor);

        for (auto& process: system_monitor.process_list) {
            auto y_pos = &process - &system_monitor.process_list[0]; 
            wmove(ui.process_info_w, y_pos, 0);
            ui.print_process_info(process, system_monitor);
        }
        
        ui.highlight_row_under_input_curser(HIGHLIGHT_ROW::SET);
        ui.reposition_curser_to_input_curser();
        
        ui.draw();
        //std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    return 0;
}

