#include "Process.h"

Process::Process(std::string name, int id, int numInstructions) 
	: processName(name), id(id), currentInstruction(0), totalInstructions(numInstructions), isFinished(false) {}

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

bool Process::getIsFinished() const {
	return isFinished;
}

void Process::execute() {
	if (currentInstruction < totalInstructions) {
		currentInstruction++;
	} else {
		isFinished = true;
	}
}