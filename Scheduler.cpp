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

std::shared_ptr<Process> Scheduler::addProcess(const Process& process) {
    std::lock_guard<std::mutex> lock(processMutex);
    auto newProcess = std::make_shared<Process>(process);
    processes.push_back(newProcess);

    std::lock_guard<std::mutex> queueLock(queueMutex);
    readyQueue.push(newProcess);

    return newProcess;
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
        if (configManager->getSchedulerAlgorithm() == "rr") {
            cores.emplace_back(std::make_unique<CoreWorker>(i + 1, configManager->getDelayPerExec(), configManager->getQuantumCycles()));
            cores.back()->start();
        }

        else {
            cores.emplace_back(std::make_unique<CoreWorker>(i + 1, configManager->getDelayPerExec()));
            cores.back()->start();
        }
        
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

//void Scheduler::setConsoleColor(WORD attributes) {
//    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
//    SetConsoleTextAttribute(hConsole, attributes);
//}

void Scheduler::displayStatus() {
    std::lock_guard<std::mutex> lock(processMutex);

    int coresUsed = 0;
    for (const auto& core : cores) {
        if (core->isAssignedProcess()) {
            coresUsed++;
        }
    }

    int totalCores = cores.size();
    int cpuUtilization = totalCores ? (coresUsed * 100 / totalCores) : 0;

    std::cout << "CPU utilization: " << cpuUtilization << "%\n";
    std::cout << "Cores used: " << coresUsed << "\n";
    std::cout << "Cores available: " << totalCores - coresUsed << "\n";
    std::cout << "--------------------------------------------\n";

    std::cout << "Running processes:\n";

    for (const auto& process : processes) {
        if (!process->isFinished()) {
            std::cout << std::left << std::setw(20) << process->getName()
                << std::left << std::setw(30) << process->getCreationTime()
                << "Core:   " << std::setw(15) << process->getCore()
                << std::left << std::setw(1) << process->getCurrentInstruction() << " / "
                << process->getTotalInstructions() << "\n";
        }
    }

    std::cout << "\nFinished processes:\n";

    for (const auto& process : processes) {
        if (process->isFinished()) {
            std::cout << std::left << std::setw(20) << process->getName()
                << std::left << std::setw(30) << process->getCreationTime()
                << "Core:   " << std::setw(15) << process->getCore()
                << std::left << std::setw(1) << process->getCurrentInstruction() << " / "
                << process->getTotalInstructions() << "\n";
        }
    }

    std::cout << "--------------------------------------------\n";
}


void Scheduler::saveReport() {
    std::cout << "Saving report..." << std::endl;

    std::ofstream file("csopesy-log.txt");
    if (!file) {
        std::cerr << "Error opening file." << std::endl;
        return;
    }

    std::lock_guard<std::mutex> lock(processMutex);

    int coresUsed = 0;
    for (const auto& core : cores) {
        if (core->isAssignedProcess()) {
            coresUsed++;
        }
    }

    int totalCores = cores.size();
    int cpuUtilization = totalCores ? (coresUsed * 100 / totalCores) : 0;

    file << "CPU utilization: " << cpuUtilization << "%\n";
    file << "Cores used: " << coresUsed << "\n";
    file << "Cores available: " << totalCores - coresUsed << "\n";
    file << "--------------------------------------------\n";

    file << "Running processes:\n";

    // Column headers
    file << std::left << std::setw(20) << "Process"
        << std::left << std::setw(30) << "Creation Time"
        << std::left << std::setw(15) << "Core"
        << std::left << std::setw(15) << "Progress" << "\n";

    for (const auto& process : processes) {
        if (!process->isFinished()) {
            file << std::left << std::setw(20) << process->getName()
                << std::left << std::setw(30) << process->getCreationTime()
                << "Core: " << std::setw(5) << process->getCore()
                << std::left << std::setw(5) << process->getCurrentInstruction() << " / "
                << process->getTotalInstructions() << "\n";
        }
    }

    file << "\nFinished processes:\n";

    // Column headers
    file << std::left << std::setw(20) << "Process"
        << std::left << std::setw(30) << "Creation Time"
        << std::left << std::setw(15) << "Core"
        << std::left << std::setw(15) << "Progress" << "\n";

    for (const auto& process : processes) {
        if (process->isFinished()) {
            file << std::left << std::setw(20) << process->getName()
                << std::left << std::setw(30) << process->getCreationTime()
                << "Core: " << std::setw(5) << process->getCore()
                << std::left << std::setw(5) << process->getCurrentInstruction() << " / "
                << process->getTotalInstructions() << "\n";
        }
    }

    file << "--------------------------------------------\n";

    file.close();

    std::cout << "Report saved to csopesy-log.txt" << std::endl;
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

            // Sort the ready queue by total instructions
            std::vector<std::shared_ptr<Process>> sortedProcesses;
            while (!readyQueue.empty()) {
                sortedProcesses.push_back(readyQueue.front());
                readyQueue.pop();
            }

            std::sort(sortedProcesses.begin(), sortedProcesses.end(), [](const std::shared_ptr<Process>& a, const std::shared_ptr<Process>& b) {
                return a->getTotalInstructions() < b->getTotalInstructions();
            });

            // Assign sorted processes to available cores
            for (auto& process : sortedProcesses) {
                auto coreID = getAvailableCoreWorkerID();

                if (coreID > 0) {
                    process->setCore(coreID);
                    cores[coreID - 1]->setProcess(process);
                }
                else {
                    readyQueue.push(process); // Put back in the ready queue if no core is available
                }
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

                // Use a lambda function to handle requeueing the process after execution
                cores[coreID - 1]->setProcessCompletionCallback([this](std::shared_ptr<Process> completedProcess) {
                    if (!completedProcess->isFinished()) {
                        std::lock_guard<std::mutex> queueLock(this->queueMutex);
                        this->readyQueue.push(completedProcess);
                    }
                    else {
                        std::lock_guard<std::mutex> processLock(this->processMutex);
                        this->finishedProcesses.push_back(completedProcess);
                    }
                    });
            }
            else {
                // No available core, put the process back at the front of the queue
                readyQueue.push(process);
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
