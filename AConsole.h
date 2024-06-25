#pragma once
#include <string>

class AConsole
{
public: 
	AConsole(std::string name); // Contructor
	~AConsole(); // Destructor

	void getName(); // Function to get the name of the console
	virtual void run() = 0; // Pure virtual function to run the console

private:
	std::string name; // Name of the console
};

