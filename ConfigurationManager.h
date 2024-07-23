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
	float getQuantumCycles() const; // Returns the quantum cycles
	bool isPreemptive() const; // Returns true if the scheduler is preemptive
	float getBatchProcessFrequency() const; // Returns the batch process frequency
	float getMinInstructions() const; // Returns the minimum number of instructions a process can have
	float getMaxInstructions() const; // Returns the maximum number of instructions a process can have
	float getDelayPerExec() const; // Returns the delay per execution
	float getMaxOverallMemory() const; // Returns the maximum overall memory
	float getMinMemoryPerProcess() const; // Returns the minimum memory per process
	float getMaxMemoryPerProcess() const; // Returns the maximum memory per process

private:
	void parseConfigFile();	// Parses the config file and sets the configuration values
	void printConfig(); 	// Prints the configuration values to the console

	bool initialized = false;
		
	int numCPU;
	std::string schedulerAlgorithm;
	float quantumCycles;
	bool preemptive;
	float batchProcessFrequency;
	float minInstructions;
	float maxInstructions;
	float delayPerExec;
	float maxOverallMemory;
	float minMemoryPerProcess;
	float maxMemoryPerProcess;
};

