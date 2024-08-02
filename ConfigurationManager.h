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
	int getMinInstructions() const; // Returns the minimum number of instructions a process can have
	int getMaxInstructions() const; // Returns the maximum number of instructions a process can have
	float getDelayPerExec() const; // Returns the delay per execution
	float getMaxOverallMemory() const; // Returns the maximum overall memory
	float getMinMemoryPerProcess() const; // Returns the minimum memory per process
	float getMaxMemoryPerProcess() const; // Returns the maximum memory per process
	float getMinPagePerProcess() const; // Returns the minimum number of pages per process
	float getMaxPagePerProcess() const; // Returns the maximum number of pages per process
	std::string getMemoryManagerAlgorithm() const; // Returns the memory manager algorithm
	float getMaxFrames() const; // Returns the maximum number of frames

private:
	void parseConfigFile();	// Parses the config file and sets the configuration values
	void printConfig(); 	// Prints the configuration values to the console

	bool initialized = false;
		
	int numCPU;
	std::string schedulerAlgorithm;
	float quantumCycles;
	bool preemptive;
	float batchProcessFrequency;
	int minInstructions;
	int maxInstructions;
	float delayPerExec;
	float maxOverallMemory;
	float minMemoryPerProcess;
	float maxMemoryPerProcess;
	float minPagePerProcess;
	float maxPagePerProcess;
	std::string memoryManagerAlgorithm;

	float maxFrames;

	int getRandomInt2N(int min, int max); // Randomizer for the process details wherein number = 2^n
};

