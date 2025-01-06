#include "Slave.h"
#include "../RandomShips/RandomShips.h"
#include <fstream>
#include <iostream>

Slave::Slave(Settings& s) : settings(s) {}

std::string Slave::handleShot(uint64_t x, uint64_t y) {
    auto& ships = settings.getMasterShipsMutable();
    for (auto& ship : ships) {
        if (ship.contains(x, y)) {
            if (ship.hitAt(x, y)) {
                if (ship.isSunk()) {
                    return "hit_and_sunk";
                }
                return "hit";
            }
        }
    }
    return "miss";
}

void Slave::processShotResult(const std::string& result, uint64_t x, uint64_t y) {
    if (result == "hit") {
        std::cout << "Slave: Hit at (" << x << ", " << y << ")!" << std::endl;
    }
    else if (result == "hit_and_sunk") {
        std::cout << "Slave: Hit and sunk a ship at (" << x << ", " << y << ")!" << std::endl;
    }
    else if (result == "miss") {
        std::cout << "Slave: Miss at (" << x << ", " << y << ")." << std::endl;
    }
}

void Slave::setupShips() {
    // Размещаем корабли Slave случайным образом
    RandomShips randomShips(settings);
    try {
        randomShips.placeShips("slave");
    }
    catch (const std::exception& e) {
        std::cerr << "Slave::setupShips(): Exception during ship placement: " << e.what() << std::endl;
        throw; // Re-throw для обработки в main
    }

    // Сохраняем корабли в файл slave_ships.txt
    std::ofstream slaveFile("../game_data/slave_ships.txt", std::ios::trunc);
    if (!slaveFile.is_open()) {
        std::cerr << "Slave: Unable to open slave_ships.txt for writing." << std::endl;
        throw std::runtime_error("Slave::setupShips(): Unable to open slave_ships.txt for writing.");
    }
    const auto& ships = settings.getSlaveShips();
    for (const auto& ship : ships) {
        slaveFile << "ship_type=" << ship.len
                  << ",orientation=" << ship.orientation
                  << ",x=" << ship.x
                  << ",y=" << ship.y << "\n";
    }
    slaveFile.close();
    std::cout << "Slave: Ships written to slave_ships.txt" << std::endl;

    if (loadMasterShips("../game_data/master_ships.txt")) {

        std::cout << "Slave: Master ships loaded (hidden from your view)." << std::endl;

    }
    else {
        std::cerr << "Slave: Failed to load Master ships from master_ships.txt" << std::endl;
    }
}

bool Slave::loadMasterShips(const std::string& filename) {
    std::ifstream in(filename);
    if (!in.is_open()) {
        std::cerr << "Slave: Unable to open " << filename << " for reading." << std::endl;
        return false;
    }

    std::string line;
    settings.clearMasterShips();

    while (std::getline(in, line)) {
        std::stringstream ss(line);
        std::string token;

        int ship_type = 0;
        char orientation = 'h';
        uint64_t x = 0, y = 0;

        while (std::getline(ss, token, ',')) {
            std::size_t pos = token.find('=');
            if (pos == std::string::npos) continue;
            std::string key = token.substr(0, pos);
            std::string value = token.substr(pos + 1);

            if (key == "ship_type") {
                ship_type = std::stoi(value);
            } else if (key == "orientation") {
                orientation = value[0];
            } else if (key == "x") {
                x = std::stoull(value);
            } else if (key == "y") {
                y = std::stoull(value);
            }
        }

        Ship ship(ship_type, orientation, x, y);
        settings.addMasterShip(ship);
    }

    in.close();
    return true;
}

bool Slave::allShipsSunk() const {
    return settings.allMasterShipsSunk();
}
