#include <iostream>

#include "MainMenuScreen.h"
#include "ConsoleManager.h"

extern ConsoleManager consoleManager;

MainMenuScreen::MainMenuScreen()
	: AConsole("MAIN_MENU"){

}

MainMenuScreen::~MainMenuScreen(){

}

void MainMenuScreen::onExecute(){
	display();
	process();
}

void MainMenuScreen::display(){
	displayHeader();
}

void MainMenuScreen::process() {
    string command;
    while (true) {
        std::cout << "> ";
        getline(cin, command);
        handleCommand(command);
        std::cout << endl;
    }
}

void MainMenuScreen::displayHeader(){
    std::cout << R"(
                     _________   _________________ _____________________ ______________.___.
                     \_   ___ \ /   _____/\_____  \\______   \_   _____//   _____/\__  |   |
                     /    \  \/ \_____  \  /   |   \|     ___/|    __)_ \_____  \  /   |   |
                     \     \____/        \/    |    \    |    |        \/        \ \____   |
                      \______  /_______  /\_______  /____|   /_______  /_______  / / ______|
                             \/        \/         \/                 \/        \/  \/       
    )" << endl;

    std::cout << "Hello. Welcome to CSOPESY command line interface.\n";
}

void MainMenuScreen::handleCommand(string command) {
    if (command == "exit") {
        std::cout << "Exiting the program..." << std::endl;
        exit(0);

    } else if (command == "initialize" && !consoleManager.isInitialized()) {
        consoleManager.setInitialized();

	} else if (consoleManager.isInitialized()) {
        if (command == "clear" || command == "cls") {
            system("cls");

        } else if (command == "marquee") {
            consoleManager.switchScreen("MARQUEE_SCREEN");

        } else if (command.substr(0,6) == "screen") {
            if (command == "screen -ls") {
                std::cout << "I am screen -ls!";
            }

            else if (command.substr(0,9) == "screen -s") {
                std::string processName = command.substr(10);
                consoleManager.createProcessScreen(processName);
            }

        } else {
            std::cout << "Invalid command. Please try again.\n";
        }

    } else {
        std::cout << "You must initialize the program first.\n";
    }
}