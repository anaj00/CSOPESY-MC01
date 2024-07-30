#include <iostream>

#include "MemoryManager.h"


MemoryManager::MemoryManager() : running(false) // Initialize running to false
{}

MemoryManager::~MemoryManager() {
    stop();
    if (memoryThread.joinable()) {
        memoryThread.join();
    }
}

bool MemoryManager::initialize(ConfigurationManager* configManager) {
    try {
        this->configManager = configManager;
        allocationType = configManager->getMemoryManagerAlgorithm();

        if (allocationType == "flat") {
            flatAllocator.initialize(configManager);
        } else if (allocationType == "paging") {
            pagingAllocator.initialize(configManager);
        }

        running = true;
        memoryThread = std::thread(&MemoryManager::run, this);
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error initializing memory manager: " << e.what() << std::endl;
        return false;
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

void MemoryManager::stop() {
    running = false;
}

void MemoryManager::run() {
    while (running) {
        // Perform periodic memory management tasks
        // For example: Check memory status, handle requests, etc.

        // Here, we'll just simulate work with sleep
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Adjust as needed
    }
}