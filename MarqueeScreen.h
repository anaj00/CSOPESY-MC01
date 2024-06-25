#pragma once
#include <iostream>;

#include "AConsole.h";
#include <Windows.h>

class MarqueeScreen : public AConsole
{
public:
	MarqueeScreen();
	~MarqueeScreen();

	void onExecute() override;
	void display() override;
	void process() override;

private:
	int screenWidth;
	int screenHeight;

	const int refresh_rate = 60;
	const int polling_rate = 1;
	const std::string marqueeText = "Hello World!";
	
	std::string userInput;
	void getConsoleSize();
	void drawOnBuffer(int x, int y, const std::string text, WORD attributes);
	void displayHeader();
	void displayFooter();
	void clearConsole();
};

