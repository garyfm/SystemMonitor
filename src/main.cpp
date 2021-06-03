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
#include "SystemMonitorUI.h"

#include <ncurses.h> // Must be places after my headers ??

void system_monitor_update(SystemMonitorUI& ui, SystemMonitor& system_monitor) {
    while(1) {
        int y_pos = 0;

        system_monitor.update();
        ui.print_header_info(system_monitor);

        ui.curser_lock.lock();
        
        wmove(ui.process_info_w, 0, 0);
        for (auto process : system_monitor.process_list) {
            ui.print_process_info(process, y_pos);
            y_pos++;
            wmove(ui.process_info_w, y_pos, 0);
        }

        // Restore the curser to the input curser
        wmove(ui.process_info_w, ui.input_curser_y, ui.input_curser_x);
        // Highlight the current line under the input curser
        mvwchgat(ui.process_info_w, ui.input_curser_y, 0, -1, A_STANDOUT, 0, NULL);
        ui.curser_lock.unlock();

        ui.draw();

        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
}

int main() {
    int field_index = 0;
    int key; 
    
    SystemMonitor system_monitor;
    SystemMonitorUI ui;
    
    ui.init(system_monitor);

    
    std::thread system_monitor_thread { system_monitor_update, std::ref(ui), std::ref(system_monitor)};

    while (1) {
        key = wgetch(ui.process_info_w); // FIXME: This is blocking. Maybe find interrupt based solution ?

        ui.curser_lock.lock();
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

        ui.draw();
        ui.curser_lock.unlock();
    }

    return 0;
}

