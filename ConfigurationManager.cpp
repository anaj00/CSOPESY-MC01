#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "ConfigurationManager.h"

ConfigurationManager::ConfigurationManager()
	: numCPU(0), quantumCycles(0), preemptive(false), batchProcessFrequency(0), minInstructions(0), maxInstructions(0), delayPerExec(0), maxOverallMemory(0), minMemoryPerProcess(0), maxMemoryPerProcess(0), minPagePerProcess(0), maxPagePerProcess(0), memoryManagerAlgorithm("")
{}

ConfigurationManager::~ConfigurationManager()
{}

bool ConfigurationManager::initialize()
{
	try {
		parseConfigFile(); // Parse the configuration file
		printConfig(); // Print the configuration settings
		initialized = true;
		return true;

	} catch (const std::exception& e) {
		std::cerr << "Error initializing configuration: " << e.what() << std::endl;
		return false;
	}
}

bool ConfigurationManager::isInitialized()
{
	return initialized;
}

int ConfigurationManager::getNumCPU() const {
	return numCPU;
}

std::string ConfigurationManager::getSchedulerAlgorithm() const {
	// "rr" or "fcfs" or "sjf"
	size_t firstQuote = schedulerAlgorithm.find('\"');
	size_t secondQuote = schedulerAlgorithm.find('\"', firstQuote + 1);
	return schedulerAlgorithm.substr(firstQuote + 1, secondQuote - firstQuote - 1);
}

float ConfigurationManager::getQuantumCycles() const {
	return quantumCycles;
}

bool ConfigurationManager::isPreemptive() const {
	return preemptive;
}

float ConfigurationManager::getBatchProcessFrequency() const {
	return batchProcessFrequency;
}

int ConfigurationManager::getMinInstructions() const {
	return minInstructions;
}

int ConfigurationManager::getMaxInstructions() const {
	return maxInstructions;
}

float ConfigurationManager::getDelayPerExec() const {
	return delayPerExec;
}

float ConfigurationManager::getMaxOverallMemory() const {
	return maxOverallMemory;
}

float ConfigurationManager::getMinMemoryPerProcess() const {
	return minMemoryPerProcess;
}

float ConfigurationManager::getMaxMemoryPerProcess() const {
	return maxMemoryPerProcess;
}

float ConfigurationManager::getMinPagePerProcess() const {
	return minPagePerProcess;
}

float ConfigurationManager::getMaxPagePerProcess() const {
	return maxPagePerProcess;
}

std::string ConfigurationManager::getMemoryManagerAlgorithm() const {
	std::string memoryManagerAlgorithm;
	
	if (minPagePerProcess == 1 && maxPagePerProcess == 1) {
		memoryManagerAlgorithm = "flat";
	}else {
		memoryManagerAlgorithm = "paging";
	}

	return memoryManagerAlgorithm;
}

void ConfigurationManager::parseConfigFile() {
	std::ifstream configFile("config.txt");

	if (!configFile.is_open()) {
		throw std::runtime_error("Could not open config file");
	}

	std::string line;
	while (std::getline(configFile, line)) {
		std::istringstream iss(line);
		std::string key;

		if (!(iss >> key)) {
			continue; // Skip empty lines
		}

		// find key word
		if (key == "num-cpu") {
			iss >> numCPU;

		} else if (key == "scheduler") {
			iss >> schedulerAlgorithm;

		} else if (key == "quantum-cycles"){
			iss >> quantumCycles;

		} else if (key == "preemptive") {
			int preemptiveFlag;
			iss >> preemptiveFlag;
			
			if (preemptiveFlag == 1) {
				preemptive = true;
			}
			else if (preemptiveFlag == 0) {
				preemptive = false;
			}
			else {
				throw std::runtime_error("Invalid value for preemptive flag");
			}

		}else if (key == "batch-process-freq") {
			iss >> batchProcessFrequency;

		} else if (key == "min-ins") {
			iss >> minInstructions;

		} else if (key == "max-ins") {
			iss >> maxInstructions;

		} else if (key == "delay-per-exec") {
			iss >> delayPerExec;

		} else if (key == "max-overall-mem") {
			iss >> maxOverallMemory;

		} else if (key == "min-mem-per-proc") {
			iss >> minMemoryPerProcess;

		} else if (key == "max-mem-per-proc") {
			iss >> maxMemoryPerProcess;

		} else if (key == "min-page-per-proc"){
			iss >> minPagePerProcess;

		} else if (key == "max-page-per-proc"){
			iss >> maxPagePerProcess;
		} 
	}

	configFile.close();

	// Determine memory manager algorithm
		// If min-page-per-proc and max-page-per-proc are 1, 
		// then the memory manager must use a flat memory allocator. 
		// If it’s >1, it’s a paging allocator
	if (minPagePerProcess == 1 && maxPagePerProcess == 1) {
		memoryManagerAlgorithm = "flat";
	} else {
		memoryManagerAlgorithm = "paging";
	}
}

void ConfigurationManager::printConfig() {
	std::cout << "--------------------------" << std::endl;
	std::cout << "Configuration Settings:" << std::endl;
	std::cout << "num-cpu: " << numCPU << std::endl;
	std::cout << "scheduler: " << schedulerAlgorithm << std::endl;
	std::cout << "quantum-cycles: " << quantumCycles << std::endl;
	std::cout << "preemptive: " << preemptive << std::endl;
	std::cout << "batch-process-freq: " << batchProcessFrequency << std::endl;
	std::cout << "min-ins: " << minInstructions << std::endl;
	std::cout << "max-ins: " << maxInstructions << std::endl;
	std::cout << "delay-per-exec: " << delayPerExec << std::endl;
	std::cout << "max-overall-mem: " << maxOverallMemory << std::endl;
	std::cout << "min-mem-per-proc: " << minMemoryPerProcess << std::endl;
	std::cout << "max-mem-per-proc: " << maxMemoryPerProcess << std::endl;
	std::cout << "min-page-per-proc: " << minPagePerProcess << std::endl;
	std::cout << "max-page-per-proc: " << maxPagePerProcess << std::endl;
	std::cout << "memory-manager: " << memoryManagerAlgorithm << std::endl; // "flat" or "paging"
	std::cout << "--------------------------" << std::endl;
}
