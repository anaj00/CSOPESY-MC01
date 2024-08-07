#include "FlatMemoryAllocator.h"
#include <iostream>
#include <algorithm>

FlatMemoryAllocator::FlatMemoryAllocator() {
    srand(static_cast<unsigned int>(time(nullptr))); // Initialize random seed once
}

void FlatMemoryAllocator::initialize(ConfigurationManager* newConfigManager) {
    configManager = newConfigManager;
    memorySize = static_cast<int>(configManager->getMaxOverallMemory());
    memoryBlocks.push_back({ 0, memorySize, true }); // Initialize with a single free block of the entire memory
}

void printBlockInfo(const MemoryBlock& block) {
	std::cout << "Start: " << block.start
		<< "\nSize: " << block.size
		<< "\nStatus: " << (block.isFree ? "Free" : "Allocated") << std::endl << std::endl;
}

bool FlatMemoryAllocator::allocate(Process process) {
    int processSize = static_cast<int>(process.getMemorySize());

    for (auto& block : memoryBlocks) {

        // check if block is free and has enough space for the process
        if (block.isFree && block.size >= processSize) {

            int currBlockSize = block.size;

            block.isFree = false;
            block.size = process.getMemorySize();

            processMemoryMap[process.getID()] = block.start;

            // split block if it has more space than the process
            if (currBlockSize > process.getMemorySize()) {
                memoryBlocks.push_back({ block.start + ((int) process.getMemorySize()), currBlockSize - ((int) process.getMemorySize()), true});
            }
            
            return true;
        }
    }
    return false; // no sufficient free block found
}

void FlatMemoryAllocator::deallocate(int pid) {
    auto it = processMemoryMap.find(pid);

    // find block with the process
    if (it != processMemoryMap.end()) {
        for (auto& block : memoryBlocks) {

            // check if block is the one with the process
            if (block.start == it->second) {
                block.isFree = true;
                processMemoryMap.erase(it);
                mergeFreeBlocks(); // Merge adjacent free blocks after deallocation
                break;
            }
        }
    }
}

int FlatMemoryAllocator::swapOutRandomProcess(const std::unordered_set<int>& runningProcessIDs) {
    if (processMemoryMap.empty()) {
        return -1;
    }

    std::vector<int> nonRunningProcesses;
    for (const auto& entry : processMemoryMap) {
        if (runningProcessIDs.find(entry.first) == runningProcessIDs.end()) {
            nonRunningProcesses.push_back(entry.first);
        }
    }

    if (nonRunningProcesses.empty()) {
        return -1;
    }

    int randIndex = rand() % nonRunningProcesses.size(); // get random index
    int pid = nonRunningProcesses[randIndex];
    deallocate(pid);
    return pid;
}

void FlatMemoryAllocator::mergeFreeBlocks() {

    // Sort memory blocks by start address
    std::sort(memoryBlocks.begin(), memoryBlocks.end(), [](const MemoryBlock& a, const MemoryBlock& b) {
        return a.start < b.start;
        });

    // Iterate through memory blocks and merge adjacent free blocks
    for (size_t i = 0; i < memoryBlocks.size() - 1; ++i) {
        if (memoryBlocks[i].isFree && memoryBlocks[i + 1].isFree) { // Check if current block and next block are free
            if (memoryBlocks[i].start + memoryBlocks[i].size == memoryBlocks[i + 1].start) { // Check if blocks are contiguous (magkatabi)
                memoryBlocks[i].size += memoryBlocks[i + 1].size; // Merge blocks by adding their sizes onto current block
                memoryBlocks.erase(memoryBlocks.begin() + i + 1); // Erase next block
                --i; // Check again in case there are multiple contiguous free blocks
            }
        }
    }
}

void FlatMemoryAllocator::displayMemory() {
    std::cout << "Memory Blocks:" << std::endl;
    for (const auto& block : memoryBlocks) {
        std::cout << "Start: " << block.start 
            << "\nSize: " << block.size
            << "\nStatus: " << (block.isFree ? "Free" : "Allocated") << std::endl << std::endl;
    }

}

// Total Memory allocated (active + inacive)
int FlatMemoryAllocator::getUsedMemory() {
    int usedMemory = 0;
    for (const auto& block : memoryBlocks) {
        if (!block.isFree) {
            usedMemory += block.size;
        }
    }
    return usedMemory;
}

int FlatMemoryAllocator::getInactiveMemory(const std::unordered_set<int>& runningProcessIDs) {
    int inactiveMemory = 0;
    for (const auto& block : memoryBlocks) {
        if (!block.isFree && runningProcessIDs.find(block.start) == runningProcessIDs.end()) {
            inactiveMemory += block.size;
        }
    }
    return inactiveMemory;
}
