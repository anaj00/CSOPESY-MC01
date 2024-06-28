#include <iostream>

#include <fstream>
#include <sstream>
#include <thread>

#include "Scheduler.h"
#include "ConfigurationManager.h"

extern ConfigurationManager configManager;

Scheduler::Scheduler(const ConfigurationManager& configManager)
	: configManager(configManager), running(false) {}

Scheduler::~Scheduler() {}

void Scheduler::addProcess(const Process& process) {
	processes.push_back(std::make_shared<Process>(process));
}

std::shared_ptr<Process> Scheduler::getProcessByName(const std::string& name) {
	for (auto& process : processes) {
		if (process->getName() == name) {
			return process;
		}
	}
	return nullptr;
}

void Scheduler::listProcesses() const {
	for (auto& process : processes) {
		std::cout << "Process Name: " << process->getName() << ", ID: " << process->getID() << std::endl;
	}
}

void Scheduler::initialize() {
	if (!configManager.isInitialized()) {
		std::cout << "Configuration Manager is not initialized. Please initialize it first." << std::endl;
		return;
	}

	numCPU = configManager.getNumCPU();
	algorithm = configManager.getSchedulerAlgorithm();
	quantumCycles = configManager.getQuantumCycles();
	preemptive = configManager.isPreemptive();
	batchProcessFrequency = configManager.getBatchProcessFrequency();
	minInstructions = configManager.getMinInstructions();
	maxInstructions = configManager.getMaxInstructions();
	delayPerExec = configManager.getDelayPerExec();

	running = true;
}

void Scheduler::run() {
	while (running) {
		if (configManager.getSchedulerAlgorithm() == "fcfs") {
			scheduleFCFS();

		}

		else if (configManager.getSchedulerAlgorithm() == "sjf") {
			scheduleSJF();
		}

		else if (configManager.getSchedulerAlgorithm() == "rr") {
			scheduleRR();
		}
	}

	std::this_thread::sleep_for(std::chrono::milliseconds(delayPerExec * 1000));
}

void Scheduler::stop() {
		running = false;
}

void Scheduler::displayStatus() const {
	std::cout << "Status: " << std::endl;

	// TODO: Add status data here
}

void Scheduler::saveReport() const {
	std::cout << "Saving report..." << std::endl;

	std::ofstream file("csopesy-log.txt");
	if (!file) {
		std::cerr << "Error opening file." << std::endl;
		return;
	}

	// TODO: Add report data here
}

void Scheduler::scheduleFCFS() {
	if (!readyQueue.empty()) {
        auto process = readyQueue.front(); // Get the first process in the queue
        readyQueue.pop(); // Remove the process from the queue
        // Simulate process execution
        std::cout << "Executing process: " << process->getName() << "\n";
        // Assume the process is finished for simplicity
        process->execute(); 
    }
}

void Scheduler::scheduleSJF() {
	if (!readyQueue.empty()) {
		std::sort(processes.begin(), processes.end(), [](const std::shared_ptr<Process>& a, const std::shared_ptr<Process>& b) {
			return a->getTotalInstructions() < b->getTotalInstructions();
			}); // Sort the processes by total instructions
		auto process = readyQueue.front(); // Get the first process in the queue
		readyQueue.pop(); // Remove the process from the queue
		// Simulate process execution
		std::cout << "Executing process: " << process->getName() << "\n";
		// Assume the process is finished for simplicity
		process->execute();
	}
}

void Scheduler::scheduleRR() {
	if (!readyQueue.empty()) {
		auto process = readyQueue.front(); // Get the first process in the queue
		readyQueue.pop(); // Remove the process from the queue
		// Simulate process execution
		std::cout << "Executing process: " << process->getName() << " for " << quantumCycles << " cycles\n";
		// Assume the process executes for a quantum cycle
		process->execute(); 
		if (!process->isFinished()) { // If the process is not finished
			readyQueue.push(process); // Add the process back to the queue
		}
	}
}

