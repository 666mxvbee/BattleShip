#pragma once
#include <string>
#include <fstream>
#include <mutex>

class ReadWriteQueue {
private:
    std::mutex queue_mutex;

public:
    int readQueueFromFile(const std::string& fileName);

    void writeQueueToFile(const std::string& fileName, int player);

    void writeShotToFile(const std::string& filename, int player, uint64_t x, uint64_t y, const std::string& result);
};
