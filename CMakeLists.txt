cmake_minimum_required(VERSION 3.10)
project(Battleship)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories("${CMAKE_CURRENT_SOURCE_DIR}/lib")

set(SOURCES
    bin/master_main.cpp
    bin/slave_main.cpp
    lib/Settings/Settings.cpp
    lib/ReadWriteQueue/ReadWriteQueue.cpp
    lib/Ship/Ship.cpp
    lib/Master/Master.cpp
    lib/Slave/Slave.cpp
    lib/RandomShips/RandomShips.cpp
    lib/Commands/MasterCommandHandler.cpp
    lib/Commands/SlaveCommandHandler.cpp
    lib/Utils/FileUtils.cpp
    lib/Utils/PrintUtils.cpp
)

add_executable(master_main
    bin/master_main.cpp
    lib/Settings/Settings.cpp
    lib/ReadWriteQueue/ReadWriteQueue.cpp
    lib/Ship/Ship.cpp
    lib/Master/Master.cpp
    lib/RandomShips/RandomShips.cpp
    lib/Commands/MasterCommandHandler.cpp
    lib/Utils/FileUtils.cpp
    lib/Utils/PrintUtils.cpp
)


add_executable(slave_main
    bin/slave_main.cpp
    lib/Settings/Settings.cpp
    lib/ReadWriteQueue/ReadWriteQueue.cpp
    lib/Ship/Ship.cpp
    lib/Slave/Slave.cpp
    lib/RandomShips/RandomShips.cpp
    lib/Commands/SlaveCommandHandler.cpp
    lib/Utils/FileUtils.cpp
    lib/Utils/PrintUtils.cpp
)
