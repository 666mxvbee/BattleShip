#include <iostream>
#include <filesystem>
#include <thread>
#include <chrono>
#include <fstream>

#include "../lib/Commands/SlaveCommandHandler.h"
#include "../lib/ReadWriteQueue/ReadWriteQueue.h"
#include "../lib/Settings/Settings.h"
#include "../lib/Slave/Slave.h"
#include "../lib/Utils/FileUtils.h"

namespace fs = std::filesystem;

int main()
{
    try
    {
        fs::create_directories("../game_data");
        std::cout << "Slave: Ensured that ../game_data exists.\n";
    } catch (const fs::filesystem_error& e)
    {
        std::cerr << "Slave: Failed to create game_data directory: " << e.what() << "\n";
        return 1;
    }

    Settings settings;
    ReadWriteQueue manager;
    Slave slaveLogic(settings);

    initGameDataFilesForSlave();

    SlaveCommandHandler commandHandler(settings, manager, slaveLogic);

    std::cout << "Slave: Waiting for game to start...\n";

    while (!settings.getStart() && !settings.getExit())
    {
        commandHandler.handleGameStartIfNeeded();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    if (settings.getExit())
    {
        std::cout << "Slave: Exiting game before start.\n";
        return 0;
    }

    std::cout << "Slave: Game started! Use 'shot X Y' or 'exit' commands.\n";

    std::string inputLine;
    while (true)
    {
        if (!std::getline(std::cin, inputLine))
        {
            break;
        }

        bool shouldExit = commandHandler.handleCommand(inputLine);
        if (shouldExit)
        {
            break;
        }
        if (settings.getExit())
        {
            break;
        }
    }

    std::cout << "Slave: Game ended.\n";
    return 0;
}