#pragma once

#include <vector>
#include <unordered_map>

#include "Process.h"

class PagingAllocator
{
public:
	PagingAllocator(int memorySize, int pageSize);
	bool allocate(Process process);
	void deallocate(int pid);

private:
	std::vector<int> memory; // memory represented as page frames
	std::unordered_map<int, std::vector<int>> processPageTable; // map of process id to page frames
	int memorySize;
	int pageSize;
};

