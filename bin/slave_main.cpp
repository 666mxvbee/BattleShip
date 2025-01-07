#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>
#include <mutex>
#include <filesystem>
#include "../lib/ReadWriteQueue/ReadWriteQueue.h"
#include "../lib/Settings/Settings.h"
#include "../lib/Slave/Slave.h"

namespace fs = std::filesystem;

const std::string SETTINGS_FILE = "../game_data/settings.txt";
const std::string MASTER_SHIPS_FILE = "../game_data/master_ships.txt";
const std::string SLAVE_SHIPS_FILE = "../game_data/slave_ships.txt";
const std::string QUEUE_FILE = "../game_data/queue.txt";
const std::string SHOTS_FILE = "../game_data/shots.txt";

bool stopFlag = false;

struct SlaveCachedSettings {
    std::string mode;
    uint64_t width;
    uint64_t height;
    bool start;
    bool stop;
    bool exitFlag;

    SlaveCachedSettings() : mode(""), width(0), height(0), start(false), stop(false), exitFlag(false) {}
};

void saveSettingsToFile(const std::string& filename, const Settings& settings) {
    static std::mutex file_mutex;
    std::lock_guard<std::mutex> lock(file_mutex);

    std::ofstream out(filename, std::ios::trunc);
    if (!out.is_open()) {
        std::cerr << "Slave: Unable to open " << filename << " for writing." << std::endl;
        return;
    }
    out << "mode=" << settings.getMode() << "\n";
    out << "width=" << settings.getWidth() << "\n";
    out << "height=" << settings.getHeight() << "\n";
    for (const auto& [type, count] : settings.getShipCounts()) {
        out << "count=" << type << " " << count << "\n";
    }
    if (settings.hasLastShot()) {
        auto shot = settings.getLastShot();
        out << "lastShot=" << shot.first << " " << shot.second << "\n";
    }
    out << "lastResult=" << settings.getLastResult() << "\n";
    out << "start=" << (settings.getStart() ? "1" : "0") << "\n";
    out << "stop=" << (settings.getStop() ? "1" : "0") << "\n";
    out << "exit=" << (settings.getExit() ? "1" : "0") << "\n";
    out.close();
    // std::cout << "Slave: Settings saved to " << filename << std::endl;
}

int main() {
    Settings settings;
    ReadWriteQueue manager;
    Slave s(settings);

    std::cout << "Slave: Initializing game settings..." << std::endl;

    try {
        fs::create_directories("../game_data");
        std::cout << "Slave: Ensured that game_data directory exists." << std::endl;
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Slave: Failed to create game_data directory: " << e.what() << std::endl;
        return 1;
    }

    {
        std::ofstream settingsFile(SETTINGS_FILE, std::ios::trunc);
        if (!settingsFile.is_open()) {
            std::cerr << "Slave: Unable to open " << SETTINGS_FILE << " for writing." << std::endl;
            return 1;
        }
        settingsFile.close();

        std::ofstream shotsFile(SHOTS_FILE, std::ios::trunc);
        if (!shotsFile.is_open()) {
            std::cerr << "Slave: Unable to open " << SHOTS_FILE << " for writing." << std::endl;
            return 1;
        }
        shotsFile.close();

        std::ofstream queueFile(QUEUE_FILE, std::ios::trunc);
        if (!queueFile.is_open()) {
            std::cerr << "Slave: Unable to open " << QUEUE_FILE << " for writing." << std::endl;
            return 1;
        }
        // Slave ходит вторым
        queueFile << "2";
        queueFile.close();
        std::cout << "Slave: Initialized queue.txt with Slave's turn." << std::endl;
    }

    std::cout << "Slave: Waiting for game to start..." << std::endl;

    SlaveCachedSettings cached;
    int lastQueueValue = -1;

    while (!settings.getStart() && !settings.getExit()) {
        std::ifstream in(SETTINGS_FILE);
        if (in.is_open()) {
            std::string line;

            std::string newMode = cached.mode;
            uint64_t newWidth = cached.width;
            uint64_t newHeight  = cached.height;
            bool newStart = cached.start;
            bool newStop = cached.stop;
            bool newExit = cached.exitFlag;


            while (std::getline(in, line)) {
                std::size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = line.substr(0, pos);
                    std::string value = line.substr(pos + 1);


                    if (key == "mode") {
                        if (value == "slave") newMode = value;

                    } else if (key == "width") {
                        try { newWidth = std::stoull(value); } catch (...) {}
                    } else if (key == "height") {
                        try { newHeight = std::stoull(value); } catch(...) {}
                    } else if (key == "count") {
                        settings.setCount(value);
                    } else if (key == "start") {
                        newStart = (value == "1");
                    } else if (key == "stop") {
                        newStop = (value == "1");
                    } else if (key == "exit") {
                        newExit = (value == "1");
                    }
                }
            }
            in.close();

            
            if (newMode != cached.mode) {
                cached.mode = newMode;
                settings.setMode(newMode); // Обновим
                std::cout << "Slave: Mode set to " << newMode << std::endl;
            }
            if (newWidth != cached.width) {
                cached.width = newWidth;
                // Установим реально в settings
                std::stringstream ss;
                ss << newWidth;
                settings.setWidth(ss.str());
                std::cout << "Slave: Width set to " << newWidth << std::endl;
            }
            if (newHeight != cached.height) {
                cached.height = newHeight;
                std::stringstream ss;
                ss << newHeight;
                settings.setHeight(ss.str());
                std::cout << "Slave: Height set to " << newHeight << std::endl;
            }
            if (newStart != cached.start) {
                cached.start = newStart;
                if (newStart) {
                    settings.startGame();
                    std::cout << "Slave: Game started." << std::endl;
                }
            }
            if (newStop != cached.stop) {
                cached.stop = newStop;
                if (newStop) {
                    settings.stopGame();
                    std::cout << "Slave: Game stopped." << std::endl;
                }
            }
            if (newExit != cached.exitFlag) {
                cached.exitFlag = newExit;
                if (newExit) {
                    settings.exitGame();
                    std::cout << "Slave: Exiting game." << std::endl;
                }
            }


            if (settings.getStart()) {
                std::cout << "Slave: Detected start=1. Proceeding with ship placement." << std::endl;
                break;
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    if (settings.getExit()) {
        std::cout << "Slave: Exiting game before start." << std::endl;
        return 0;
    }

    // Размещение кораблей Slave
    try {
        std::cout << "Slave: Setting up ships..." << std::endl;
        s.setupShips();
    } catch (const std::exception& e) {
        std::cerr << "Slave: Exception during ship setup: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Slave: Ships set up (random) and written to slave_ships.txt." << std::endl;
    std::cout << "Slave: Game started! Waiting for Master's turn..." << std::endl;

    std::string cmd;
    while (true) {

        {
            int currentQueue = manager.readQueueFromFile(QUEUE_FILE);
            if (currentQueue != lastQueueValue) {
                if (currentQueue == 2) {
                    std::cout << "[Slave] Now it's your turn." << std::endl;
                }
                lastQueueValue = currentQueue;
            }
        }

        if (!std::getline(std::cin, cmd)) {
            break;
        }

        std::istringstream iss(cmd);
        std::string command;
        iss >> command;

        if (command == "shot") {
            uint64_t x, y;
            if (iss >> x >> y) {
                if (!settings.getStart()) {
                    std::cout << "Slave: Game not started yet." << std::endl;
                    continue;
                }
                int currentPlayer = manager.readQueueFromFile(QUEUE_FILE);

                if (currentPlayer != 2) {
                    std::cout << "Slave: It's not your turn yet." << std::endl;
                    continue;
                }

                std::string result = s.handleShot(x, y);
                s.processShotResult(result, x, y);
                manager.writeShotToFile(SHOTS_FILE, 2, x, y, result);

                if (result == "miss") {
                    manager.writeQueueToFile(QUEUE_FILE, 1);
                    std::cout << "Slave: Miss at (" << x << ", " << y << ")." << std::endl;

                } else if (result == "hit" || result == "hit_and_sunk") {

                    if (s.allShipsSunk()) {
                        std::cout << "Slave: All Master ships have been sunk. Slave wins!" << std::endl;
                        settings.stopGame();
                    }
                } 
                saveSettingsToFile(SETTINGS_FILE, settings);
                std::cout << "Slave: Shot result saved." << std::endl;
                
            } else {
                std::cout << "Slave: Invalid shot command format. Use: shot <x> <y>" << std::endl;
            }

        } else if (command == "exit") {
            if (settings.exitGame() == "ok") {
                std::cout << "Slave: Exiting game." << std::endl;
                stopFlag = true;
                break;
            } else {
                std::cout << "Slave: Failed to exit game." << std::endl;
            }
        } else {
            std::cout << "Slave: Unknown command." << std::endl;
        }

        saveSettingsToFile(SETTINGS_FILE, settings);

        if (settings.getStop()) {
            std::cout << "Slave: Game is stopped. Type 'exit' to quit or wait for Master to restart." << std::endl;
        }
        if (settings.getExit()) {
            std::cout << "Slave: Exiting game now..." << std::endl;
            break;
        }
    }

    std::cout << "Slave: Game ended." << std::endl;
    return 0;
}
