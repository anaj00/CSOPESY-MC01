#pragma once

#include <vector>
#include <memory>
#include <queue>

#include "Process.h"
#include "ConfigurationManager.h"
#include "ConsoleManager.h"

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
	void startSchedulerTest();
	void stopSchedulerTest();

private:
	ConfigurationManager* configManager;

	std::vector<std::shared_ptr<Process>> processes;
	std::queue<std::shared_ptr<Process>> readyQueue;

	void scheduleFCFS();
	void scheduleSJF();
	void scheduleRR();
	void generateProcess();

	bool running = false;
	bool isTestRunning = true;
	
	int processTestNumber = 0;
	int processTestIteration = 0;
};

