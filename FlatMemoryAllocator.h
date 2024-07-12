#pragma once

#include <vector>
#include <mutex>
#include <memory>
#include <fstream>
#include "Process.h"

class MemoryManager {
public:
    MemoryManager(int totalMemory);

    int allocateMemory(int size);
    void deallocateMemory(std::shared_ptr<Process> process);
    void reportMemoryStatus();
    void addAllocatedMemory(int startAddress, std::shared_ptr<Process> process);
private:
    std::mutex memoryMutex;
    int totalMemory;
    std::vector<std::pair<int, int>> freeMemoryBlocks; // (start, size)
    std::vector<std::pair<int, std::shared_ptr<Process>>> allocatedMemory; // (start, process)

    void mergeAdjacentBlocks();
};
