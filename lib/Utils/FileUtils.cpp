#include "FileUtils.h"
#include <fstream>
#include <iostream>
#include <mutex>
#include <filesystem>

namespace {
    std::mutex g_fileMutex;
}

void saveSettingsToFile(const std::string& filename, const Settings& settings) {
    std::lock_guard<std::mutex> lock(g_fileMutex);

    std::ofstream out(filename, std::ios::trunc);
    if (!out.is_open()) {
        std::cerr << "Error: Unable to open " << filename << " for writing.\n";
        return;
    }

    out << "mode=" << settings.getMode() << "\n";
    out << "width=" << settings.getWidth() << "\n";
    out << "height=" << settings.getHeight() << "\n";

    for (auto& [type, cnt] : settings.getShipCounts()) {
        out << "count=" << type << " " << cnt << "\n";
    }

    if (settings.hasLastShot()) {
        auto shot = settings.getLastShot();
        out << "lastShot=" << shot.first << " " << shot.second << "\n";
    }

    out << "lastResult=" << settings.getLastResult() << "\n";
    out << "start=" << (settings.getStart() ? "1" : "0") << "\n";
    out << "stop=" << (settings.getStop() ? "1" : "0") << "\n";
    out << "exit=" << (settings.getExit() ? "1" : "0") << "\n";

    out.close();
}

void initGameDataFilesForMaster() {
    resetGameDataDirectory("../game_data");
    {
        std::ofstream settingsFile("../game_data/settings.txt", std::ios::trunc);
        if (!settingsFile.is_open()) {
            std::cerr << "Master: Unable to open settings.txt\n";
        }
    }
    {
        std::ofstream shotsFile("../game_data/shots.txt", std::ios::trunc);
        if (!shotsFile.is_open()) {
            std::cerr << "Master: Unable to open shots.txt\n";
        }
    }
    {
        std::ofstream queueFile("../game_data/queue.txt", std::ios::trunc);
        if (!queueFile.is_open()) {
            std::cerr << "Master: Unable to open queue.txt\n";
        } else {
            queueFile << "1";
        }
    }
    std::cout << "Master: game_data files initialized.\n";
}

void initGameDataFilesForSlave() {
    {
        std::ofstream settingsFile("../game_data/settings.txt", std::ios::trunc);
        if (!settingsFile.is_open()) {
            std::cerr << "Slave: Unable to open settings.txt\n";
        }
    }
    {
        std::ofstream shotsFile("../game_data/shots.txt", std::ios::trunc);
        if (!shotsFile.is_open()) {
            std::cerr << "Slave: Unable to open shots.txt\n";
        }
    }
    {
    if (!std::filesystem::exists("../game_data/queue.txt")) {
        std::ofstream queueFile("../game_data/queue.txt");
        if (queueFile.is_open()) {
            queueFile << 2;
        }
    }
    }
    std::cout << "Slave: game_data files initialized.\n";
}

void resetGameDataDirectory(const std::string& path) {
    std::error_code ec;
    std::filesystem::remove_all(path, ec);

    if (!std::filesystem::create_directories(path, ec)) {
        if (ec) {
            std::cerr << "Error while creating directory " << path << ": " << ec.message() << "\n";
        }
    }
}