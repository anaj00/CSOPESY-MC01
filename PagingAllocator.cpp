#include "PagingAllocator.h"
#include <iostream>
#include <algorithm>
#include <ctime>

PagingAllocator::PagingAllocator() {}

void PagingAllocator::initialize(ConfigurationManager* configManager) {
    this->configManager = configManager;

    memorySize = configManager->getMaxOverallMemory();
    pageSize = configManager->getMinPagePerProcess(); // I dont think need?
    int numFrames = memorySize / pageSize;
    memory.resize(numFrames, -1); // Initialize all frames as free
}

bool PagingAllocator::allocate(Process process, std::function<void(std::shared_ptr<Process>)> swapOutCallback) {
    int pagesNeeded = pageSize;
    std::vector<int> allocatedFrames;

    while (pagesNeeded > 0) {
        int freeFrame = findFreeFrame();
        if (freeFrame == -1) {
            // No free frame found, swap out a random page
            std::unordered_set<int> runningProcessIDs; // Obtain this from the running processes
            int swappedOutProcessID = swapOutRandomPage(runningProcessIDs, swapOutCallback);

            if (swappedOutProcessID == -1) {
                // Rollback if not enough pages were found
                for (int frame : allocatedFrames) {
                    memory[frame] = -1; // Mark the allocated pages as free
                }
                return false;
            }
        }
        else {
            memory[freeFrame] = process.getID();
            allocatedFrames.push_back(freeFrame);
            ++numPagesPagedIn; // Increment the counter for pages paged in
            --pagesNeeded;
        }
    }

    processPageTable[process.getID()] = allocatedFrames;
    return true;
}



void PagingAllocator::deallocate(int pid) {
    auto it = processPageTable.find(pid);
    if (it != processPageTable.end()) {
        for (int frame : it->second) {
            memory[frame] = -1; // Free the frame
        }
        processPageTable.erase(it);
    }
}

int PagingAllocator::findFreeFrame() {
    for (int i = 0; i < memory.size(); ++i) {
        if (memory[i] == -1) {
            return i;
        }
    }
    return -1; // No free frame found
}

int PagingAllocator::swapOutRandomPage(const std::unordered_set<int>& runningProcessIDs, std::function<void(std::shared_ptr<Process>)> swapOutCallback) {
    std::vector<int> swappableFrames;
    for (int i = 0; i < memory.size(); ++i) {
        if (memory[i] != -1 && runningProcessIDs.find(memory[i]) == runningProcessIDs.end()) {
            swappableFrames.push_back(i);
        }
    }

    if (swappableFrames.empty()) {
        return -1;
    }

    srand(static_cast<unsigned int>(time(nullptr)));
    int frameToSwap = swappableFrames[rand() % swappableFrames.size()];
    swapOutPage(frameToSwap, swapOutCallback);
    return memory[frameToSwap];
}

// TODO: Fix the shit
void PagingAllocator::swapOutPage(int frame, std::function<void(std::shared_ptr<Process>)> swapOutCallback) {
    int pid = memory[frame];
    if (pid != -1) {
        //std::cout << "Swapping out page of process " << pid << " from frame " << frame << std::endl;
        memory[frame] = -1; // Mark frame as free
        ++numPagesPagedOut;

        auto it = processPageTable.find(pid);
        if (it != processPageTable.end()) {
            // Retrieve the process details from a suitable source
            std::string processName = "Process" + std::to_string(pid); // Example process name
            int totalInstructions = 1000; // Example total instructions
            float memorySize = it->second.size() * pageSize; // Calculate memory size from the number of pages
            float processPageSize = static_cast<float>(pageSize); // Ensure it's a float

            auto swappedOutProcess = std::make_shared<Process>(processName, pid, totalInstructions, memorySize, processPageSize);

            if (swapOutCallback) {
                swapOutCallback(swappedOutProcess);
            }

            // Remove the page frame from the process's page table
            processPageTable[pid].erase(std::remove(processPageTable[pid].begin(), processPageTable[pid].end(), frame), processPageTable[pid].end());

            // If the process has no more pages, remove it from the page table
            if (processPageTable[pid].empty()) {
                processPageTable.erase(pid);
            }
        }
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
            usedMemory += (it->second.size() * process->getPageSize());
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

int PagingAllocator::getNumPagesPagedIn() const {
    return numPagesPagedIn;
}

int PagingAllocator::getNumPagesPagedOut() const {
    return numPagesPagedOut;
}

int PagingAllocator::getInactiveMemory(const std::unordered_set<int>& runningProcessIDs) const {
    int inactiveMemory = 0;
    for (const auto& entry : processPageTable) {
        if (runningProcessIDs.find(entry.first) == runningProcessIDs.end()) {
            inactiveMemory += entry.second.size() * pageSize;
        }
    }
    return inactiveMemory;
}
