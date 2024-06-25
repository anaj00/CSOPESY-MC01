#include "ConsoleManager.h"
#include "MainMenuScreen.h"

ConsoleManager::ConsoleManager() {
	auto MAIN_MENU = std::make_shared<MainMenuScreen>();
	consoles[MAIN_MENU->getName()] = MAIN_MENU;
	currentConsole = MAIN_MENU;
}

ConsoleManager::~ConsoleManager()
{
}

void ConsoleManager::run()
{
	if (currentConsole) {
		currentConsole->onExecute();
	}
}


