#pragma once
#include <string>
#include "Settings/Settings.h"
#include "ReadWriteQueue/ReadWriteQueue.h"
#include "Master/Master.h"

class MasterCommandHandler {
public:
    MasterCommandHandler(Settings& settings,
                         ReadWriteQueue& manager,
                         Master& masterLogic);

    bool handleCommand(const std::string& input);

private:
    Settings& settings_;
    ReadWriteQueue& manager_;
    Master& masterLogic_;

    void handleSetCommand(std::istringstream& iss);
    void handleStartCommand();
    void handleStopCommand();
    void handleShotCommand(std::istringstream& iss);
};
