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
	if (consoleName.substr(0, 15) == "PROCESS_SCREEN_") {
		std::string processName = consoleName.substr(15);
		std::shared_ptr<Process> process = scheduler.getProcessByName(processName);

		if (process) {
			if (!process->isFinished()) {
				previousConsole = currentConsole;
				currentConsole = consoles[consoleName];
				system("cls");
				currentConsole->onExecute();
			}
			else {
				std::cerr << "Process " << processName << " has already finished." << std::endl;
			}
		}
		else {
			std::cerr << "Process " << processName << " not found." << std::endl;
		}
	}
	else {
		if (consoles.find(consoleName) != consoles.end()) {
			previousConsole = currentConsole;
			currentConsole = consoles[consoleName];
			system("cls");
			currentConsole->onExecute();
		}
		else {
			std::cerr << "Console " << consoleName << " not found." << std::endl;
		}
	}
}

bool ConsoleManager::isInitialized() {
	return configManager.isInitialized();
}

void ConsoleManager::setInitialized() {
	if (configManager.initialize() && scheduler.initialize(&configManager)) {
		std::cout << "Initialization successful..." << std::endl;
		std::thread schedulerThread(&Scheduler::run, &scheduler);
		schedulerThread.detach();
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
	std::cout << newProcess.getName() << "\n";
	std::cout << newProcess.getID() << "\n";
	std::cout << newProcess.getTotalInstructions() << "\n";
	
	std::shared_ptr<Process> processPointer = scheduler.addProcess(newProcess);

	// Create new process screen
	auto processScreen = std::make_shared<ProcessScreen>(processPointer);

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

void ConsoleManager::startSchedulerTest() {
	std::thread schedulerThread([this]() {
		this->scheduler.startSchedulerTest(this->processID, [this]() {
			return this->getRandomInstruction();
			});
		});
	schedulerThread.detach();
}


void ConsoleManager::stopSchedulerTest() {
	scheduler.stopSchedulerTest();
}

void ConsoleManager::displayStatus() {
	scheduler.displayStatus();	
}

bool ConsoleManager::ifProcessExists(std::string name) {
	bool flag;
	if (scheduler.getProcessByName(name) == nullptr) return false;
	return true;
}
bool ConsoleManager::isProcessFinished(std::string name) {
	if (ifProcessExists(name)) {
		std::shared_ptr<Process> foundProcess = scheduler.getProcessByName(name);
		return foundProcess->isFinished();
	}
	return false;
}
