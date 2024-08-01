#pragma once

#include <thread>
#include <memory>
#include <mutex>
#include <functional>
#include <vector>

#include "Process.h"



class CoreWorker
{
public:
    CoreWorker(int id, float delayPerExec, float quantumSlice = 0);
    ~CoreWorker();

    std::shared_ptr<Process> getCurrentProcess();
    void setProcess(std::shared_ptr<Process> process);
    void runProcess();
    void finishProcess();

    bool isAvailable();
	bool isAssignedProcess();
    int getID();

    void start();
    void stop();

    void setProcessCompletionCallback(std::function<void(std::shared_ptr<Process>)> callback);

    std::vector<long long> getStats();

private:
    int id;
    std::thread coreThread;
    bool running;
    bool processAssigned;
    std::mutex coreMutex;
    std::shared_ptr<Process> currentProcess;

    float delayPerExec = 0;
    float quantumSlice = 0;

    void run();
    std::function<void(std::shared_ptr<Process>)> processCompletionCallback;

    // Stat trackers
    long long totalCPUTicks = 0;
    long long totalActiveTicks = 0;
    long long totalIdleTicks = 0;

};
