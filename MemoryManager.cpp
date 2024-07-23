#include "MemoryManager.h"

MemoryManager::MemoryManager(AllocationType type) : allocationType(type) {}

bool MemoryManager::allocate(Process process) {
    if (allocationType == FLAT) {
        if (!flatAllocator.allocate(process)) {
            flatAllocator.swapOutRandomProcess();
            return flatAllocator.allocate(process);
        }
    }
    else if (allocationType == PAGING) {
        return pagingAllocator.allocate(process);
    }
    return false;
}

void MemoryManager::deallocate(int pid) {
    if (allocationType == FLAT) {
        flatAllocator.deallocate(pid);
    }
    else if (allocationType == PAGING) {
        pagingAllocator.deallocate(pid);
    }
}