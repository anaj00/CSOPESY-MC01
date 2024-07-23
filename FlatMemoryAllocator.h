#pragma once

#include <vector>
#include <unordered_map>

#include "Process.h"

// MemoryBlock struct (for flat alloc)
struct MemoryBlock {
	int start;	// start address of the block
	int size;	// size of the block
	bool isFree;	// is the block free?
};

class FlatMemoryAllocator
{
public:
	FlatMemoryAllocator(int memorySize);
	bool allocate(Process process);	// allocate memory for a process
	void deallocate(int pid);
	void swapOutRandomProcess();	// swap out a random process

private:
	std::vector<MemoryBlock> memoryBlocks;	// memory blocks
	std::unordered_map<int, int> processMemoryMap;	// memory block index
	int memorySize;	// total memory size
};

