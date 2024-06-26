#pragma once

#include <string>
#include <unordered_map>

class ConfigurationManager
{
public:
	ConfigurationManager();
	~ConfigurationManager();

	bool runInitialize();
	bool isInitialized();

private:
	void parseConfigFile();
	void printConfig();

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

