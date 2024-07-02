#pragma once
#include <memory>
#include <unordered_map>
#include <string>
#include <thread>


#include "AConsole.h"
#include "ConfigurationManager.h"
#include "Scheduler.h"

class ConsoleManager
{

public:
	ConsoleManager();
	~ConsoleManager();

	void run(); // Function to run the console
	void switchScreen(std::string consoleName); // Function to switch the console)

	bool isInitialized(); // Function to check if the console is initialized
	void setInitialized(); // Function to set the console as initialized
	ConfigurationManager& getConfigurationManager(); // Function to get the configuration manager

	const std::unordered_map<std::string, std::shared_ptr<AConsole>>& getConsoles() const; // Function to get the consoles
	void addConsole(std::shared_ptr<AConsole> console); // Function to add a new console
	void returnToPreviousScreen(); // Function to return to the previous screen

	Scheduler& getScheduler(); // Function to get the scheduler
	void createProcessScreen(const std::string processName); // Function to create a process screen)

	void startSchedulerTest();
	void stopSchedulerTest();
	
	void displayStatus();

private:
	ConfigurationManager configManager; // Configuration manager object
	Scheduler scheduler; // Scheduler object

	std::unordered_map<std::string, std::shared_ptr<AConsole>> consoles; // Map of consoles
	std::shared_ptr<AConsole> previousConsole; // Pointer to the previous console
	std::shared_ptr<AConsole> currentConsole; // Pointer to the current console

	int processID = 0; // Process ID Tracker
	int getRandomInstruction(); // Function to get a random number of instructions
};

