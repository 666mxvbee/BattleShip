#pragma once
#include <vector>
#include <cstdint>

class Ship {
public:
    int len; // len of the ship
    char orientation; // 'h' / 'v'
    uint64_t x; // X
    uint64_t y; // Y
    std::vector<bool>hits;

    Ship(int length, char orient, uint64_t xcord, uint64_t ycord);

    bool isSunk() const;
    bool hitAt(uint64_t shotX, uint64_t shotY);
    bool contains(uint64_t shotX, uint64_t shotY) const;
};
