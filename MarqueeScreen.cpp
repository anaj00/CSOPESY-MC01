#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#include "MarqueeScreen.h"

MarqueeScreen::MarqueeScreen()
	: AConsole("MARQUEE_SCREEN") {

}

MarqueeScreen::~MarqueeScreen() {

}

void MarqueeScreen::onExecute() {
    getConsoleSize();
    process();
}

void MarqueeScreen::display() {

}

void MarqueeScreen::process() {
    int x = (screenWidth - marqueeText.length()) / 2;
    int y = screenHeight / 2;
    int dx = 1;
    int dy = 1;
    int frameDelay = 1000 / refresh_rate; // Calculate delay in milliseconds based on refresh rate
    std::string marqueeText = marqueeText;

    while (true) {
        clearConsole();
        displayHeader();
        displayFooter();
        drawOnBuffer(x, y, marqueeText, 0x0C); // Red text

        // Update position
        x += dx;
        y += dy;

        // Check for collisions and change direction if necessary
        if (x <= 0 || x + marqueeText.length() >= screenWidth) {
            dx = -dx;
        }
        if (y <= 3 || y >= screenHeight - 3) {
            dy = -dy;
        }

        // Check for user input
        if (GetAsyncKeyState(VK_BACK) & 0x1) {
            // Delete the last character from userInput
            if (!userInput.empty()) {
                userInput.pop_back();
                marqueeText = userInput;
            }
        }

        else {
            // Check for other input characters
            for (char c = 'A'; c <= 'Z'; c++) {
                if (GetAsyncKeyState(c) & 0x1) {
                    userInput += static_cast<char>(tolower(c));
                    marqueeText = userInput;
                    break;
                }
            }
            for (char c = 'a'; c <= 'z'; c++) {
                if (GetAsyncKeyState(c) & 0x1) {
                    userInput += c;
                    marqueeText = userInput;
                    break;
                }
            }
        }

        // Slow down the loop based on refresh rate
        std::this_thread::sleep_for(std::chrono::milliseconds(frameDelay));

        // Polling delay (optional, if additional logic is needed at polling intervals)
        std::this_thread::sleep_for(std::chrono::milliseconds(polling_rate));
    }
}

void MarqueeScreen::getConsoleSize() {
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi)) {
		screenWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
		screenHeight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
	} else {
		screenWidth = 80;
		screenHeight = 25;
	}
}

void MarqueeScreen::drawOnBuffer(int x, int y, const std::string text, WORD attributes) {
	COORD coord = { static_cast<SHORT>(x), static_cast<SHORT>(y) };
	DWORD written;
	std::wstring wideText = std::wstring(text.begin(), text.end()); // Convert to wide string
	WriteConsoleOutputCharacter(GetStdHandle(STD_OUTPUT_HANDLE), wideText.c_str(), static_cast<DWORD>(wideText.length()), coord, &written); // Write text
	FillConsoleOutputAttribute(GetStdHandle(STD_OUTPUT_HANDLE), attributes, static_cast<DWORD>(text.length()), coord, &written); // Set text color
}

void MarqueeScreen::displayHeader() {
	drawOnBuffer(0, 0, "***********************************", 0x0F);
	drawOnBuffer(0, 1, "Displaying a MEOWquee console!", 0x0F);
	drawOnBuffer(0, 2, "***********************************", 0x0F);
}

void MarqueeScreen::displayFooter() {
	drawOnBuffer(0, screenHeight - 2, "Meow meow we are MEOWrquee", 0x0F);
	drawOnBuffer(0, screenHeight - 1, "Type your text here: " + userInput, 0x0F);
}

void MarqueeScreen::clearConsole() {
    COORD topLeft = { 0, 0 };
    DWORD written;
    FillConsoleOutputCharacter(GetStdHandle(STD_OUTPUT_HANDLE), ' ', screenWidth * screenHeight, topLeft, &written);
    FillConsoleOutputAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 0x07, screenWidth * screenHeight, topLeft, &written);
}
