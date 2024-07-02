#include "Process.h"
#include <iostream>

Process::Process(std::string name, int id, int numInstructions) 
	: processName(name), id(id), currentInstruction(0), totalInstructions(numInstructions), finished(false) {}

std::string Process::getName() const {
	return processName;
}

int Process::getID() const {
	return id;
}

int Process::getCurrentInstruction() const {
	return currentInstruction;
}

int Process::getTotalInstructions() const {
	return totalInstructions;
}

int Process::getCore() {
	return core;
}

void Process::setCore(int coreID) {
	core = coreID;
};

bool Process::isFinished() const {
	return finished;
}

void Process::execute() {
	if (currentInstruction < totalInstructions) {
		currentInstruction++;
	} else {
		finished = true;
	}
}