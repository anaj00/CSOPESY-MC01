#include "ConsoleManager.h"
#include "MainMenuScreen.h"
#include "MarqueeScreen.h"
#include "ConfigurationManager.h"

ConsoleManager::ConsoleManager() {
	auto MAIN_MENU = std::make_shared<MainMenuScreen>();
	consoles[MAIN_MENU->getName()] = MAIN_MENU;

	auto MARQUEE_SCREEN = std::make_shared<MarqueeScreen>();
	consoles[MARQUEE_SCREEN->getName()] = MARQUEE_SCREEN;

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

void ConsoleManager::switchScreen(const std::string consoleName) {
	if (consoles.find(consoleName) != consoles.end()) {
		previousConsole = currentConsole;
		currentConsole = consoles[consoleName];
		system("cls");
		currentConsole->onExecute();

	} else {
		std::cerr << "Console " << consoleName << " not found." << std::endl;
	}
}

bool ConsoleManager::isInitialized() {
	return configManager.isInitialized();
}

void ConsoleManager::setInitialized() {
	if (configManager.runInitialize()) {
		std::cout << "Initialization successful..." << std::endl;
	}
}

ConfigurationManager& ConsoleManager::getConfigurationManager() {
	return configManager;
}


