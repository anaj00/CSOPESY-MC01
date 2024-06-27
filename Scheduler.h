#pragma once

#include <vector>
#include <memory>

#include "Process.h"

class Scheduler
{
public:
	Scheduler();
	~Scheduler();

	void addProcess(const Process& process);
	std::shared_ptr<Process> getProcessByName(const std::string& name);
	void listProcesses() const;

private:
	std::vector<std::shared_ptr<Process>> processes;
};

