#pragma once
#include <memory>
#include <unordered_map>

#include "AConsole.h"

class ConsoleManager
{

public:
	ConsoleManager();
	~ConsoleManager();

	void run(); // Function to run the console

private:
	std::unordered_map<std::string, std::shared_ptr<AConsole>> consoles; // Map of consoles
	std::shared_ptr<AConsole> previousConsole; // Pointer to the previous console
	std::shared_ptr<AConsole> currentConsole; // Pointer to the current console
};

