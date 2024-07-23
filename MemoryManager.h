#pragma once

#include "Process.h"
#include "FlatMemoryAllocator.h"
#include "PagingAllocator.h"

// Enum for memory allocation type
enum AllocationType {
	FLAT,
	PAGING
};

class MemoryManager
{
public: 
	MemoryManager(AllocationType type);
	bool allocate(Process process);
	void deallocate (int pid);

private: 
	FlatMemoryAllocator flatAllocator;
	PagingAllocator pagingAllocator;
	AllocationType allocationType;
};

