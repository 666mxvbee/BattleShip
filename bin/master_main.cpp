#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>
#include <sstream>
#include <mutex>
#include <filesystem>
#include "../lib/ReadWriteQueue/ReadWriteQueue.h"
#include "../lib/Settings/Settings.h"
#include "../lib/Master/Master.h"

namespace fs = std::filesystem;

const std::string SETTINGS_FILE = "../game_data/settings.txt";
const std::string MASTER_SHIPS_FILE = "../game_data/master_ships.txt";
const std::string SLAVE_SHIPS_FILE = "../game_data/slave_ships.txt";
const std::string QUEUE_FILE = "../game_data/queue.txt";
const std::string SHOTS_FILE = "../game_data/shots.txt";

bool stopFlag = false;

void saveSettingsToFile(const std::string& filename, const Settings& settings) {
    static std::mutex file_mutex;
    std::lock_guard<std::mutex> lock(file_mutex);

    std::ofstream out(filename, std::ios::trunc);
    if (!out.is_open()) {
        std::cerr << "Master: Unable to open " << filename << " for writing." << std::endl;
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
    // std::cout << "Master: Settings saved to " << filename << std::endl;
}

void printSettings(const Settings& settings) {
    std::cout << "Current Settings:\n";
    std::cout << "Mode: " << settings.getMode() << "\n";
    std::cout << "Width: " << settings.getWidth() << "\n";
    std::cout << "Height: " << settings.getHeight() << "\n";
    std::cout << "Ship Counts:\n";
    for (const auto& [type, count] : settings.getShipCounts()) {
        std::cout << "  Type " << type << ": " << count << "\n";
    }
    std::cout << "Start: " << (settings.getStart() ? "Yes" : "No") << "\n";
    std::cout << "Stop: " << (settings.getStop() ? "Yes" : "No") << "\n";
    std::cout << "Exit: " << (settings.getExit() ? "Yes" : "No") << "\n";
}

int main() {
    Settings settings;
    ReadWriteQueue manager;
    Master m(settings);

    std::cout << "Master: Initializing game settings..." << std::endl;

    try {
        fs::create_directories("../game_data");
        std::cout << "Master: Ensured that game_data directory exists." << std::endl;
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Master: Failed to create game_data directory: " << e.what() << std::endl;
        return 1;
    }

    {
        std::ofstream settingsFile(SETTINGS_FILE, std::ios::trunc);
        if (!settingsFile.is_open()) {
            std::cerr << "Master: Unable to open " << SETTINGS_FILE << " for writing." << std::endl;
            return 1;
        }
        settingsFile.close();

        std::ofstream shotsFile(SHOTS_FILE, std::ios::trunc);
        if (!shotsFile.is_open()) {
            std::cerr << "Master: Unable to open " << SHOTS_FILE << " for writing." << std::endl;
            return 1;
        }
        shotsFile.close();

        std::ofstream queueFile(QUEUE_FILE, std::ios::trunc);
        if (!queueFile.is_open()) {
            std::cerr << "Master: Unable to open " << QUEUE_FILE << " for writing." << std::endl;
            return 1;
        }
        queueFile << "1"; // Master ходит первым
        queueFile.close();
        std::cout << "Master: Initialized queue.txt with Master's turn." << std::endl;
    }

    int lastQueueValue = -1;

    std::string cmd;
    std::cout << "Master: Enter commands (e.g. set mode master, set width 10, set height 10, set count 4 1, start):" << std::endl;
    while (true) {

        {
            int currentQueue = manager.readQueueFromFile(QUEUE_FILE);
            if (currentQueue != lastQueueValue) {
                if (currentQueue == 1) {
                    std::cout << "[Master] Now it's your turn" << std::endl;
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

        if (command == "set") {
            std::string sub;
            iss >> sub;
            if (sub == "mode") {
                std::string mode;
                iss >> mode;
                if (settings.setMode(mode) == "ok") {
                    std::cout << "Master: Mode set to " << mode << std::endl;
                } else {
                    std::cout << "Master: Failed to set mode." << std::endl;
                }
            } else if (sub == "width") {
                std::string w;
                iss >> w;
                if (settings.setWidth(w) == "ok") {
                    std::cout << "Master: Width set to " << w << std::endl;
                } else {
                    std::cout << "Master: Failed to set width." << std::endl;
                }
            } else if (sub == "height") {
                std::string h;
                iss >> h;
                if (settings.setHeight(h) == "ok") {
                    std::cout << "Master: Height set to " << h << std::endl;
                } else {
                    std::cout << "Master: Failed to set height." << std::endl;
                }
            } else if (sub == "count") {
                int typeShip;
                uint64_t c;
                if (iss >> typeShip >> c) {
                    std::stringstream ss;
                    ss << typeShip << " " << c;
                    if (settings.setCount(ss.str()) == "ok") {
                        std::cout << "Master: Ship count set: Type " << typeShip << ", Count " << c << std::endl;
                    } else {
                        std::cout << "Master: Failed to set ship count." << std::endl;
                    }
                } else {
                    std::cout << "Master: Invalid format for set count. Use: set count <typeShip> <count>" << std::endl;
                }
            } else {
                std::cout << "Master: Unknown subcommand for set: " << sub << std::endl;
            }
            printSettings(settings);

        } else if (command == "start") {
            std::cout << "Master: 'start' command received. Attempting to start the game." << std::endl;
            std::string res;
            try {
                res = settings.startGame(); 
            } catch (const std::exception& e) {
                std::cerr << "Master: Exception calling startGame(): " << e.what() << std::endl;
                continue;
            }

            std::cout << "Master: startGame() returned: " << res << std::endl;

            if (res == "ok") {
                std::cout << "Master: Game started successfully. Saving settings..." << std::endl;
                saveSettingsToFile(SETTINGS_FILE, settings);

                try {
                    std::cout << "Master: Setting up ships..." << std::endl;
                    m.setupShips();
                    std::cout << "Master: Ships setup completed. Master ships saved to master_ships.txt" << std::endl;

                    saveSettingsToFile(SETTINGS_FILE, settings);
                    std::cout << "Master: Settings saved after ship setup." << std::endl;

                    std::cout << "Master: Waiting for Slave to setup ships..." << std::endl;
                    bool slaveReady = false;
                    while (!slaveReady && !settings.getExit()) {
                        std::ifstream slaveFile(SLAVE_SHIPS_FILE);
                        if (slaveFile.is_open()) {
                            std::cout << "Master: Detected slave_ships.txt. Slave has set up ships." << std::endl;
                            slaveReady = true;
                            slaveFile.close();
                        } else {
                            std::cout << "Master: Slave not ready yet. Waiting..." << std::endl;
                            std::this_thread::sleep_for(std::chrono::seconds(1));
                        }
                    }

                    if (!settings.getExit()) {
                        std::cout << "Master:: Loading Slave ships now..." << std::endl;
                        m.loadSlaveShips(SLAVE_SHIPS_FILE);

                        std::cout << "Master: Game begins now!" << std::endl;
                        std::cout << "Master: It's your turn. Use 'shot x y' to shoot." << std::endl;
                    } else {
                        std::cout << "Master: Exiting game." << std::endl;
                    }

                } catch (const std::exception& e) {
                    std::cerr << "Master: Exception during start: " << e.what() << std::endl;
                }

            } else {
                // Если startGame() вернуло не "ok", вывести причину
                std::cout << "Master: Failed to start game. Reason: " << res << std::endl;
            }

        } else if (command == "stop") {
            if (settings.stopGame() == "ok") {
                std::cout << "Master: Game stopped." << std::endl;
                saveSettingsToFile(SETTINGS_FILE, settings);
            } else {
                std::cout << "Master: Failed to stop game." << std::endl;
            }

        } else if (command == "exit") {
            if (settings.exitGame() == "ok") {
                std::cout << "Master: Exiting game." << std::endl;
                stopFlag = true;
                break;
            } else {
                std::cout << "Master: Failed to exit game." << std::endl;
            }

        } else if (command == "shot") {
            uint64_t x,y;
            if (iss >> x >> y) {
                if (!settings.getStart()) {
                    std::cout << "Master: Game not started yet. Use 'start' first." << std::endl;
                    continue;
                }
                int currentPlayer = manager.readQueueFromFile(QUEUE_FILE);
                if (currentPlayer != 1) {
                    std::cout << "Master: It's not your turn." << std::endl;
                    continue;
                }
                
                std::string result = m.handleShot(x, y);
                m.processShotResult(result, x, y);
                manager.writeShotToFile(SHOTS_FILE, 1, x, y, result);

                if (result == "miss") {
                    manager.writeQueueToFile(QUEUE_FILE, 2);
                    std::cout << "Master: Miss at (" << x << ", " << y << ")." << std::endl;
                    

                } else if (result == "hit" || result == "hit_and_sunk"){

                    if (m.allShipsSunk()) {
                        std::cout << "Master: All Slave ships have been sunk. Master wins!" << std::endl;
                        settings.stopGame();
                    }
                }
                saveSettingsToFile(SETTINGS_FILE, settings);
                std::cout << "Master: Shot result saved." << std::endl;

            } else {
                std::cout << "Master: Invalid shot command. Use: shot <x> <y>" << std::endl;
            }
        } else {
            std::cout << "Master: Unknown command." << std::endl;
        }

        saveSettingsToFile(SETTINGS_FILE, settings);

        if (settings.getStop()) {
            std::cout << "Master: Game is stopped. Type 'exit' to quit or 'start' to restart." << std::endl;
        }

        if (settings.getExit()) {
            std::cout << "Master: Exiting game now..." << std::endl;
            break;
        }
    }

    std::cout << "Master: Game ended." << std::endl;
    return 0;
}
