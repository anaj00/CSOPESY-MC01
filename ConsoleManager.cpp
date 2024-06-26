#include "ConsoleManager.h"
#include "MainMenuScreen.h"
#include "MarqueeScreen.h"
#include "ConfigurationManager.h"
#include "Scheduler.h"
#include "Process.h"
#include "ProcessScreen.h"

#include <random>

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
	if (configManager.initialize() && scheduler.initialize(&configManager)) {
		std::cout << "Initialization successful..." << std::endl;
	}
}

ConfigurationManager& ConsoleManager::getConfigurationManager() {
	return configManager;
}

const std::unordered_map<std::string, std::shared_ptr<AConsole>>& ConsoleManager::getConsoles() const {
	return consoles;
}

void ConsoleManager::addConsole(std::shared_ptr<AConsole> console) {
	consoles[console->getName()] = console;
}

void ConsoleManager::returnToPreviousScreen() {
	if (previousConsole) {
		currentConsole = previousConsole;
		previousConsole = nullptr;
		system("cls");
		currentConsole->onExecute();
	}
	else {
		std::cerr << "No previous screen to return to." << std::endl;
	}
}

Scheduler& ConsoleManager::getScheduler() {
	return scheduler;
}

void ConsoleManager::createProcessScreen(const std::string processName) {
	processID++;

	// Create new process
	Process newProcess(processName, processID, getRandomInstruction());
	scheduler.addProcess(newProcess);

	// Create new process screen
	auto processScreen = std::make_shared<ProcessScreen>(newProcess);

	// Add process screen to console manager
	addConsole(processScreen);

	// Switch to process screen
	switchScreen(processScreen->getName());
}

int ConsoleManager::getRandomInstruction() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> distr(configManager.getMinInstructions(), configManager.getMaxInstructions());
	
	return distr(gen);
}


