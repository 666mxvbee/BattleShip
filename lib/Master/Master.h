#pragma once
#include "Settings/Settings.h"
#include <string>
#include <sstream>

class Master {
private:
    Settings& settings;

public:
    explicit Master(Settings& s);

    void setupShips();

    std::string handleShot(uint64_t x, uint64_t y);

    void processShotResult(const std::string& result, uint64_t x, uint64_t y);

    bool loadSlaveShips(const std::string& filename);

    bool allShipsSunk() const;

};
