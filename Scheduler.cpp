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


std::shared_ptr<Process> Scheduler::getProcessByName(const std::string name) {
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
            if (configManager->isPreemptive()) {
				schedulePreemptiveSJF();
			}
			else {
				scheduleNonPreemptiveSJF();
			}
        }
        else if (configManager->getSchedulerAlgorithm() == "rr") {
            scheduleRR();
        }

        std::this_thread::sleep_for(std::chrono::duration<float>(configManager->getDelayPerExec()));
    }
}

void Scheduler::stop() {
    running = false;
}

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
                << std::left << std::setw(30) << process->getCreationTime();

            // Check if the process has been assigned a core
            if (process->getCore() != -1) {
                std::cout << "Core:   " << std::setw(15) << process->getCore();
                std::cout << std::left << std::setw(1) << process->getCurrentInstruction() << " / "
                    << process->getTotalInstructions() << "\n";
            }
            else {
                std::cout << "Core:   " << std::setw(15) << " "; // Adjust the width to maintain alignment
                std::cout << std::left << std::setw(1) << process->getCurrentInstruction() << " / "
                    << process->getTotalInstructions() << "\n";
            }
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

    for (const auto& process : processes) {
        if (!process->isFinished()) {
            file << std::left << std::setw(20) << process->getName()
                << std::left << std::setw(30) << process->getCreationTime();

            // Check if the process has been assigned a core
            if (process->getCore() != -1) {
                file << "Core:   " << std::setw(15) << process->getCore();
                file << std::left << std::setw(1) << process->getCurrentInstruction() << " / "
                    << process->getTotalInstructions() << "\n";
            }
            else {
                file << "Core:   " << std::setw(15) << " "; // Adjust the width to maintain alignment
                file << std::left << std::setw(1) << process->getCurrentInstruction() << " / "
                    << process->getTotalInstructions() << "\n";
            }
        }
    }

    file << "\nFinished processes:\n";

    for (const auto& process : processes) {
        if (process->isFinished()) {
            file << std::left << std::setw(20) << process->getName()
                << std::left << std::setw(30) << process->getCreationTime()
                << "Core:   " << std::setw(15) << process->getCore()
                << std::left << std::setw(1) << process->getCurrentInstruction() << " / "
                << process->getTotalInstructions() << "\n";
        }
    }

    file << "--------------------------------------------\n";

    std::cout << "Report saved at csopesy-log.txt!" << std::endl;
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

void Scheduler::scheduleNonPreemptiveSJF() {
    while (running) {
        std::lock_guard<std::mutex> lock(queueMutex);
        if (!readyQueue.empty()) {

            // Move processes to a vector for sorting
            std::vector<std::shared_ptr<Process>> sortedProcesses;
            while (!readyQueue.empty()) {
                sortedProcesses.push_back(readyQueue.front());
                readyQueue.pop();
            }

            // Sort processes by total instructions (burst time)
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

void Scheduler::schedulePreemptiveSJF() {
    while (running) {
        std::lock_guard<std::mutex> lock(queueMutex);
        if (!readyQueue.empty()) {

            // Sort the ready queue by remaining instructions
            std::vector<std::shared_ptr<Process>> sortedProcesses;
            while (!readyQueue.empty()) {
                sortedProcesses.push_back(readyQueue.front());
                readyQueue.pop();
            }

            std::sort(sortedProcesses.begin(), sortedProcesses.end(), [](const std::shared_ptr<Process>& a, const std::shared_ptr<Process>& b) {
                return a->getRemainingInstructions() < b->getRemainingInstructions();
                });

            // Assign sorted processes to available cores or preempt if necessary
            for (auto& process : sortedProcesses) {
                auto coreID = getAvailableCoreWorkerID();

                if (coreID > 0) {
                    process->setCore(coreID);
                    cores[coreID - 1]->setProcess(process);
                }
                else {
                    // Check for preemption
                    bool preempted = false;
                    for (auto& core : cores) {
                        auto runningProcess = core->getCurrentProcess();
                        if (runningProcess && runningProcess->getRemainingInstructions() > process->getRemainingInstructions()) {
                            
                            // Preempt the current process
                            readyQueue.push(runningProcess);
                            process->setCore(core->getID());
                            core->setProcess(process);
                            preempted = true;
                            break;
                        }
                    }

                    if (!preempted) {
                        readyQueue.push(process); // Put back in the ready queue if no preemption occurred
                    }
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
