#include "Master.h"
#include "../RandomShips/RandomShips.h"
#include <fstream>
#include <iostream>

Master::Master(Settings& s) : settings(s) {}

std::string Master::handleShot(uint64_t x, uint64_t y) {
    auto& ships = settings.getSlaveShipsMutable();
    for (auto& ship : ships) {
        if (ship.contains(x, y)) {
            if (ship.hitAt(x ,y)) {
                if (ship.isSunk()) {
                    return "hit_and_sunk";
                }
                return "hit";
            }
        }
    }
    return "miss";
}

void Master::processShotResult(const std::string& result, uint64_t x, uint64_t y) {
    if (result == "hit") {
        std::cout << "Master: Hit at (" << x << ", " << y << ")!" << std::endl;
    }
    else if (result == "hit_and_sunk") {
        std::cout << "Master: Hit and sunk a ship at (" << x << ", " << y << ")!" << std::endl;
    }
    else if (result == "miss") {
        std::cout << "Master: Miss at (" << x << ", " << y << ")." << std::endl;
    }
}

void Master::setupShips() {
    // Размещаем корабли Master случайным образом
    RandomShips randomShips(settings);
    try {
        randomShips.placeShips("master");
    }
    catch (const std::exception& e) {
        std::cerr << "Master::setupShips(): Exception during ship placement: " << e.what() << std::endl;
        throw; // Re-throw для обработки в main
    }

    // Сохраняем корабли в файл master_ships.txt
    std::ofstream masterFile("../game_data/master_ships.txt", std::ios::trunc);
    if (!masterFile.is_open()) {
        std::cerr << "Master: Unable to open master_ships.txt for writing." << std::endl;
        throw std::runtime_error("Master::setupShips(): Unable to open master_ships.txt for writing.");
    }
    const auto& ships = settings.getMasterShips();
    for (const auto& ship : ships) {
        masterFile << "ship_type=" << ship.len
                   << ",orientation=" << ship.orientation
                   << ",x=" << ship.x
                   << ",y=" << ship.y << "\n";
    }
    masterFile.close();
    std::cout << "Master: Ships written to master_ships.txt" << std::endl;

}


bool Master::loadSlaveShips(const std::string& filename) {
    std::ifstream in(filename);
    if (!in.is_open()) {
        std::cerr << "Master: Unable to open " << filename << " for reading." << std::endl;
        return false;
    }

    std::string line;

    // Очистка текущего списка кораблей Slave
    settings.clearSlaveShips();

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
        settings.addSlaveShip(ship);

    }

    in.close();
    return true;
}

bool Master::allShipsSunk() const {
    return settings.allSlaveShipsSunk();
}
