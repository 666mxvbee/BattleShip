#include "RandomShips.h"
#include "Ship/Ship.h"
#include <iostream>

static bool isOverLaps(const Ship& existingShip, int newLen, char newOrient, uint64_t newX, uint64_t newY) {
    for (int j = 0; j < newLen; ++j) {
        uint64_t cellX = (newOrient == 'h') ? (newX + j) : newX;
        uint64_t cellY = (newOrient == 'v') ? (newY + j) : newY;

        for (int k = 0; k < existingShip.len; ++k) {
            uint64_t exX = (existingShip.orientation == 'h') ? (existingShip.x + k) : existingShip.x;
            uint64_t exY = (existingShip.orientation == 'v') ? (existingShip.y + k) : existingShip.y;
            
            if (std::abs((long long)cellX - (long long)exX) <= 1 &&
                std::abs((long long)cellY - (long long)exY) <= 1) {
                    return true;
                }
        }
    }

    return false;
}

RandomShips::RandomShips(Settings& s)
    : settings(s), width(s.getWidth()), height(s.getHeight()), rng(std::random_device{}()) {}

void RandomShips::placeShips(const std::string& player) {
    std::map<int, uint64_t> shipCounts = settings.getShipCounts();
    if (shipCounts.empty()) {
        throw std::runtime_error("RandomShips::placeShips(): Ship counts are empty.");
    }

    for (const auto& [type, count] : shipCounts) {
        for (uint64_t i = 0; i < count; ++i) {
            bool placed = false;
            int attempts = 0;
            const int MAX_ATTEMPTS = 2000;

            std::uniform_int_distribution<int> orientation_dist(0, 1);

            while (!placed && attempts < MAX_ATTEMPTS) {
                char orientation = (orientation_dist(rng) == 0) ? 'h' : 'v';

                std::uniform_int_distribution<uint64_t> x_dist(0, (orientation == 'h') ? (width - type) : (width - 1));
                std::uniform_int_distribution<uint64_t> y_dist(0, (orientation == 'v') ? (height - type) : (height - 1));

                uint64_t x = x_dist(rng);
                uint64_t y = y_dist(rng);

                std::vector<Ship> existingShips;
                if (player == "master") {
                    existingShips = settings.getMasterShips();
                }
                else {
                    existingShips = settings.getSlaveShips();
                }

                bool badPosition = false;
                for (const auto& ship : existingShips) {
                    if (isOverLaps(ship, type, orientation, x, y)) {
                        badPosition = true;
                        break;
                    }
                }

                if (!badPosition) {
                    Ship newShip(type, orientation, x, y);
                    if (player == "master") {
                        settings.addMasterShip(newShip);
                        std::cout << "Master: Placed ship type " << type
                                  << " at (" << x << ", " << y
                                  << ") orientation " << orientation << std::endl;
                    }
                    else {
                        settings.addSlaveShip(newShip);
                        std::cout << "Slave: Place ship type " << type
                                  << " at (" << x << ", " << y
                                  << ") orientation " << orientation << std::endl;
                    }
                    placed = true;
                }

                attempts++;
            }

            if (!placed) {
                std::cerr << "RandomShips: Failed to place ship type " << type
                          << " after " << MAX_ATTEMPTS << " attempts."
                          << " Possibly the map is too small." << std::endl;
                throw std::runtime_error("RandomShips::placeShips(): Failed to place all ships.");
            }
        }
    }
}