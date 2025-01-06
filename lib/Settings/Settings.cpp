#include "Settings.h"
#include <stdexcept>
#include <sstream>
#include <regex>
#include <iostream>

Settings::Settings()
    : mode(""), width(0), height(0), start(false), stop(false), exitFlag(false),
      haveLastShot(false), lastShotX(0), lastShotY(0), lastResult("") {}

std::string Settings::setMode(const std::string& cmd) {
    std::lock_guard<std::mutex> lock(settings_mutex);
    if (cmd == "master" || cmd == "slave") {
        mode = cmd;
        return "ok";
    }
    return "failed";
}

std::string Settings::getMode() const {
    std::lock_guard<std::mutex> lock(settings_mutex);
    return mode;
}

std::string Settings::setWidth(const std::string& N) {
    std::lock_guard<std::mutex> lock(settings_mutex);
    std::regex valid_number_regex("^\\d+$");
    if (!std::regex_match(N, valid_number_regex)) {
        return "failed";
    }
    uint64_t value = std::stoull(N);
    width = value;
    return "ok";
}

uint64_t Settings::getWidth() const {
    std::lock_guard<std::mutex> lock(settings_mutex);
    return width;
}

std::string Settings::setHeight(const std::string& N) {
    std::lock_guard<std::mutex> lock(settings_mutex);
    std::regex valid_number_regex("^\\d+$");
    if (!std::regex_match(N, valid_number_regex)) {
        return "failed";
    }
    uint64_t value = std::stoull(N);
    height = value;
    return "ok";
}

uint64_t Settings::getHeight() const {
    std::lock_guard<std::mutex> lock(settings_mutex);
    return height;
}

std::string Settings::setCount(const std::string& cmd) {
    std::lock_guard<std::mutex> lock(settings_mutex);
    std::stringstream ss(cmd);
    int typeShip;
    uint64_t count;
    if (!(ss >> typeShip >> count)) {
        return "failed";
    }
    if (typeShip < 1 || typeShip > 4) {
        return "failed";
    }
    shipCounts[typeShip] = count;
    return "ok";
}

uint64_t Settings::getCount(int typeShip) const {
    std::lock_guard<std::mutex> lock(settings_mutex);
    auto it = shipCounts.find(typeShip);
    if (it != shipCounts.end()) {
        return it->second;
    }
    return 0;
}

const std::map<int, uint64_t>& Settings::getShipCounts() const {
    std::lock_guard<std::mutex> lock(settings_mutex);
    return shipCounts;
}


std::string Settings::startGame() {
    std::lock_guard<std::mutex> lock(settings_mutex);
    std::cout << "Settings::startGame(): Checking conditions..." << std::endl;
    std::cout << "Settings::startGame(): Calling canFitShips()..." << std::endl;

    bool fit = false;
    try {
        fit = canFitShips(); 
    } catch (const std::exception& e) {
        std::cerr << "Settings::startGame(): Exception in canFitShips(): " << e.what() << std::endl;
        return "exception in canFitShips";
    }

    if (start) {
        std::cout << "Settings::startGame(): Game has already been started." << std::endl;
        return "game has already been started";
    }
    if (!fit) {
        std::cout << "Settings::startGame(): Failed to fit all ships on the map." << std::endl;
        return "failed to fit all ships on the map";
    }
    start = true;
    std::cout << "Settings::startGame(): Game started successfully." << std::endl;
    return "ok";
}


bool Settings::getStart() const {
    std::lock_guard<std::mutex> lock(settings_mutex);
    return start;
}

std::string Settings::stopGame() {
    std::lock_guard<std::mutex> lock(settings_mutex);
    if (stop) {
        return "game has already been stopped";
    }
    stop = true;
    return "ok";
}

bool Settings::getStop() const {
    std::lock_guard<std::mutex> lock(settings_mutex);
    return stop;
}

std::string Settings::exitGame() {
    std::lock_guard<std::mutex> lock(settings_mutex);
    if (exitFlag) {
        return "game has already been exited";
    }
    exitFlag = true;
    return "ok";
}

bool Settings::getExit() const {
    std::lock_guard<std::mutex> lock(settings_mutex);
    return exitFlag;
}

// Выстрелы
void Settings::setLastShot(uint64_t x, uint64_t y) {
    std::lock_guard<std::mutex> lock(settings_mutex);
    lastShotX = x;
    lastShotY = y;
    haveLastShot = true;
}

bool Settings::hasLastShot() const {
    std::lock_guard<std::mutex> lock(settings_mutex);
    return haveLastShot;
}

std::pair<uint64_t, uint64_t> Settings::getLastShot() const {
    std::lock_guard<std::mutex> lock(settings_mutex);
    return {lastShotX, lastShotY};
}

void Settings::clearLastShot() {
    std::lock_guard<std::mutex> lock(settings_mutex);
    haveLastShot = false;
}

void Settings::setLastResult(const std::string& res) {
    std::lock_guard<std::mutex> lock(settings_mutex);
    lastResult = res;
}

std::string Settings::getLastResult() const {
    std::lock_guard<std::mutex> lock(settings_mutex);
    return lastResult;
}

// Корабли
void Settings::addMasterShip(const Ship& ship) {
    std::lock_guard<std::mutex> lock(settings_mutex);
    masterShips.push_back(ship);
}

const std::vector<Ship>& Settings::getMasterShips() const {
    std::lock_guard<std::mutex> lock(settings_mutex);
    return masterShips;
}

std::vector<Ship>& Settings::getMasterShipsMutable() {
    std::lock_guard<std::mutex> lock(settings_mutex);
    return masterShips;
}

bool Settings::allMasterShipsSunk() const {
    std::lock_guard<std::mutex> lock(settings_mutex);
    for (const auto& ship : masterShips) {
        if (!ship.isSunk()) return false;
    }
    return true;
}

void Settings::clearMasterShips() {
    std::lock_guard<std::mutex> lock(settings_mutex);
    masterShips.clear();
}

void Settings::addSlaveShip(const Ship& ship) {
    std::lock_guard<std::mutex> lock(settings_mutex);
    slaveShips.push_back(ship);
}

const std::vector<Ship>& Settings::getSlaveShips() const {
    std::lock_guard<std::mutex> lock(settings_mutex);
    return slaveShips;
}

std::vector<Ship>& Settings::getSlaveShipsMutable() {
    std::lock_guard<std::mutex> lock(settings_mutex);
    return slaveShips;
}

bool Settings::allSlaveShipsSunk() const {
    std::lock_guard<std::mutex> lock(settings_mutex);
    for (const auto& ship : slaveShips) {
        if (!ship.isSunk()) return false;
    }
    return true;
}

void Settings::clearSlaveShips() {
    std::lock_guard<std::mutex> lock(settings_mutex);
    slaveShips.clear();
}

bool Settings::canFitShips() const {
    uint64_t totalShipCells = 0;
    for (const auto& [type, count] : shipCounts) {
        totalShipCells += type * count;
    }
    std::cout << "Settings::canFitShips(): totalShipCells=" << totalShipCells
              << " mapSize=" << (width*height) << std::endl;
    if (totalShipCells > width * height) {
        std::cout << "Settings::canFitShips(): Not enough space." << std::endl;
        return false;
    }
    std::cout << "Settings::canFitShips(): Enough space." << std::endl;
    return true;
}

void Settings::clearShips() {
    std::lock_guard<std::mutex> lock(settings_mutex);
    masterShips.clear();
    slaveShips.clear();
}

void Settings::clearAllShots() {
    std::lock_guard<std::mutex> lock(settings_mutex);
    for (auto& ship : masterShips) {
        std::fill(ship.hits.begin(), ship.hits.end(), false);
    }
    for (auto& ship : slaveShips) {
        std::fill(ship.hits.begin(), ship.hits.end(), false);
    }
}
