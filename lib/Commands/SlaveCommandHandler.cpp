#include "SlaveCommandHandler.h"
#include <iostream>
#include <fstream>
#include <sstream>

#include "SlaveCommandHandler.h"

#include "Utils/FileUtils.h"
#include "Utils/PrintUtils.h"

SlaveCommandHandler::SlaveCommandHandler(Settings& settings,
                                         ReadWriteQueue& manager,
                                         Slave& slaveLogic)
    : settings_(settings), manager_(manager), slaveLogic_(slaveLogic)
{}

void SlaveCommandHandler::handleGameStartIfNeeded() {
    std::ifstream in("../game_data/settings.txt");
    if (!in.is_open()) {
        return;
    }

    bool newStart = false;
    bool newExit = false;

    std::string line;
    while (std::getline(in, line)) {
        auto pos = line.find('=');
        if (pos == std::string::npos) continue;

        std::string key = line.substr(0, pos);
        std::string val = line.substr(pos+1);

        if (key == "mode") {
            settings_.setMode(val);
        }
        else if (key == "width") {
            settings_.setWidth(val);
        }
        else if (key == "height") {
            settings_.setHeight(val);
        }
        else if (key == "count") {
            settings_.setCount(val);
        }
        else if (key == "start") {
            newStart = (val == "1");
        }
        else if (key == "exit") {
            newExit = (val == "1");
        }
    }
    in.close();

    if (newExit) {
        settings_.exitGame();
        std::cout << "Slave: Detected exit=1 in settings. Exiting.\n";
        return;
    }

    if (newStart && !shipsPlaced_) {
        settings_.startGame();
        std::cout << "Slave: start=1 detected. Placing ships...\n";

        try {
            slaveLogic_.setupShips();
            shipsPlaced_ = true;
            std::cout << "Slave: Ships placed (slave_ships.txt).\n";
        }
        catch (const std::exception& e) {
            std::cerr << "Slave: Exception while placing ships: " << e.what() << "\n";
        }
    }
}

bool SlaveCommandHandler::handleCommand(const std::string& input) {
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
                std::cout << "Slave: Detected exit=1 from Master.\n";
            }
        }
    }

    if (settings_.getExit()) {
        std::cout << "Slave: Exiting (because exit=1 in settings).\n";
        return true;
    }

    std::istringstream iss(input);
    std::string command;
    iss >> command;

    if (command == "shot") {
        handleShotCommand(iss);
    }
    else if (command == "exit") {
        settings_.exitGame();
        std::cout << "Slave: Exiting game.\n";
        return true;
    }
    else if (command == "print") {
        std::string sub;
        iss >> sub;
        if (sub == "settings") {
            printSettings(settings_);
        } else {
            std::cout << "Slave: Unknown print subcommand.\n";
        }
    }
    else {
        std::cout << "Slave: Unknown command.\n";
    }

    return false;
}

void SlaveCommandHandler::handleShotCommand(std::istringstream& iss) {
    uint64_t x, y;
    if (!(iss >> x >> y)) {
        std::cout << "Slave: Invalid shot command. Use 'shot X Y'.\n";
        return;
    }
    if (!settings_.getStart()) {
        std::cout << "Slave: Game not started yet.\n";
        return;
    }
    int currentQueue = manager_.readQueueFromFile("../game_data/queue.txt");
    if (currentQueue != 2) {
        std::cout << "Slave: Not your turn.\n";
        return;
    }

    std::string result = slaveLogic_.handleShot(x, y);
    slaveLogic_.processShotResult(result, x, y);
    manager_.writeShotToFile("../game_data/shots.txt", 2, x, y, result);

    if (result == "miss") {
        manager_.writeQueueToFile("../game_data/queue.txt", 1);
    }
    else if (result == "hit" || result == "hit_and_sunk") {
        if (slaveLogic_.allShipsSunk()) {
            std::cout << "Slave: All Master ships are sunk. Slave wins!\n";
            settings_.exitGame();
            saveSettingsToFile("../game_data/settings.txt", settings_);
        }
    }

    saveSettingsToFile("../game_data/settings.txt", settings_);
}
