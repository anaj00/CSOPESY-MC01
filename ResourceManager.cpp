#include "ResourceManager.h"

#include <cstdlib>
#include <ctime>
#include <random>
#include <algorithm>
#include <thread>
#include <iomanip>

ResourceManager::ResourceManager() : processCounter(0) {
	srand(static_cast<unsigned int>(time(0))); // Seed the random number generator
}

ResourceManager::~ResourceManager() {
	stopAllocationThread();
}

bool ResourceManager::initialize(ConfigurationManager* newConfigManager){	
	configManager = newConfigManager;

	// Initialize the scheduler and memory manager
	if (scheduler.initialize(configManager) && memoryManager.initialize(configManager, &scheduler)) {
		running = true;
		startAllocationThread();
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
	float randomMemory = getRandomFloatN2(configManager->getMinMemoryPerProcess(), configManager->getMaxMemoryPerProcess());
	float randomPage = getRandomFloatN2(configManager->getMinPagePerProcess(), configManager->getMaxPagePerProcess());

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
		processes.pop_back(); // Remove the process from the list
		lock.unlock(); // Unlock the mutex to allow other threads to add processes

		// Try to allocate memory for the process
		if (memoryManager.allocate(*process)) {
			// Add process to the scheduler if memory allocation is successful
			scheduler.addProcess(*process);
		}
		else {
			// If memory allocation fails, remove the process from the list
			// This should not happen if the memory manager is working correctly with a backing store
			std::lock_guard<std::mutex> lock(processMutex);
			processes.erase(std::remove(processes.begin(), processes.end(), process), processes.end());
		}
	}
}


void ResourceManager::startAllocationThread() {
	allocationThread = std::thread(&ResourceManager::allocateAndScheduleProcesses, this);
}

void ResourceManager::stopAllocationThread() {
	{
		std::lock_guard<std::mutex> lock(processMutex);
		running = false;
	}
	processAdded.notify_all();
	if (allocationThread.joinable()) {
		allocationThread.join();
	}
}

float ResourceManager::getRandomFloat (float min, float max) {
	return min + static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / (max - min)));
}

float ResourceManager::getRandomFloatN2(float min, float max) {

	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(std::sqrt(min), std::sqrt(max));

	float sqrtRandom = dis(gen);
	float quadraticRandom = sqrtRandom * sqrtRandom;

	return quadraticRandom;
}

Scheduler* ResourceManager::getScheduler()
{
	return &scheduler;
}
MemoryManager* ResourceManager::getMemoryManager()
{
	return &memoryManager;
}

// Scheduler test

void ResourceManager::startSchedulerTest() {
	std::lock_guard<std::mutex> lock(mtx);
	if (!schedulerTest) {
		schedulerTest = true;
		testThread = std::thread(&ResourceManager::schedulerTestLoop, this);
	}
}

void ResourceManager::stopSchedulerTest() {
	{
		std::lock_guard<std::mutex> lock(mtx);
		if (schedulerTest) {
			schedulerTest = false;
		}
	}
	if (testThread.joinable()) {
		testThread.join();  // Ensure the thread is properly joined
	}
}

void ResourceManager::schedulerTestLoop() {
	while (schedulerTest) {
		schedulerCounter++;
		int processID = schedulerCounter;
		createProcess("process_test" + std::to_string(processID));
		std::this_thread::sleep_for(std::chrono::duration<double>(configManager->getBatchProcessFrequency()));
	}
}

void ResourceManager::displayStatus() {
	std::lock_guard<std::mutex> lock(processMutex);

	int coresUsed = 0;
	const std::vector<std::unique_ptr<CoreWorker>>& cores = scheduler.getCoreWorkers();
	for (const auto& core : cores) {
		if (core->isAssignedProcess()) {
			coresUsed++;
		}
	}

	int totalCores = configManager->getNumCPU();
	int cpuUtilization = totalCores ? (coresUsed * 100 / totalCores) : 0;

	std::cout << "CPU utilization: " << cpuUtilization << "%\n";
	std::cout << "Cores used: " << coresUsed << "\n";
	std::cout << "Cores available: " << totalCores - coresUsed << "\n";
	std::cout << "--------------------------------------------\n";

	std::cout << "Running processes:\n";
	const std::vector<std::shared_ptr<Process>>& processes = scheduler.getProcesses();
	for (const auto& process : processes) {
		if (!process->isFinished()) {
			std::cout << std::left << std::setw(20) << process->getName()
				<< std::left << std::setw(30) << process->getCreationTime();

			// Check if the process has been assigned a core
			if (process->getCore() != -1) {
				std::cout << "Core:   " << std::setw(15) << process->getCore();
				std::cout << std::left << std::setw(1) << process->getCurrentInstruction() << " / "
					<< process->getTotalInstructions() << "\n";
			}
			else {
				std::cout << "Core:   " << std::setw(15) << " "; // Adjust the width to maintain alignment
				std::cout << std::left << std::setw(1) << process->getCurrentInstruction() << " / "
					<< process->getTotalInstructions() << "\n";
			}
		}
	}

	std::cout << "\nFinished processes:\n";

	for (const auto& process : processes) {
		if (process->isFinished()) {
			std::cout << std::left << std::setw(20) << process->getName()
				<< std::left << std::setw(30) << process->getCreationTime()
				<< "Core:   " << std::setw(15) << process->getCore()
				<< std::left << std::setw(1) << process->getCurrentInstruction() << " / "
				<< process->getTotalInstructions() << "\n";
		}
	}

	std::cout << "--------------------------------------------\n";
}


void ResourceManager::displayProcessSmi() {
	std::cout << "--------------------------------------------\n";
	std::cout << "| PROCESS-SMI V01.00 Driver Version 01.00 | \n";
	std::cout << "--------------------------------------------\n";
	std::cout << "CPU-Util: " << this->getCPUUtilization() << "%\n";
	std::cout << "Memory Usage: " << memoryManager.flatAllocator.getUsedMemory() << "/" << configManager->getMaxOverallMemory()  << "\n";
	std::cout << "Memory-Util: "<< this->getMemoryUtilization() << "%" << "%\n";
	std::cout << "============================================	\n";
	std::cout << "Running processes and memory usage: \n";
	std::cout << "--------------------------------------------\n";

	const std::vector<std::shared_ptr<Process>>& processes = scheduler.getProcesses();
	for (const auto& process : processes) {
		if (!process->isFinished() && process->getCore() != -1) {
			std::cout << std::left << std::setw(20) << process->getName()
				<< std::left << std::setw(30) << process->getMemorySize() << std::endl;
		}
	}
	std::cout << "--------------------------------------------\n";
}

void ResourceManager::displayVMStat() {
	std::cout << configManager->getMaxOverallMemory() << " KB total memory\n";
	std::cout << memoryManager.flatAllocator.getUsedMemory() << " KB used memory\n";
	// TODO: implement elements bellow	
	std::cout << " KB active memory";
	std::cout << " KB inactive memory\n";;
	std::cout << " idle cpu ticks\n";
	std::cout << " active cpu ticks\n";
	std::cout << " total cpu ticks\n";
	std::cout << " pages paged in\n";
	std::cout << " pages paged out\n";
}


int ResourceManager::getCPUUtilization() {
	int coresUsed = 0;
	const std::vector<std::unique_ptr<CoreWorker>>& cores = scheduler.getCoreWorkers();
	for (const auto& core : cores) {
		if (core->isAssignedProcess()) {
			coresUsed++;
		}
	}

	int totalCores = configManager->getNumCPU();
	int cpuUtilization = totalCores ? (coresUsed * 100 / totalCores) : 0;

	return cpuUtilization;
}

int ResourceManager::getMemoryUtilization() {
	int usedMemory = memoryManager.flatAllocator.getUsedMemory();
	int totalMemory = configManager->getMaxOverallMemory();
	int memoryUtilization = totalMemory ? (usedMemory * 100 / totalMemory) : 0;
	return memoryUtilization;
}

