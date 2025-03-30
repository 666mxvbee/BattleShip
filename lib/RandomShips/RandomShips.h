#pragma once
#include "Settings/Settings.h"
#include <random>

class RandomShips {
private:
    Settings& settings;
    uint64_t width;
    uint64_t height;
    std::mt19937 rng;

public: 
    explicit RandomShips(Settings& s);
    void placeShips(const std::string& player);
};
