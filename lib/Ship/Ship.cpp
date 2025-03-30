#include "Ship.h"
#include <iostream>

Ship::Ship(int length, char orient, uint64_t xcord, uint64_t ycord)
    : len(length), orientation(orient), x(xcord), y(ycord), hits(length, false) {}

bool Ship::isSunk() const {
    for (bool h : hits) {
        if (!h) return false;
    }
    return true;
}

bool Ship::hitAt(uint64_t shotX, uint64_t shotY) {
    if (orientation == 'h') {
        if (shotY == y && shotX >= x && shotX < x + len) {
            hits[ shotX - x ] = true;
            std::cout << "Ship::hitAt(): Hit at(" << shotX << ", " << shotY << ")." << std::endl;
            return true;
        }
    } else if (orientation == 'v') {
        if (shotX == x && shotY >= y && shotY < y + len) {
            hits[ shotY - y ] = true;
            std::cout << "Ship::hitAt(): Hit at(" << shotX << ", " << shotY << ")." << std::endl;
            return true;
        }
    }
    return false;
}


bool Ship::contains(uint64_t shotX, uint64_t shotY) const {
    if (orientation == 'h') {
        return (shotY == y && shotX >= x && shotX < x + len);
    }
    else {
        return (shotX == x && shotY >= y && shotY < y + len);
    }
}