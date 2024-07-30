#pragma once

#include <thread>
#include <atomic>
#include <iostream>
#include "ConfigurationManager.h"
#include "Process.h"
#include "FlatMemoryAllocator.h"
#include "PagingAllocator.h"

class MemoryManager
{
public:
    MemoryManager();
    ~MemoryManager();

    bool initialize(ConfigurationManager* configManager);
    bool allocate(Process process);
    void deallocate(int pid);

    void stop(); // Method to stop the thread

private:
    void run(); // Method that the thread will execute

    ConfigurationManager* configManager;
    FlatMemoryAllocator flatAllocator;
    PagingAllocator pagingAllocator;
    std::string allocationType;

    std::thread memoryThread;
    std::atomic<bool> running;
};
