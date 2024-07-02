#include "Scheduler.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <thread>
#include <windows.h>
#include <iomanip>
#include <optional>

Scheduler::Scheduler() : running(false), isTestRunning(false) {}

Scheduler::~Scheduler() {
    stop();
    if (schedulerThread.joinable()) {
        schedulerThread.join();
    }
    for (auto& core : cores) {
        core->stop();
    }
}

void Scheduler::addProcess(const Process& process) {
    std::lock_guard<std::mutex> lock(processMutex);
    auto newProcess = std::make_shared<Process>(process);
    processes.push_back(newProcess);

    std::lock_guard<std::mutex> queueLock(queueMutex);
    readyQueue.push(newProcess);
}


std::shared_ptr<Process> Scheduler::getProcessByName(const std::string& name) {
    std::lock_guard<std::mutex> lock(processMutex);
    for (auto& process : processes) {
        if (process->getName() == name) {
            return process;
        }
    }
    return nullptr;
}

bool Scheduler::initialize(ConfigurationManager* newConfigManager) {
    try {
        configManager = newConfigManager;
        initializeCoreWorkers();
        running = true;
        return true;
    }
    catch (const std::exception& e) {
        std::cerr << "Error initializing scheduler: " << e.what() << std::endl;
        return false;
    }
}

void Scheduler::initializeCoreWorkers() {
    for (int i = 0; i < configManager->getNumCPU(); i++) {
        cores.emplace_back(std::make_unique<CoreWorker>(i + 1));
        cores.back()->start();
    }
}

void Scheduler::run() {
    schedulerThread = std::thread(&Scheduler::schedulerLoop, this);
}

void Scheduler::schedulerLoop() {
    while (running) {
        if (configManager->getSchedulerAlgorithm() == "fcfs") {
            scheduleFCFS();
        }
        else if (configManager->getSchedulerAlgorithm() == "sjf") {
            scheduleSJF();
        }
        else if (configManager->getSchedulerAlgorithm() == "rr") {
            scheduleRR();
        }
    }
}

void Scheduler::stop() {
    running = false;
}

void Scheduler::setConsoleColor(WORD attributes) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, attributes);
}

void Scheduler::printProcessList() {
    setConsoleColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
    std::cout << "| Processes:                                                     |" << std::endl;
    std::cout << "|  ID   Process Name       Current Instruction   Total Instruction   Core |" << std::endl;
    std::cout << "|====================================================================|" << std::endl;
    for (const auto& process : processes) {
        std::cout << "|  " << std::setw(0) << process->getID()
            << "   " << std::setw(10) << process->getName()
            << "   " << std::setw(15) << process->getCurrentInstruction()
            << "   " << std::setw(18) << process->getTotalInstructions()
            << "   " << std::setw(4) << process->getCore()
            << " |" << std::endl;
    }
    std::cout << "+-----------------------------------------------------------------------------+" << std::endl;
    setConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void Scheduler::displayStatus() {
    printProcessList();
}

void Scheduler::saveReport() {
    std::cout << "Saving report..." << std::endl;

    std::ofstream file("csopesy-log.txt");
    if (!file) {
        std::cerr << "Error opening file." << std::endl;
        return;
    }

    // TODO: Add report data here
}

void Scheduler::startSchedulerTest(int& ID, std::function<int()> getRandomInstruction) {
    if (!isTestRunning) {
        isTestRunning = true;
    }
    while (isTestRunning) {
        int instructionCount = getRandomInstruction();
        generateProcess(ID, instructionCount);
        std::this_thread::sleep_for(std::chrono::milliseconds(configManager->getBatchProcessFrequency() * 1000));
    }
}

void Scheduler::stopSchedulerTest() {
    isTestRunning = false;
    processTestIteration++;
}

int Scheduler::getAvailableCoreWorkerID() {
    for (auto& core : cores) {
        if (core->isAvailable()) {
            return core->getID();
        }
    }
    return 0;
}

void Scheduler::initializeFinishedProcess(std::shared_ptr<Process> process, int CoreWorkerID) {
    cores[CoreWorkerID - 1]->finishProcess();
    finishedProcesses.push_back(process);
}

void Scheduler::scheduleFCFS() {
    while (running) {
        std::lock_guard<std::mutex> lock(queueMutex);
        if (!readyQueue.empty()) {

            auto process = readyQueue.front();
            readyQueue.pop();

            auto coreID = getAvailableCoreWorkerID();

            if (coreID > 0) {
                process->setCore(coreID);
                cores[coreID - 1]->setProcess(process);

                std::thread([this, process, coreID]() {
                    cores[coreID - 1]->runProcess();
                    initializeFinishedProcess(process, coreID);
                    }).detach();
            }
            else {
                // No available core, put the process back at the front of the queue
                std::queue<std::shared_ptr<Process>> tempQueue;
                tempQueue.push(process);
                while (!readyQueue.empty()) {
                    tempQueue.push(readyQueue.front());
                    readyQueue.pop();
                }
                readyQueue = tempQueue;
            }
        }
    }
}


void Scheduler::scheduleSJF() {
    while (running) {
        std::lock_guard<std::mutex> lock(queueMutex);
        if (!readyQueue.empty()) {
            std::sort(processes.begin(), processes.end(), [](const std::shared_ptr<Process>& a, const std::shared_ptr<Process>& b) {
                return a->getTotalInstructions() < b->getTotalInstructions();
                });

            auto process = readyQueue.front();
            readyQueue.pop();

            auto coreID = getAvailableCoreWorkerID();

            if (coreID > 0) {
                process->setCore(coreID);
                cores[coreID - 1]->setProcess(process);

                while (!process->isFinished()) {
                    cores[coreID - 1]->runProcess();
                    std::this_thread::sleep_for(std::chrono::milliseconds(configManager->getDelayPerExec() * 1000));
                }
                initializeFinishedProcess(process, coreID);
            }
            else {
                // No available core, put the process back at the front of the queue
                std::queue<std::shared_ptr<Process>> tempQueue;
                tempQueue.push(process);
                while (!readyQueue.empty()) {
                    tempQueue.push(readyQueue.front());
                    readyQueue.pop();
                }
                readyQueue = tempQueue;
            }
        }
    }
}



void Scheduler::scheduleRR() {
    while (running) {
        std::lock_guard<std::mutex> lock(queueMutex);
        if (!readyQueue.empty()) {
            auto process = readyQueue.front();
            readyQueue.pop();

            auto coreID = getAvailableCoreWorkerID();

            if (coreID > 0) {
                process->setCore(coreID);
                cores[coreID - 1]->setProcess(process);

                int quantum = configManager->getQuantumCycles();
                while (quantum > 0 && !process->isFinished()) {
                    cores[coreID - 1]->runProcess();
                    quantum--;
                    std::this_thread::sleep_for(std::chrono::milliseconds(configManager->getDelayPerExec() * 1000));
                }

                if (!process->isFinished()) {
                    // Put back at the front of the queue if not finished
                    std::queue<std::shared_ptr<Process>> tempQueue;
                    tempQueue.push(process);
                    while (!readyQueue.empty()) {
                        tempQueue.push(readyQueue.front());
                        readyQueue.pop();
                    }
                    readyQueue = tempQueue;
                }
                else {
                    initializeFinishedProcess(process, coreID);
                }
            }
            else {
                // No available core, put the process back at the front of the queue
                std::queue<std::shared_ptr<Process>> tempQueue;
                tempQueue.push(process);
                while (!readyQueue.empty()) {
                    tempQueue.push(readyQueue.front());
                    readyQueue.pop();
                }
                readyQueue = tempQueue;
            }
        }
    }
}



void Scheduler::generateProcess(int& id, int instructionCount) {
    std::string processName = "SchedTest";
    processName.append("_" + std::to_string(processTestIteration));
    processName.append("_" + std::to_string(processTestNumber));

    Process newProcess(processName, id, instructionCount);
    addProcess(newProcess);

    id++;
    processTestNumber++;
}
