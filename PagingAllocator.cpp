#include "PagingAllocator.h"

#include <iostream>

PagingAllocator::PagingAllocator() {}

void PagingAllocator::initialize(ConfigurationManager* configManager) {
    configManager = configManager;
	memory.resize(configManager->getMaxOverallMemory(), -1); // Initialize memory frames to -1 (indicating they are free)
}

bool PagingAllocator::allocate(Process process) {
    int pageSize = process.getPageSize();  // Get the page size from the process
    if (pageSize <= 0) {
        std::cout << "Page size must be greater than 0";  // Error handling for invalid page size
        return false;
    }

    int pagesNeeded = (process.getMemorySize() + pageSize - 1) / pageSize;  // Calculate the number of pages needed for the process
    std::vector<int> allocatedPages;  // Vector to store allocated page indices

    // Iterate through the memory to find free pages
    for (int i = 0; i < memory.size(); ++i) {
        if (memory[i] == -1) {  // Check if the page frame is free
            memory[i] = process.getID();  // Allocate the page to the process
            allocatedPages.push_back(i);  // Store the allocated page index
            if (--pagesNeeded == 0) {  // If all needed pages are allocated
                processPageTable[process.getID()] = allocatedPages;  // Update the process page table
                return true;  // Allocation successful
            }
        }
    }

    // Rollback if not enough pages were found
    for (int page : allocatedPages) {
        memory[page] = -1;  // Mark the allocated pages as free
    }
    return false;  // Allocation failed
}

void PagingAllocator::deallocate(int pid) {
    auto it = processPageTable.find(pid);
    if (it != processPageTable.end()) {
        for (int page : it->second) {
            memory[page] = -1;
        }
        processPageTable.erase(it);
    }
}

int PagingAllocator::getUsedMemory(std::vector<std::shared_ptr<Process>> processes) const {
    int usedMemory = 0;

    for (const auto& process : processes) {
        // Get the process ID from the process object
        int pid = process->getID();

        // Find the process ID in the process page table
        auto it = processPageTable.find(pid);
        if (it != processPageTable.end()) {
            // Calculate the memory used by this process
            usedMemory += (it->second.size() * process->getPageSize()) + 2 - process->getPageSize();
        }
    }

    return usedMemory;
}

std::vector<int> PagingAllocator::getProcessKeys() const {
    std::vector<int> processIDs;
    for (const auto& entry : processPageTable) {
        processIDs.push_back(entry.first);
    }
    return processIDs;
}