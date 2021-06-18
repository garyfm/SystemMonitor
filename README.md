Creating a top clone, a simple system monitor for linux. This is a learning project for brushing up on C++.

Displays a list of the current processes. With inforamation on each process such as CPU & Memory Usage, PID, User etc.
The process list can be sorted in ascending/descending order and each process can be killed, stopped or resumed.

## Dependencies:
Ubuntu 20.04

ncurses:
```
sudo apt-get install libncurses5-dev libncursesw5-dev
```
## Build:
```
cd build
make clean
make
```

## Run
```
cd build
./SystemMonitor.elf
```

## Demo
![gif](https://github.com/garyfm/SystemMonitor/blob/master/demo.gif)
