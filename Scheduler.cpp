#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>

#include "Scheduler.h"

Scheduler::Scheduler() {}

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

void Scheduler::getAllProcesses() {
	std::cout << "-------------------------------" << std::endl;
	std::cout << "CPU utilization: " << std::endl;
	std::cout << "Cores used: " << std::endl;
	std::cout << "Cores available: " << std::endl << std::endl;

	std::cout << "Finished: " << std::endl << std::endl;
	for (auto& process : processes) {
		if (process->isFinished()) {
			std::cout << process->getName() << std::endl;
		} 
	}
	
	std::cout << std::endl;

	std::cout << "Not finished: " << std::endl;
	for (auto& process : processes) {
		if (process -> isFinished() == false) {
			std::cout << process->getName() << std::endl;
		}
	}
}

bool Scheduler::initialize(ConfigurationManager* newConfigManager) {
	try {
		configManager = newConfigManager;
		running = true;
		return true;

	}
	catch (const std::exception& e) {
		std::cerr << "Error initializing scheduler: " << e.what() << std::endl;
		return false;
	}
}

void Scheduler::run() {
	// TODO: make this into multi core implementation
	std::cout << configManager -> getSchedulerAlgorithm() << std::endl;
	while (running) {
		if (configManager->getSchedulerAlgorithm() == "fcfs") {
			scheduleFCFS();
			std::cout << "FCFS" << std::endl;
		}

		else if (configManager->getSchedulerAlgorithm() == "sjf") {
			scheduleSJF();
			std::cout << "SJF" << std::endl;
		}

		else if (configManager->getSchedulerAlgorithm() == "rr") {
			scheduleRR();
			std::cout << "RR" << std::endl;
		}
		
		std::this_thread::sleep_for(std::chrono::milliseconds(configManager->getDelayPerExec() * 1000));
	}
}

void Scheduler::stop() {
	running = false;
}

void Scheduler::displayStatus() const {
	std::cout << "Status" << std::endl;

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

void Scheduler::startSchedulerTest(int& ID, std::function<int()> getRandomInstruction) {
	// TODO: generate function every x seconds
	// Set looping condition to true if turned off. 
	if (!isTestRunning) {
		isTestRunning = true;
	}
	while (isTestRunning) {
		// generate function
		int instructionCount = getRandomInstruction();
		generateProcess(ID, instructionCount);
		// wait 
		std::this_thread::sleep_for(std::chrono::milliseconds(configManager->getBatchProcessFrequency() * 1000));
	}
	
}


void Scheduler::stopSchedulerTest() {
	isTestRunning = false;
	processTestIteration++;
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
		std::cout << "Executing process: " << process->getName() << " for " << configManager->getQuantumCycles() << " cycles\n";
		// Assume the process executes for a quantum cycle
		process->execute();
		if (!process->isFinished()) { // If the process is not finished
			readyQueue.push(process); // Add the process back to the queue
		}
	}
}

void Scheduler::generateProcess(int& ID, int instructionCount) {
	std::string processName = "SchedTest";
	processName.append("_" + std::to_string(processTestIteration));
	processName.append("_" + std::to_string(processTestNumber));


	Process newProcess(processName, ID, instructionCount);
	addProcess(newProcess);

	ID++;
	processTestNumber++;
}



