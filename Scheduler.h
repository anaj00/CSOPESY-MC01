#pragma once

#include <vector>
#include <memory>
#include <queue>
#include <functional>
#include <thread>
#include <mutex>
#include <windows.h>

#include "Process.h"
#include "ConfigurationManager.h"
#include "CoreWorker.h"

class Scheduler
{
public:
    Scheduler();
    ~Scheduler();

    std::shared_ptr<Process> addProcess(const Process& process);
    std::shared_ptr<Process> getProcessByName(const std::string name);

    bool initialize(ConfigurationManager* newConfigManager);
    void run();
    void stop();

    void displayStatus();
    void saveReport();

private:
    ConfigurationManager* configManager;

    std::vector<std::unique_ptr<CoreWorker>> cores; // All cores
    std::vector<std::shared_ptr<Process>> processes; // All processes regardless of state
    std::queue<std::shared_ptr<Process>> readyQueue; // All processes ready to go once a thread yields
    std::vector<std::shared_ptr<Process>> finishedProcesses; // Add finished processes here
    std::mutex processMutex; // Protects access to the processes vector
    std::mutex queueMutex;   // Protects access to the readyQueue

    bool running;
    bool isTestRunning;

    void initializeCoreWorkers();
    int getAvailableCoreWorkerID();

    void scheduleFCFS();
    void scheduleNonPreemptiveSJF();
    void schedulePreemptiveSJF();
    void scheduleRR();

    std::thread schedulerThread;
    void schedulerLoop();
};
