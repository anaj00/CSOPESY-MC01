#include "FlatMemoryAllocator.h"
#include <algorithm>
#include <ctime>
#include <iostream>

MemoryManager::MemoryManager(int totalMemory) : totalMemory(totalMemory) {
    freeMemoryBlocks.push_back({ 0, totalMemory });
}

int MemoryManager::allocateMemory(int size) {
    std::lock_guard<std::mutex> lock(memoryMutex);

    for (auto it = freeMemoryBlocks.begin(); it != freeMemoryBlocks.end(); ++it) {
        if (it->second >= size) {
            int startAddress = it->first;
            int remainingSize = it->second - size;

            if (remainingSize > 0) {
                // Update the current block
                it->first += size;
                it->second = remainingSize;
            }
            else {
                // Remove the block if it's fully allocated
                freeMemoryBlocks.erase(it);
            }

            return startAddress;
        }
    }
    return -1; // Indicate that memory allocation failed
}

void MemoryManager::deallocateMemory(std::shared_ptr<Process> process) {
    std::lock_guard<std::mutex> lock(memoryMutex);
    for (auto it = allocatedMemory.begin(); it != allocatedMemory.end(); ++it) {
        if (it->second == process) {
            int start = it->first;
            int size = process->getMemorySize();

            // Find the correct position to insert the freed block
            auto insertPos = std::lower_bound(freeMemoryBlocks.begin(), freeMemoryBlocks.end(), start,
                [](const auto& block, int addr) { return block.first < addr; });

            freeMemoryBlocks.insert(insertPos, { start, size });
            allocatedMemory.erase(it);

            mergeAdjacentBlocks();
            return;
        }
    }
}

void MemoryManager::mergeAdjacentBlocks() {
    if (freeMemoryBlocks.size() < 2) return;

    std::sort(freeMemoryBlocks.begin(), freeMemoryBlocks.end(),
        [](const auto& a, const auto& b) { return a.first < b.first; });

    for (auto it = freeMemoryBlocks.begin(); it != freeMemoryBlocks.end() - 1; ) {
        auto next = it + 1;
        if (it->first + it->second == next->first) {
            it->second += next->second;
            freeMemoryBlocks.erase(next);
        }
        else {
            ++it;
        }
    }
}

void MemoryManager::reportMemoryStatus() {
    static int reportCounter = 0; // To differentiate between multiple reports
    std::ofstream file("memory_stamp_" + std::to_string(reportCounter++) + ".txt");
    if (!file) {
        std::cerr << "Error opening file." << std::endl;
        return;
    }

    std::time_t now = std::time(nullptr);
    char buffer[26]; // Buffer size should be 26 for ctime_s
    ctime_s(buffer, sizeof(buffer), &now);
    file << "Timestamp: " << buffer;
    file << "Number of processes in memory: " << allocatedMemory.size() << "\n";

    // Calculate total external fragmentation
    int totalFragmentation = 0;
    for (const auto& block : freeMemoryBlocks) {
        totalFragmentation += block.second;
    }
    file << "Total external fragmentation: " << totalFragmentation << " KB\n";

    // Print memory layout
    file << "\nMemory Layout:\n";

    file << "-----end----- = " << totalMemory << "\n\n";

    for (const auto& process : allocatedMemory) {
        file << (process.first + process.second->getMemorySize() - 1) << "\n" << process.second->getName() << "\n" << process.first << "\n\n";
    }

    file << "-----start----- = 0" << "\n\n";
}

void MemoryManager::addAllocatedMemory(int startAddress, std::shared_ptr<Process> process) {
    std::lock_guard<std::mutex> lock(memoryMutex);
    allocatedMemory.push_back({ startAddress, process });
}
