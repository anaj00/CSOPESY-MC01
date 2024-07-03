#include <iostream>

#include "ProcessScreen.h"
#include "ConsoleManager.h"

extern ConsoleManager consoleManager;

ProcessScreen::ProcessScreen(std::shared_ptr<Process> myProcess)
	: AConsole("PROCESS_SCREEN_" + myProcess->getName()), thisProcess(myProcess) {
}

void ProcessScreen::onExecute() {
	display();
	process();
}

void ProcessScreen::display() {
	displayHeader();
}

void ProcessScreen::process() {
    string command;
    bool running = true;

    while (running) {
        std::cout << "> ";
        getline(cin, command);
        running = handleCommand(command);
        std::cout << endl;
    }
}

void ProcessScreen::displayHeader() {
    std::cout << "--------------------------" << endl;
    // Displays process name and ID
    std::cout << "Process: " << thisProcess->getName() << endl;
    std::cout << "ID: " << thisProcess->getID() << endl << endl;

    // Prints finished if process is done else print current line and total lines
    if (!thisProcess->isFinished()) {
        std::cout << "Current instruction line: " << thisProcess->getCurrentInstruction() << endl;
        std::cout << "Lines of code: " << thisProcess->getTotalInstructions() << endl;
        std::cout << "--------------------------" << endl;
    }
    else {
        std::cout << "Finished!" << endl;
    }
}

bool ProcessScreen::handleCommand(string command) {
    if (command == "exit") {
        consoleManager.returnToPreviousScreen();
        return false;
    }
    else if (command == "process-smi") {
        onExecute();
        return true;
    }
    else {
        std::cout << "Invalid command. Please try again." << endl;
        return true;
    }
}



