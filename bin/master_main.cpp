#include <iostream>
#include <filesystem>

#include "../lib/Commands/MasterCommandHandler.h"
#include "../lib/ReadWriteQueue/ReadWriteQueue.h"
#include "../lib/Settings/Settings.h"
#include "../lib/Master/Master.h"
#include "../lib/Utils/FileUtils.h"

namespace fs = std::filesystem;

int main()
{
    try
    {
        fs::create_directories("../game_data");
    }
    catch(...)
    {
        std::cerr << "Master: Failed to create game_data directory.\n";
        return 1;
    }

    Settings settings;
    ReadWriteQueue manager;
    Master masterLogic(settings);

    initGameDataFilesForMaster();

    std::cout << "Master: Enter commands (e.g. set mode master, set width 10, set count 4 1, start)\n";
    
    MasterCommandHandler commandHandler(settings, manager, masterLogic);

    std::cout << "Master: Enter commands...\n";

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

    std::cout << "Master: Game ended.\n";
    return 0;
}