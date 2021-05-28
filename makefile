SystemMonitor:
	g++ -g -std=c++17 -o SystemMonitor.elf  src/*.cpp -lncurses -pthread

clean:
	rm SystemMonitor.elf

run:
ifneq ("$(wildcard ./SystemMonitor.elf)", "")
	rm SystemMonitor.elf
endif
	g++ -g -std=c++17 -o SystemMonitor.elf  src/*.cpp -lncurses -pthread
	./SystemMonitor.elf