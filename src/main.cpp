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

#define START_OF_PROCESS_INFO_ROW (7)

void system_monitor_update(SystemMonitorUI& ui, SystemMonitor& system_monitor) {
    while(1) {
        system_monitor.update();
        ui.print_header_info(system_monitor);

        int y_pos = 0;
        ui.curser_lock.lock();
        wmove(ui.process_info_w, 0, 0);
        for (auto process : system_monitor.process_list) {
            ui.print_process_info(ui.process_info_w, process, y_pos);
            y_pos++;
            wmove(ui.process_info_w, y_pos, 0);
        }
        // Highlight the current line under the input curser
        mvwchgat(ui.process_info_w, ui.input_curser_y, 0, -1, A_STANDOUT, 0, NULL);
        // Restore the curser to the input curser
        wmove(ui.process_info_w, ui.input_curser_y, ui.input_curser_x);
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
        ui.update_input_curser();
        key = wgetch(ui.process_info_w);

        ui.curser_lock.lock();
        switch (key) {
        case KEY_UP:
            if (ui.input_curser_y <= ui.pad_y)
                ui.pad_y--;
            // Move up so clear current line highlighing
            mvwchgat(ui.process_info_w, ui.input_curser_y, 0, -1, A_NORMAL, 0, NULL);
            wmove(ui.process_info_w, --ui.input_curser_y, ui.input_curser_x);
            break;
        case KEY_DOWN: {
                /* Increament the pad when the curser goes past the screen size 
                * Account for the screen size moveing up by ui.pad_y */
                int bottom_screen_boundary = LINES + ui.pad_y - (START_OF_PROCESS_INFO_ROW + 1);
                if (ui.input_curser_y >= bottom_screen_boundary )
                    ui.pad_y++;
                // Move down so clear current line highlighing
                mvwchgat(ui.process_info_w, ui.input_curser_y, 0, -1, A_NORMAL, 0, NULL);
                wmove(ui.process_info_w, ++ui.input_curser_y, ui.input_curser_x);
                break;
            }
        case KEY_RIGHT:
            field_index = ui.move_curser_to_next_field(ui.process_info_w, field_index, ui.input_curser_y);
            ui.update_input_curser();
            break;
        case KEY_LEFT:
            field_index = ui.move_curser_to_previous_field(ui.process_info_w, field_index, ui.input_curser_y);
            ui.update_input_curser();
            break;
        default:
            break;
        }

        // Keey ui.pad_y and ui.input_curser_y positive
        if (ui.pad_y < 0 || ui.input_curser_y < 0) {
            ui.pad_y = 0;
            ui.input_curser_y = 0;
        }
        //ui.draw();
        prefresh(ui.process_info_w, ui.pad_y, 0, START_OF_PROCESS_INFO_ROW, 1, LINES -1, COLS - 1);
        ui.curser_lock.unlock();
    }

    return 0;
}

