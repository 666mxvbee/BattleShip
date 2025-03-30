#pragma once
#include <vector>
#include <cstdint>

class Ship {
public:
    int len;
    char orientation;
    uint64_t x;
    uint64_t y;
    std::vector<bool>hits;

    Ship(int length, char orient, uint64_t xcord, uint64_t ycord);

    bool isSunk() const;
    bool hitAt(uint64_t shotX, uint64_t shotY);
    bool contains(uint64_t shotX, uint64_t shotY) const;
};
