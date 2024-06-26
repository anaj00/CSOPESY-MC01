#pragma once
#include <string>

using namespace std;

class AConsole
{
public: 
	AConsole(string name); // Contructor
	~AConsole(); // Destructor

	string getName(); // Function to get the name of the console

	virtual void onExecute() = 0; // Pure virtual function to execute the console
	virtual void display() = 0; // Pure virtual function to display the console
	virtual void process() = 0; // Pure virtual function to process the console

private:
	std::string name; // Name of the console
};

