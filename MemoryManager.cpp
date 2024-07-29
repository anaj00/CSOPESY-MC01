#include "MemoryManager.h"


MemoryManager::MemoryManager(ConfigurationManager* configManager)
    : configManager(configManager),
    flatAllocator(configManager->getMemoryManagerAlgorithm() == "flat" ? FlatMemoryAllocator(configManager->getMaxOverallMemory()) : FlatMemoryAllocator()), // Correctly initialize here
    pagingAllocator(configManager->getMemoryManagerAlgorithm() == "paging" ? PagingAllocator(configManager->getMaxOverallMemory()) : PagingAllocator()) // Correctly initialize here
{
    if (configManager->getMemoryManagerAlgorithm() == "flat") {
        allocationType = "flat";
    }
    else if (configManager->getMemoryManagerAlgorithm() == "paging") {
        allocationType = "paging";
    }
}

bool MemoryManager::allocate(Process process) {
    if (allocationType == "flat") {
        if (!flatAllocator.allocate(process)) {
            flatAllocator.swapOutRandomProcess();
            return flatAllocator.allocate(process);
        }
    }
    else if (allocationType == "paging") {
        return pagingAllocator.allocate(process);
    }
    return false;
}

void MemoryManager::deallocate(int pid) {
    if (allocationType == "flat") {
        flatAllocator.deallocate(pid);
    }
    else if (allocationType == "paging") {
        pagingAllocator.deallocate(pid);
    }
}