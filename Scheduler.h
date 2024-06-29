#pragma once

#include <vector>
#include <memory>
#include <queue>
#include <functional>

#include "Process.h"
#include "ConfigurationManager.h"


class Scheduler
{
public:
	Scheduler();
	~Scheduler();

	void addProcess(const Process& process);
	std::shared_ptr<Process> getProcessByName(const std::string& name);
	void getAllProcesses();

	bool initialize(ConfigurationManager* newConfigManager);
	void run();
	void stop();
	void displayStatus() const;
	void saveReport() const;
	void startSchedulerTest(int& ID, std::function<int()> getRandomInstruction);
	void stopSchedulerTest();

private:
	ConfigurationManager* configManager;

	std::vector<std::shared_ptr<Process>> processes;
	std::queue<std::shared_ptr<Process>> readyQueue;

	void scheduleFCFS();
	void scheduleSJF();
	void scheduleRR();
	void generateProcess(int& ID, int instructionCount);

	bool running = false;
	bool isTestRunning = true;
	
	int processTestNumber = 0;
	int processTestIteration = 0;
	int (*getRandomInstruction)() = nullptr;
};

