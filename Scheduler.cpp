#include <iostream>

#include "Scheduler.h"

Scheduler::Scheduler() {}

Scheduler::~Scheduler() {}

void Scheduler::addProcess(const Process& process) {
	processes.push_back(std::make_shared<Process>(process));
}

std::shared_ptr<Process> Scheduler::getProcessByName(const std::string& name) {
	for (auto& process : processes) {
		if (process->getName() == name) {
			return process;
		}
	}
	return nullptr;
}

void Scheduler::listProcesses() const {
	for (auto& process : processes) {
		std::cout << "Process Name: " << process->getName() << ", ID: " << process->getID() << std::endl;
	}
}



