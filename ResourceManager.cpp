#include "ResourceManager.h"

#include <cstdlib>
#include <ctime>
#include <random>
#include <algorithm>

ResourceManager::ResourceManager() : processCounter(0) {
	srand(static_cast<unsigned int>(time(0))); // Seed the random number generator
	allocationThread = std::thread(&ResourceManager::allocateAndScheduleProcesses, this); // Start the allocation thread
}

ResourceManager::~ResourceManager() {
	{
		std::lock_guard<std::mutex> lock(processMutex);
		running = false;
	}
	processAdded.notify_all();
	if (allocationThread.joinable()) {
		allocationThread.join();
	}
}

bool ResourceManager::initialize(ConfigurationManager* newConfigManager){	
	configManager = newConfigManager;

	// Initialize the scheduler and memory manager
	if (scheduler.initialize(configManager) && memoryManager.initialize(configManager)) {
		running = true;
		return true;
	} else {
		return false;
	}
}

std::shared_ptr<Process> ResourceManager::createProcess(std::string process_name) {
	std::lock_guard<std::mutex> lock(processMutex);

	processCounter++;

	// Generate random values for the process
	float randomMaxInstructions = getRandomFloat(configManager->getMinInstructions(), configManager->getMaxInstructions());
	float randomMemory = getRandomFloat(configManager->getMinMemoryPerProcess(), configManager->getMaxMemoryPerProcess());
	float randomPage = getRandomFloat(configManager->getMinPagePerProcess(), configManager->getMaxPagePerProcess());

	// Create a new process
	auto newProcess = std::make_shared<Process>(process_name, processCounter, randomMaxInstructions, randomMemory, randomPage);
	processes.push_back(newProcess);

	// Notify the allocation thread
	processAdded.notify_all();

	return newProcess;
}

bool ResourceManager::processExists(std::string name) {
	auto it = std::find_if(processes.begin(), processes.end(),
		[&name](const std::shared_ptr<Process>& process) {
			return process->getName() == name;
		});
	return it != processes.end();
}

std::shared_ptr<Process> ResourceManager::findProcessByName(const std::string name) {
	auto it = std::find_if(processes.begin(), processes.end(),
		[&name](const std::shared_ptr<Process>& process) {
			return process->getName() == name;
		});

	if (it != processes.end()) {
		return *it;
	}
	else {
		return nullptr; // or you could throw an exception if preferred
	}
}

void ResourceManager::allocateAndScheduleProcesses() {
	while (running) {
		std::unique_lock<std::mutex> lock(processMutex);
		processAdded.wait(lock, [this] { return !running || !processes.empty(); }); // Wait until a process is added or the program is stopped

		if (!running) {
			break;
		}

		auto process = processes.back(); // Get the last process added
		lock.unlock(); // Unlock the mutex to allow other threads to add processes

		// Try to allocate memory for the process
		if (memoryManager.allocate(*process)) {
			// Add process to the scheduler if memory allocation is successful
			scheduler.addProcess(*process);
		}
		else {
			// If memory allocation fails, remove the process from the list
			std::lock_guard<std::mutex> lock(processMutex);
			processes.erase(std::remove(processes.begin(), processes.end(), process), processes.end());
		}
	}
}

float ResourceManager::getRandomFloat (float min, float max) {
	return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
}

Scheduler* ResourceManager::getScheduler()
{
	return &scheduler;
}
MemoryManager* ResourceManager::getMemoryManager()
{
	return &memoryManager;
}
