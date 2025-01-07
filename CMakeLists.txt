cmake_minimum_required(VERSION 3.10)
project(Battleship)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(lib/Settings)
include_directories(lib/ReadWriteQueue)
include_directories(lib/Ship)
include_directories(lib/Master)
include_directories(lib/Slave)
include_directories(lib/RandomShips)

set(SOURCES
    bin/master_main.cpp
    bin/slave_main.cpp
    lib/Settings/Settings.cpp
    lib/ReadWriteQueue/ReadWriteQueue.cpp
    lib/Ship/Ship.cpp
    lib/Master/Master.cpp
    lib/Slave/Slave.cpp
    lib/RandomShips/RandomShips.cpp
)

add_executable(master_main.exe
    bin/master_main.cpp
    lib/Settings/Settings.cpp
    lib/ReadWriteQueue/ReadWriteQueue.cpp
    lib/Ship/Ship.cpp
    lib/Master/Master.cpp
    lib/RandomShips/RandomShips.cpp
)


add_executable(slave_main.exe
    bin/slave_main.cpp
    lib/Settings/Settings.cpp
    lib/ReadWriteQueue/ReadWriteQueue.cpp
    lib/Ship/Ship.cpp
    lib/Slave/Slave.cpp
    lib/RandomShips/RandomShips.cpp
)
