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
        std::cout << "root:\\> ";
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

    std::cout << "Hello. Welcome to CSOPESY command line interface." << endl << endl;
}

void MainMenuScreen::handleCommand(string command) {
    if (command == "exit") {
        std::cout << "Exiting the program..." << std::endl;
        exit(0);
        std::terminate();

    } else if (command == "initialize" && !consoleManager.getConfigurationManager().isInitialized()) {
        consoleManager.setInitialized();

	} else if (consoleManager.getConfigurationManager().isInitialized()) {
        if (command == "clear" || command == "cls") {
            system("cls");

        } else if (command == "marquee") {
            consoleManager.switchScreen("MARQUEE_SCREEN");

        } else if (command.substr(0,6) == "screen") {
            if (command == "screen -ls") {
                consoleManager.displayStatus();
                consoleManager.getResourceManager().getMemoryManager()->getBackingStore()->displayContents();
            }

            else if (command.substr(0,9) == "screen -s") { 
                std::string processName = command.substr(10);
                trim(processName);
                if (processName.empty()){ // if the process name is empty
                    std::cout << "Command not recognized! Please provide a process name." << std::endl;
                }

                else { // if the process name is not empty
                    if (!consoleManager.getResourceManager().processExists(processName)) { // if the process does not exist, create process
                        consoleManager.createProcessScreen(processName);
                    }
                    else { // if the process already exists
                        std::cout << "Process already exists or has existed. Please provide a different name." << std::endl;
                    }
                   
                }
            }

            else if (command.substr(0,9) == "screen -r") {
                std::string processName = command.substr(10);
                trim(processName);

                if (processName.empty()) { // if the process name is empty
                    std::cout << "Command not recognized! Please provide a process name." << std::endl;
                }

                else {
                    if (consoleManager.isProcessFinished(processName)) { // if the process is finished, there will be no screen
                        std::cout << "Process " << processName << " not found." << std::endl;
                    }
                    else { // if the process is not finished, switch to the screen
                        consoleManager.switchScreen("PROCESS_SCREEN_" + processName);
                    }
                }
            }

        } else if (command.substr(0, 9) == "scheduler") {
            if (command.substr(10) == "test") { 
                consoleManager.getResourceManager().startSchedulerTest();
            }

            else if (command.substr(10) == "stop") { 
                consoleManager.getResourceManager().stopSchedulerTest();
            } 

            else {
                std::cout << "Invalid command.Please try again." << std::endl;
            }

        } else if (command == "report-util") {
            // TODO: Implement new report utility with memory details
            consoleManager.getResourceManager().saveReport();

		} else if (command == "help") {
			std::cout << "\nCommands:" << std::endl;
			std::cout << "clear/cls - Clear the screen" << std::endl;
			std::cout << "marquee - Switch to marquee screen" << std::endl;
			std::cout << "screen -ls - List all screens" << std::endl;
			std::cout << "screen -s [processName] - Create a new process screen" << std::endl;
			std::cout << "scheduler-test - Start the scheduler test" << std::endl;
			std::cout << "scheduler-stop - Stop the scheduler test" << std::endl;
			std::cout << "report-util - Save the report" << std::endl;
			std::cout << "exit - Exit the program" << std::endl;
		} else if (command == "process-smi") {
            consoleManager.getResourceManager().displayProcessSmi();
        } else if (command == "vmstat") {
            consoleManager.getResourceManager().displayVMStat();
        }

        else {
            std::cout << "Invalid command. Please try again." << std::endl;
        }

    } else {
        std::cout << "You must initialize the program first." << std::endl;
    }
}

void MainMenuScreen::ltrim(std::string& s) {
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](int ch) {
        return !std::isspace(ch);
        }));
}

void MainMenuScreen::rtrim(std::string& s) {
    s.erase(std::find_if(s.rbegin(), s.rend(), [](int ch) {
        return !std::isspace(ch);
        }).base(), s.end());
}

void MainMenuScreen::trim(std::string& s) {
    ltrim(s);
    rtrim(s);
}