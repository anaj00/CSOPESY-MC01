#pragma once

#include <vector>
#include <unordered_map>

#include "Process.h"
#include "ConfigurationManager.h"

// MemoryBlock struct (for flat alloc)
struct MemoryBlock {
	int start;	// start address of the block
	int size;	// size of the block
	bool isFree;	// is the block free?
};

class FlatMemoryAllocator
{
public:
	FlatMemoryAllocator();

	void initialize(ConfigurationManager* configManager);	// initialize memory blocks
	bool allocate(Process process);	// allocate memory for a process
	void deallocate(int pid);
	void swapOutRandomProcess();	// swap out a random process

private:
	ConfigurationManager* configManager;	// configuration manager
	std::vector<MemoryBlock> memoryBlocks;	// memory blocks
	std::unordered_map<int, int> processMemoryMap;	// memory block index
	int memorySize;	// total memory size
};

