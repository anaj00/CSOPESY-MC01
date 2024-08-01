#pragma once

#include <vector>
#include <unordered_map>
#include <memory>

#include "Process.h"
#include "ConfigurationManager.h"

class PagingAllocator
{
public:
	PagingAllocator();

	void initialize(ConfigurationManager* configManager);
	bool allocate(Process process);
	void deallocate(int pid);

	int getUsedMemory(std::vector<std::shared_ptr<Process>> processes) const;
	std::vector<int> getProcessKeys() const;
private:
	ConfigurationManager* configManager;

	// free frame list
	std::vector<int> memory; // memory represented as page frames
	std::unordered_map<int, std::vector<int>> processPageTable; // map of process id to page frames
	int memorySize;
};

