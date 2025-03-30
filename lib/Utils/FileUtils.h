#pragma once
#include <string>
#include "../Settings/Settings.h"

void saveSettingsToFile(const std::string& filename, const Settings& settings);
void initGameDataFilesForMaster();
void initGameDataFilesForSlave();
