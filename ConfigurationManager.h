#pragma once

#include <string>
#include <unordered_map>

class ConfigurationManager
{
public:
	ConfigurationManager();
	~ConfigurationManager();

	bool runInitialize(); // Returns true if initialization was successful
	bool isInitialized(); // Returns true if the configuration manager has been initialized

	int getMinInstructions() const; // Returns the minimum number of instructions a process can have
	int getMaxInstructions() const; // Returns the maximum number of instructions a process can have

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
};

