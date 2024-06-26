#pragma once
#include <memory>
#include <unordered_map>
#include <string>

#include "AConsole.h"
#include "ConfigurationManager.h"

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

private:

	std::unordered_map<std::string, std::shared_ptr<AConsole>> consoles; // Map of consoles
	std::shared_ptr<AConsole> previousConsole; // Pointer to the previous console
	std::shared_ptr<AConsole> currentConsole; // Pointer to the current console
	ConfigurationManager configManager; // Configuration manager object
};

