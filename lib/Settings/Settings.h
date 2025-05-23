#pragma once
#include "Ship/Ship.h"
#include <string>
#include <map>
#include <vector>
#include <mutex>

class Settings {
private:
    mutable std::mutex settings_mutex;
    std::string mode;
    uint64_t width;
    uint64_t height;
    bool start;
    bool stop;
    bool exitFlag;

    std::map<int, uint64_t> shipCounts;

    std::vector<Ship> masterShips;
    std::vector<Ship> slaveShips;

    bool haveLastShot;
    uint64_t lastShotX;
    uint64_t lastShotY;
    std::string lastResult;

public:
    Settings();

    std::string setMode(const std::string& cmd);
    std::string getMode() const;

    std::string setWidth(const std::string& N);
    uint64_t getWidth() const;
    std::string setHeight(const std::string& N);
    uint64_t getHeight() const;

    std::string setCount(const std::string& cmd);
    uint64_t getCount(int typeShip) const;
    const std::map<int, uint64_t>& getShipCounts() const;

    std::string startGame();
    bool getStart() const;
    std::string stopGame();
    bool getStop() const;
    std::string exitGame();
    bool getExit() const;

    void setLastShot(uint64_t x, uint64_t y);
    bool hasLastShot() const;
    std::pair<uint64_t, uint64_t> getLastShot() const;
    void clearLastShot();
    void setLastResult(const std::string& res);
    std::string getLastResult() const;

    void addMasterShip(const Ship& ship);
    const std::vector<Ship>& getMasterShips() const;
    std::vector<Ship>& getMasterShipsMutable();
    bool allMasterShipsSunk() const;
    void clearMasterShips();

    void addSlaveShip(const Ship& ship);
    const std::vector<Ship>& getSlaveShips() const;
    std::vector<Ship>& getSlaveShipsMutable();
    bool allSlaveShipsSunk() const;
    void clearSlaveShips();

    bool canFitShips() const;
    void clearShips();
    void clearAllShots();
};
