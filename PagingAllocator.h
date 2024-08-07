#pragma once

#include <vector>
#include <unordered_map>
#include <memory>
#include <functional>
#include <unordered_set>
#include "Process.h"
#include "ConfigurationManager.h"

class PagingAllocator
{
public:
    PagingAllocator();

    void initialize(ConfigurationManager* configManager);
    bool allocate(Process process, std::function<void(std::shared_ptr<Process>)> swapOutCallback);
    void deallocate(int pid);

    int getUsedMemory(std::vector<std::shared_ptr<Process>> processes) const;
    std::vector<int> getProcessKeys() const;
    int swapOutRandomPage(const std::unordered_set<int>& runningProcessIDs, std::function<void(std::shared_ptr<Process>)> swapOutCallback);

    int getNumPagesPagedIn() const;
    int getNumPagesPagedOut() const;

    int getInactiveMemory(const std::unordered_set<int>& runningProcessIDs) const;

private:
    ConfigurationManager* configManager;
    int memorySize;
    int pageSize;
    int numPagesPagedIn;
    int numPagesPagedOut;

    std::vector<int> memory; // memory represented as page frames
    std::unordered_map<int, std::vector<int>> processPageTable; // map of process id to page frames

    int findFreeFrame();
    void swapOutPage(int frame, std::function<void(std::shared_ptr<Process>)> swapOutCallback);
};
