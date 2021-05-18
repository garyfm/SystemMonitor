SystemMonitor:
	g++ -g -std=c++17 -o SystemMonitor.elf  main.cpp SystemMonitor.cpp Process.cpp  

clean:
	rm SystemMonitor.elf