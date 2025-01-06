#include "ReadWriteQueue.h"
#include <iostream>

int ReadWriteQueue::readQueueFromFile(const std::string& filename) {
    std::lock_guard<std::mutex> lock(queue_mutex);
    std::ifstream in(filename);
    if (!in.is_open()) {
        std::cerr << "ReadWriteQueue: Unable to open " << filename << " for reading." << std::endl;
        return -1; // Ошибка (подловил на ошибке)
    }
    int player = -1;
    in >> player;
    in.close();
    return player;
}

void ReadWriteQueue::writeQueueToFile(const std::string& filename, int player) {
    std::lock_guard<std::mutex> lock(queue_mutex);
    std::ofstream out(filename, std::ios::trunc);
    if (!out.is_open()) {
        std::cerr << "ReadWriteQueue: Unable to open " << filename << " for writing." << std::endl;
        return;
    }
    out << player;
    out.close();
}

void ReadWriteQueue::writeShotToFile(const std::string& filename, int player, uint64_t x, uint64_t y, const std::string& result) {
    std::lock_guard<std::mutex> lock(queue_mutex);
    std::ofstream out(filename, std::ios::app);
    if (!out.is_open()) {
        std::cerr << "ReadWriteQueue: Unable to open " << filename << " for appending." << std::endl;
        return;
    }
    out << "Player " << player << " shot at (" << x << ", " << y << "): " << result << "\n";
    out.close();
}
