#pragma once

#include <string>
#include <unordered_map>

class ConfigurationManager
{
public:
	ConfigurationManager();
	~ConfigurationManager();

	bool runInitialize(); // returns true if initialization was successful
	bool isInitialized(); // returns true if the configuration manager has been initialized

private:
	void parseConfigFile();	// parses the config file and sets the configuration values
	void printConfig(); 	// prints the configuration values to the console

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

