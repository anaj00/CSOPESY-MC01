#pragma once

#include <string>
#include <unordered_map>

class ConfigurationManager
{
public:
	ConfigurationManager();
	~ConfigurationManager();

	bool initialize(); // Returns true if initialization was successful
	bool isInitialized(); // Returns true if the configuration manager has been initialized

	int getNumCPU() const; // Returns the number of CPUs
	std::string getSchedulerAlgorithm() const; // Returns the scheduler algorithm
	int getQuantumCycles() const; // Returns the quantum cycles
	bool isPreemptive() const; // Returns true if the scheduler is preemptive
	int getBatchProcessFrequency() const; // Returns the batch process frequency
	int getMinInstructions() const; // Returns the minimum number of instructions a process can have
	int getMaxInstructions() const; // Returns the maximum number of instructions a process can have
	int getDelayPerExec() const; // Returns the delay per execution
	int getMaxOverallMemory() const; // Returns the maximum overall memory
	int getMinMemoryPerProcess() const; // Returns the maximum memory per process
	int getMaxMemoryPerProcess() const; // Returns the maximum memory per process


private:
	void parseConfigFile();	// Parses the config file and sets the configuration values
	void printConfig(); 	// Prints the configuration values to the console

	bool initialized = false;
		
	int numCPU;
	std::string schedulerAlgorithm;
	int quantumCycles;
	bool preemptive;
	int batchProcessFrequency;
	int minInstructions;
	int maxInstructions;
	int delayPerExec;
	int maxOverallMemory;
	int minMemoryPerProcess;
	int maxMemoryPerProcess;
};

