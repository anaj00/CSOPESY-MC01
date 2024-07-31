#pragma once
#include <memory>
#include <unordered_map>
#include <string>
#include <thread>


#include "AConsole.h"
#include "ConfigurationManager.h"
#include "Scheduler.h"
#include "ProcessScreen.h"
#include "ResourceManager.h"

class ConsoleManager
{

public:
	ConsoleManager();
	~ConsoleManager();

	void run(); // Function to run the console
	void switchScreen(std::string consoleName); // Function to switch the console)

	void setInitialized(); // Function to set the console as initialized
	ConfigurationManager& getConfigurationManager(); // Function to get the configuration manager

	const std::unordered_map<std::string, std::shared_ptr<AConsole>>& getConsoles() const; // Function to get the consoles
	void addConsole(std::shared_ptr<AConsole> console); // Function to add a new console
	void returnToPreviousScreen(); // Function to return to the previous screen

	ResourceManager& getResourceManager(); // Function to get the scheduler
	void createProcessScreen(const std::string processName); // Function to create a process screen)

	void displayStatus();

	bool ifProcessExists(std::string name);
	bool isProcessFinished(std::string name);

private:
	ConfigurationManager configManager; // Configuration manager object
	ResourceManager resourceManager; // Resource manager object

	std::unordered_map<std::string, std::shared_ptr<AConsole>> consoles; // Map of consoles
	std::shared_ptr<AConsole> previousConsole; // Pointer to the previous console
	std::shared_ptr<AConsole> currentConsole; // Pointer to the current console

	int processID = 0; // Process ID Tracker
	std::thread testThread;
	std::mutex mtx;

	
};

