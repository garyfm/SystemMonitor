SystemMonitor:
	g++ -g -std=c++17 -o SystemMonitor.elf  src/*.cpp 

clean:
	rm SystemMonitor.elf

run:
	rm SystemMonitor.elf
	g++ -g -std=c++17 -o SystemMonitor.elf  src/*.cpp 
	./SystemMonitor.elf