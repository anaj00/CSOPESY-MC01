#include "PagingAllocator.h"

PagingAllocator::PagingAllocator(int memorySize) 
	: memorySize(memorySize) {}

bool PagingAllocator::allocate(Process process) {
    int pagesNeeded = (process.getMemorySize() + process.getPageSize() - 1) / process.getPageSize();
    std::vector<int> allocatedPages;
    for (int i = 0; i < memory.size(); ++i) {
        if (memory[i] == -1) {
            memory[i] = process.getID();
            allocatedPages.push_back(i);
            if (--pagesNeeded == 0) {
                processPageTable[process.getID()] = allocatedPages;
                return true;
            }
        }
    }
    // Rollback if not enough pages
    for (int page : allocatedPages) {
        memory[page] = -1;
    }
    return false;
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