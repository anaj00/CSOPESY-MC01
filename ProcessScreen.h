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

	void onExecute() override;
	void display() override;
	void process() override;

private:
	void displayHeader();
	bool handleCommand(string command);

	std::shared_ptr<Process> thisProcess;
};

