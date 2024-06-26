#include "ProcessScreen.h"

ProcessScreen::ProcessScreen(Process myProcess)
	//TODO: Pass Process Object and format a string for the process name
	: AConsole("PROCESS_SCREEN_" + myProcess.getName()), thisProcess(myProcess) {
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
    while (true) {
        std::cout << "> ";
        getline(cin, command);
        handleCommand(command);
        std::cout << endl;
    }
}

void ProcessScreen::displayHeader() {
    //Displays process name and ID
    std::cout << "Process: " << thisProcess.getName() << endl;
    std::cout << "ID: " << thisProcess.getID() << endl << endl;

    //Prints finished if process is done else print current line and total lines
    if (!thisProcess.getIsFinished()) {
        std::cout << "Current instruction line: " << thisProcess.getCurrentInstruction() << endl;
        std::cout << "Lines of code: " << thisProcess.getTotalInstructions() << endl << endl;
    }
    else {
        std::cout << "Finished!" << endl;
    }
}

void ProcessScreen::handleCommand(string command) {
    if (command == "exit") {
        //TODO: return user to main menu
        //Not sure if this is working already please check out
        return;
    }
    else if (command == "process-smi") {
        onExecute();
    }
    else {
        std::cout << "Invalid command. Please try again." << endl;
    }
}



