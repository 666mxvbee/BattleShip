#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <chrono>

#include "MasterCommandHandler.h"

#include "Utils/FileUtils.h"
#include "Utils/PrintUtils.h"

MasterCommandHandler::MasterCommandHandler(Settings& settings,
                                           ReadWriteQueue& manager,
                                           Master& masterLogic)
    : settings_(settings), manager_(manager), masterLogic_(masterLogic)
{
}

bool MasterCommandHandler::handleCommand(const std::string& input) {
    {
        std::ifstream in("../game_data/settings.txt");
        if (in.is_open()) {
            bool newExit = false;
            std::string line;
            while (std::getline(in, line)) {
                auto pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = line.substr(0, pos);
                    std::string val = line.substr(pos + 1);
                    if (key == "exit" && val == "1") {
                        newExit = true;
                    }
                }
            }
            if (newExit) {
                settings_.exitGame();
                std::cout << "Master: Detected exit=1 from Slave.\n";
            }
        }
    }

    if (settings_.getExit()) {
        std::cout << "Master: Exiting (because exit=1 in settings).\n";
        return true;
    }

    std::istringstream iss(input);
    std::string command;
    iss >> command;

    if (command == "set") {
        handleSetCommand(iss);
    }
    else if (command == "start") {
        handleStartCommand();
    }
    else if (command == "stop") {
        handleStopCommand();
    }
    else if (command == "exit") {
        settings_.exitGame();
        std::cout << "Master: Exiting game.\n";
        return true;
    }
    else if (command == "shot") {
        handleShotCommand(iss);
    }
    else if (command == "print") {
        std::string sub;
        iss >> sub;
        if (sub == "settings") {
            printSettings(settings_);
        } else {
            std::cout << "Master: Unknown print subcommand.\n";
        }
    }
    else {
        std::cout << "Master: Unknown command.\n";
    }

    return false;
}

void MasterCommandHandler::handleSetCommand(std::istringstream& iss) {
    std::string sub;
    iss >> sub;
    if (sub == "mode") {
        std::string mode;
        iss >> mode;
        std::string res = settings_.setMode(mode);
        std::cout << "Master: set mode -> " << res << "\n";
    }
    else if (sub == "width") {
        std::string w;
        iss >> w;
        std::string res = settings_.setWidth(w);
        std::cout << "Master: set width -> " << res << "\n";
    }
    else if (sub == "height") {
        std::string h;
        iss >> h;
        std::string res = settings_.setHeight(h);
        std::cout << "Master: set height -> " << res << "\n";
    }
    else if (sub == "count") {
        int typeShip;
        uint64_t countShip;
        if (iss >> typeShip >> countShip) {
            std::stringstream ss;
            ss << typeShip << " " << countShip;
            std::string res = settings_.setCount(ss.str());
            std::cout << "Master: set count -> " << res << "\n";
        } else {
            std::cout << "Master: Invalid syntax for 'set count'.\n";
        }
    }
    else {
        std::cout << "Master: Unknown subcommand for set: " << sub << "\n";
    }

    saveSettingsToFile("../game_data/settings.txt", settings_);
}

void MasterCommandHandler::handleStartCommand() {
    std::cout << "Master: 'start' command received.\n";

    if (settings_.getMode() != "master") {
        std::cout << "Master: Warning: mode is not 'master'. Attempting to set...\n";
        settings_.setMode("master");
    }
    if (settings_.getWidth() == 0 || settings_.getHeight() == 0) {
        std::cout << "Master: Invalid width/height. Set them before start.\n";
        return;
    }
    if (settings_.getShipCounts().empty()) {
        std::cout << "Master: No ships specified. Use 'set count <type> <count>'.\n";
        return;
    }

    std::string res = settings_.startGame();
    if (res != "ok") {
        std::cout << "Master: Failed to start game. Reason: " << res << "\n";
        return;
    }
    saveSettingsToFile("../game_data/settings.txt", settings_);

    try {
        masterLogic_.setupShips();
        std::cout << "Master: Ships placed (see master_ships.txt).\n";
        saveSettingsToFile("../game_data/settings.txt", settings_);

        bool slaveReady = false;
        while (!slaveReady && !settings_.getExit()) {
            std::ifstream slaveFile("../game_data/slave_ships.txt");
            if (slaveFile.is_open()) {
                slaveReady = true;
                slaveFile.close();
            } else {
                std::cout << "Master: Waiting for Slave to place ships...\n";
                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }

        if (!settings_.getExit()) {
            masterLogic_.loadSlaveShips("../game_data/slave_ships.txt");
            std::cout << "Master: Slave ships loaded. Game begins!\n";
            std::cout << "Master: It's your turn. Use 'shot x y'.\n";
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Master: Exception during start: " << e.what() << "\n";
    }
}

void MasterCommandHandler::handleStopCommand() {
    std::string res = settings_.stopGame();
    if (res == "ok") {
        std::cout << "Master: Game stopped.\n";
    } else {
        std::cout << "Master: " << res << "\n";
    }
    saveSettingsToFile("../game_data/settings.txt", settings_);
}

void MasterCommandHandler::handleShotCommand(std::istringstream& iss) {
    uint64_t x, y;
    if (!(iss >> x >> y)) {
        std::cout << "Master: Invalid shot command. Use 'shot X Y'.\n";
        return;
    }
    if (!settings_.getStart()) {
        std::cout << "Master: Game not started yet.\n";
        return;
    }

    int currentQueue = manager_.readQueueFromFile("../game_data/queue.txt");
    if (currentQueue != 1) {
        std::cout << "Master: It's not your turn.\n";
        return;
    }

    std::string result = masterLogic_.handleShot(x, y);
    masterLogic_.processShotResult(result, x, y);
    manager_.writeShotToFile("../game_data/shots.txt", 1, x, y, result);

    if (result == "miss") {
        manager_.writeQueueToFile("../game_data/queue.txt", 2);
    }
    else if (result == "hit" || result == "hit_and_sunk") {
        if (masterLogic_.allShipsSunk()) {
            std::cout << "Master: All Slave ships have been sunk. Master wins!\n";
            settings_.exitGame();
            saveSettingsToFile("../game_data/queue.txt", settings_);
        }
    }

    saveSettingsToFile("../game_data/settings.txt", settings_);
}