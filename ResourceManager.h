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

	bool initialize(ConfigurationManager* newConfigManager);

	std::shared_ptr<Process> createProcess(const std::string process_name);
	bool processExists(std::string name);
	std::shared_ptr<Process> findProcessByName(const std::string name);
	void allocateAndScheduleProcesses();

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
	std::thread allocationThread;

	int processCounter;
	float getRandomFloat(float min, float max);
};

