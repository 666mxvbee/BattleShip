#include "PrintUtils.h"
#include <iostream>

void printSettings(const Settings& settings) {
    std::cout << "===== Current Settings =====\n"
              << "Mode: " << settings.getMode() << "\n"
              << "Width: " << settings.getWidth() << "\n"
              << "Height: " << settings.getHeight() << "\n"
              << "Ship Counts:\n";
    for (auto& [type, count] : settings.getShipCounts()) {
        std::cout << "  - Type " << type << " => " << count << "\n";
    }
    std::cout << "Start: " << (settings.getStart() ? "Yes" : "No") << "\n"
              << "Stop: " << (settings.getStop() ? "Yes" : "No") << "\n"
              << "Exit: " << (settings.getExit() ? "Yes" : "No") << "\n"
              << "============================\n";
}

void printShotResult(const std::string& result, uint64_t x, uint64_t y, bool isMaster) {
    std::string player = isMaster ? "Master" : "Slave";
    if (result == "miss") {
        std::cout << player << ": Miss at (" << x << ", " << y << ").\n";
    } else if (result == "hit") {
        std::cout << player << ": Hit at (" << x << ", " << y << ").\n";
    } else if (result == "hit_and_sunk") {
        std::cout << player << ": Hit and sunk at (" << x << ", " << y << ").\n";
    } else {
        std::cout << player << ": Unknown shot result '" << result << "' at (" << x << ", " << y << ").\n";
    }
}
