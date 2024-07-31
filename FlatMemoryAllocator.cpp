#include "FlatMemoryAllocator.h"
#include <iostream>
#include <algorithm>

FlatMemoryAllocator::FlatMemoryAllocator() {
    srand(static_cast<unsigned int>(time(nullptr))); // Initialize random seed once
}

void FlatMemoryAllocator::initialize(ConfigurationManager* newConfigManager) {
    configManager = newConfigManager;
    memoryBlocks.push_back({ 0, (int) configManager->getMaxOverallMemory(), true});
}

bool FlatMemoryAllocator::allocate(Process process) {

    for (auto& block : memoryBlocks) {
        // check if block is free and has enough space for the process
        if (block.isFree && block.size >= process.getMemorySize()) {
            block.isFree = false;
            processMemoryMap[process.getID()] = block.start;

            // split block if it has more space than the process
            if (block.size > process.getMemorySize()) {
                memoryBlocks.push_back({ block.start + ((int) process.getMemorySize()), block.size - ((int) process.getMemorySize()), true});
            }

            // update block size to process size
            block.size = process.getMemorySize();
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

void FlatMemoryAllocator::swapOutRandomProcess() {
    if (!processMemoryMap.empty()) {
        int randIndex = rand() % processMemoryMap.size(); // get random index
        auto it = processMemoryMap.begin(); // get iterator to random index
        std::advance(it, randIndex); // advance iterator to random index
        //std::cout << "Swapping out process with PID: " << it->first << std::endl;
        deallocate(it->first);
    }
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
        std::cout << "Start: " << block.start << ", Size: " << block.size
            << ", Status: " << (block.isFree ? "Free" : "Allocated") << std::endl;
    }
}

