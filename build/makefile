CC = g++
CFLAGS = -g -Wall -std=c++17
SRC_DIR = ../src
OBJ_DIR = build
LIBS = -lncurses -pthread

main: main.o SystemMonitor.o SystemMonitorUI.o
	$(CC) $(CFLAGS) -o SystemMonitor.elf main.o SystemMonitor.o SystemMonitorUI.o Process.o $(LIBS)

main.o: $(SRC_DIR)/main.cpp $(SRC_DIR)/SystemMonitor.h $(SRC_DIR)/SystemMonitorUI.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/main.cpp

SystemMonitor.o: Process.o $(SRC_DIR)/SystemMonitor.cpp $(SRC_DIR)/SystemMonitor.h $(SRC_DIR)/Process.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/SystemMonitor.cpp
	
SystemMonitorUI.o: $(SRC_DIR)/SystemMonitorUI.cpp $(SRC_DIR)/SystemMonitor.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/SystemMonitorUI.cpp

Process.o: $(SRC_DIR)/Process.h
	$(CC) $(CFLAGS) -c $(SRC_DIR)/Process.cpp

clean:
	$(RM) SystemMonitor.elf *.o
