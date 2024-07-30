#pragma once

#include "MemoryManager.h"
#include "Scheduler.h"
#include "ConfigurationManager.h"

#include <vector>
#include <memory>
#include <mutex>
#include <thread>
#include <condition_variable>

class ResourceManager
{
public:
	ResourceManager();
	~ResourceManager();

	bool initialize(ConfigurationManager* newConfigManager); // Initialize the scheduler and the memory manager

	std::shared_ptr<Process> createProcess(const std::string process_name);
	bool processExists(std::string name); // Check if a process with the given name exists
	std::shared_ptr<Process> findProcessByName(const std::string name); // Returns a process by its name
	void allocateAndScheduleProcesses(); // Allocate memory for processes and schedule them

	Scheduler* getScheduler();
	MemoryManager* getMemoryManager();

private:
	ConfigurationManager* configManager;
	MemoryManager memoryManager;
	Scheduler scheduler;
	
	bool running;
	std::vector<std::shared_ptr<Process>> processes;
	std::mutex processMutex;
	std::condition_variable processAdded;

	int processCounter;
	float getRandomFloat(float min, float max); // Randomizer for the process details
	float getRandomFloatN2(float min, float max); // Randomizer for the process details wherein number = n^2

	std::thread allocationThread;
	void startAllocationThread();
	void stopAllocationThread();
};

