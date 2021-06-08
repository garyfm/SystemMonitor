#include "SystemMonitor.h"
#include "SystemMonitorUI.h"

#include <ncurses.h> // Must be places after my headers ??

int main() {
    int key; 
    
    SystemMonitor system_monitor;
    SystemMonitorUI ui;
    
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
            default:
                break;
            }
        }

        system_monitor.update();
        ui.print_header_info(system_monitor);
        
        for (auto& process: system_monitor.process_list) {
            auto y_pos = &process - &system_monitor.process_list[0]; 
            wmove(ui.process_info_w, y_pos, 0);
            ui.print_process_info(process);
        }
        
        ui.highlight_row_under_input_curser(HIGHLIGHT_ROW::SET);
        ui.reposition_curser_to_input_curser();
        
        ui.draw();
    }

    return 0;
}

