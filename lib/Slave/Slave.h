#pragma once
#include "../Settings/Settings.h"
#include <string>
#include <sstream>

class Slave {
private:
    Settings& settings;

public:
    Slave(Settings& s);

    void setupShips();

    std::string handleShot(uint64_t x, uint64_t y);

    void processShotResult(const std::string& result, uint64_t x, uint64_t y);

    bool loadMasterShips(const std::string& filename);

    bool allShipsSunk() const;

};
