#pragma once
#include "AConsole.h"
#include "Process.h"
#include <iostream>
#include <vector>
#include <sstream>

class ProcessScreen : public AConsole
{
public:
	ProcessScreen(std::shared_ptr<Process> myProcess);

	void onExecute() override; // Main loop
	void display() override;
	void process() override; // Handle commands

private:
	void displayHeader();
	bool handleCommand(string command);

	std::shared_ptr<Process> thisProcess;
};

