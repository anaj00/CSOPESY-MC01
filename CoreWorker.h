#pragma once
#include <memory>
#include <functional>
#include <mutex>
#include "Process.h"

class CoreWorker {
public:
    CoreWorker(int id, int delayPerExec, int quantumSlice = 0);
    ~CoreWorker();

    std::shared_ptr<Process> getCurrentProcess();
    void setProcess(std::shared_ptr<Process> process);
    void setProcessCompletionCallback(std::function<void(std::shared_ptr<Process>)> callback);
    void setQuantumCycleCallback(std::function<void(int)> callback);

    bool isAvailable();
    bool isAssignedProcess();
    int getID();
    void start();
    void stop();

private:
    void run();
    void runProcess();
    void finishProcess();

    int id;
    int delayPerExec;
    int quantumSlice;
    bool running;
    bool processAssigned;
    std::shared_ptr<Process> currentProcess;
    std::thread coreThread;
    std::mutex coreMutex;
    std::function<void(std::shared_ptr<Process>)> processCompletionCallback;
    std::function<void(int)> quantumCycleCallback;
};
