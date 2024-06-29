#pragma once

#include <vector>
#include <memory>
#include <queue>

#include "Process.h"
#include "ConfigurationManager.h"

class Scheduler
{
public:
	Scheduler();
	~Scheduler();

	void addProcess(const Process& process);
	std::shared_ptr<Process> getProcessByName(const std::string& name);

	bool initialize(ConfigurationManager* newConfigManager);
	void run();
	void stop();
	void displayStatus() const;
	void saveReport() const;

private:
	ConfigurationManager* configManager;

	std::vector<std::shared_ptr<Process>> processes;
	std::queue<std::shared_ptr<Process>> readyQueue;

	void scheduleFCFS();
	void scheduleSJF();
	void scheduleRR();

	bool running = false;
};

