#pragma once
#include <string>
#include "Settings/Settings.h"
#include "ReadWriteQueue/ReadWriteQueue.h"
#include "Slave/Slave.h"

class SlaveCommandHandler {
public:
    SlaveCommandHandler(Settings& settings,
                        ReadWriteQueue& manager,
                        Slave& slaveLogic);

    void handleGameStartIfNeeded();
    bool handleCommand(const std::string& input);

private:
    Settings& settings_;
    ReadWriteQueue& manager_;
    Slave& slaveLogic_;
    bool shipsPlaced_ = false;

    void handleShotCommand(std::istringstream& iss);
};
