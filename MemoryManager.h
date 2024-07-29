#pragma once

#include "ConfigurationManager.h"
#include "Process.h"
#include "FlatMemoryAllocator.h"
#include "PagingAllocator.h"

class MemoryManager
{
public: 
	MemoryManager(ConfigurationManager* configManager);
	bool allocate(Process process);
	void deallocate (int pid);

private: 
	ConfigurationManager* configManager;

	FlatMemoryAllocator flatAllocator;
	PagingAllocator pagingAllocator;
	std::string allocationType;
};

