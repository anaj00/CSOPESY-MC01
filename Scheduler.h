#pragma once

#include <vector>
#include <memory>
#include <queue>
#include <string>

#include "Process.h"
#include "ConfigurationManager.h"

class Scheduler
{
public:
	Scheduler(const ConfigurationManager& configManager);
	~Scheduler();

	void addProcess(const Process& process);
	std::shared_ptr<Process> getProcessByName(const std::string& name);
	void listProcesses() const;

	void initialize();
	void run();
	void stop();
	void displayStatus() const;
	void saveReport() const;

private:
	std::vector<std::shared_ptr<Process>> processes;
	std::queue<std::shared_ptr<Process>> readyQueue;

	void scheduleFCFS();
	void scheduleSJF();
	void scheduleRR();

	bool running = false;

	const ConfigurationManager& configManager;

	int numCPU;
	std::string algorithm;
	int quantumCycles;
	bool preemptive;
	int batchProcessFrequency;
	int minInstructions;
	int maxInstructions;
	int delayPerExec;
};

