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
	int randomMaxInstructions = getRandomInt(configManager->getMinInstructions(), configManager->getMaxInstructions());
	int randomPage = getRandomInt(configManager->getMinPagePerProcess(), configManager->getMaxPagePerProcess());
	int randomMemory = getRandomInt2N(configManager->getMinMemoryPerProcess(), configManager->getMaxMemoryPerProcess());
	

	// Create a new process
	auto newProcess = std::make_shared<Process>(process_name, processCounter, randomMaxInstructions, randomMemory, randomPage);
	processes.push_back(newProcess);
	processesMasterList.push_back(newProcess);

	// Notify the allocation thread
	processAdded.notify_all();

	return newProcess;
}

bool ResourceManager::processExists(std::string name) {
	auto it = std::find_if(processesMasterList.begin(), processesMasterList.end(),
		[&name](const std::shared_ptr<Process>& process) {
			return process->getName() == name;
		});
	return it != processesMasterList.end();
}

std::shared_ptr<Process> ResourceManager::findProcessByName(const std::string name) {
	auto it = std::find_if(processesMasterList.begin(), processesMasterList.end(),
		[&name](const std::shared_ptr<Process>& process) {
			return process->getName() == name;
		});

	if (it != processesMasterList.end()) {
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

int ResourceManager::getRandomInt (int min, int max) {
	static std::random_device rd;  // Seed for the random number engine
	static std::mt19937 gen(rd()); // Mersenne Twister random number engine

	// Create a uniform distribution in the range [min, max]
	std::uniform_int_distribution<> dis(min, max);

	// Generate a random number in the specified range
	return dis(gen);
}

int ResourceManager::getRandomInt2N(int min, int max) {

	int minExp = std::ceil(std::log2(min));
	// Calculate the largest power of 2 less than or equal to max
	int maxExp = std::floor(std::log2(max));

	// Ensure the range is valid
	if (minExp > maxExp) {
		throw std::invalid_argument("No power of 2 within the given range");
	}

	// Initialize random number generator
	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(minExp, maxExp);

	// Generate a random exponent between minExp and maxExp
	int randomExp = dis(gen);

	// Calculate the power of 2
	int result = std::pow(2, randomExp);

	return result;
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
	std::cout << "CPU-Util: " << getCPUUtilization() << "%\n";
	std::cout << "Memory Usage: " << memoryManager.getUsedMemory() << "/" << configManager->getMaxOverallMemory()  << "\n";
	std::cout << "Memory-Util: "<< getMemoryUtilization() << "%" << "%\n";
	std::cout << "============================================	\n";
	std::cout << "Running processes and memory usage: \n";
	std::cout << "--------------------------------------------\n";

	const std::vector<std::shared_ptr<Process>>& processes = scheduler.getProcesses();
	for (const auto& process : processes) {
		if (!process->isFinished() && process->getCore() != -1) {
			std::cout << std::left << std::setw(20) << process->getName()
				<< std::left << std::setw(30) << process->getMemorySize() << std::endl
				<< std::left << std::setw(30) << process->getPageSize() << std::endl;
		}
	}
	std::cout << "--------------------------------------------\n";
}

void ResourceManager::displayVMStat() {
	std::vector<long long> stats = getCoreStats();
	int usedMemory = memoryManager.getUsedMemory();
	int activeMemory = memoryManager.getActiveMemory();
	int inactiveMemory = memoryManager.getInactiveMemory();
	int pagedIn = memoryManager.pagingAllocator.getNumPagesPagedIn();
	int pagedOut = memoryManager.pagingAllocator.getNumPagesPagedOut();

	std::cout << configManager->getMaxOverallMemory() << " KB total memory\n";
	std::cout << usedMemory << " KB used memory\n"; // Total used memory, including possible external fragmentation
	std::cout << activeMemory << " KB active memory\n"; // Total active memory used by processes. This doesn’t include possible external fragmentation.
	std::cout << inactiveMemory << " KB inactive memory\n"; // External Fragmentation
	std::cout << stats[2] << " idle cpu ticks\n"; 
	std::cout << stats[1] << " active cpu ticks\n";
	std::cout << stats[0] << " total cpu ticks\n";
	std::cout << pagedIn << " pages paged in\n";
	std::cout << pagedOut << " pages paged out\n";
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
	int usedMemory = memoryManager.getUsedMemory();
	int totalMemory = configManager->getMaxOverallMemory();
	int memoryUtilization = totalMemory ? (usedMemory * 100 / totalMemory) : 0;
	return memoryUtilization;
}

std::vector<long long> ResourceManager::getCoreStats() {
	const std::vector<std::unique_ptr<CoreWorker>>& cores = scheduler.getCoreWorkers();
	long long cpuTicks = 0;
	long long activeTicks = 0;
	long long idleTicks = 0;

	for (const auto& core : cores) {
		std::vector<long long> stats = core->getStats();
		cpuTicks += stats[0];
		activeTicks += stats[1];
		idleTicks += stats[2];
	}

	return { cpuTicks, activeTicks, idleTicks };
}

void ResourceManager::saveReport() {
	std::cout << "Saving report..." << std::endl;

	std::ofstream file("csopesy-log.txt");
	if (!file) {
		std::cerr << "Error opening file." << std::endl;
		return;
	}

	std::lock_guard<std::mutex> lock(processMutex);

	int coresUsed = 0;
	const std::vector<std::unique_ptr<CoreWorker>>& cores = scheduler.getCoreWorkers();
	for (const auto& core : cores) {
		if (core->isAssignedProcess()) {
			coresUsed++;
		}
	}

	int totalCores = cores.size();
	int cpuUtilization = totalCores ? (coresUsed * 100 / totalCores) : 0;

	file << "CPU utilization: " << cpuUtilization << "%\n";
	file << "Cores used: " << coresUsed << "\n";
	file << "Cores available: " << totalCores - coresUsed << "\n";
	file << "Memory Usage: " << memoryManager.getUsedMemory() << "/" << configManager->getMaxOverallMemory() << "\n";
	file << "Memory-Util: " << getMemoryUtilization() << "%\n";
	file << "--------------------------------------------\n";

	file << "Running processes:\n";
	for (const auto& process : processesMasterList) {
		if (!process->isFinished()) {
			file << std::left << std::setw(20) << process->getName()
				<< std::left << std::setw(30) << process->getCreationTime();

			// Check if the process has been assigned a core
			if (process->getCore() != -1) {
				file << "Core:   " << std::setw(15) << process->getCore();
				file << std::left << std::setw(1) << process->getCurrentInstruction() << " / "
					<< process->getTotalInstructions() << "\n";
				file << " Memory Size: " << process->getMemorySize() << "\n";
			}
			else {
				file << "Core:   " << std::setw(15) << " "; // Adjust the width to maintain alignment
				file << std::left << std::setw(1) << process->getCurrentInstruction() << " / "
					<< process->getTotalInstructions() << "\n";
			}
		}
	}

	file << "\nFinished processes:\n";

	for (const auto& process : processesMasterList) {
		if (process->isFinished()) {
			file << std::left << std::setw(20) << process->getName()
				<< std::left << std::setw(30) << process->getCreationTime()
				<< "Core:   " << std::setw(15) << process->getCore()
				<< std::left << std::setw(1) << process->getCurrentInstruction() << " / "
				<< process->getTotalInstructions() << "\n";
		}
	}

	file << "--------------------------------------------\n";

	std::cout << "Report saved at csopesy-log.txt!" << std::endl;
}

void ResourceManager::displayAllProcesses() {
	const std::vector<std::shared_ptr<Process>>& processes = scheduler.getProcesses();
	std::cout << "=========================\n";
	std::cout << "in display all processes\n";
	for (auto& process : processes) {
		std::cout << process->getName() << "\n";
	}
	std::cout << "=========================\n";
}

