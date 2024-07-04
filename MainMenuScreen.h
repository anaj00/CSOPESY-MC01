#pragma once


#include "AConsole.h"

class MainMenuScreen : public AConsole
{
public:
	MainMenuScreen();
	~MainMenuScreen();

	void onExecute() override;
	void display() override;
	void process() override;

private:
	void displayHeader();
	void handleCommand(string command);
	void ltrim(std::string& s);
	void rtrim(std::string& s);
	void trim(std::string& s);
};

