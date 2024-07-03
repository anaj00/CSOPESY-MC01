#pragma once

#include <thread>
#include <memory>
#include <mutex>
#include "Process.h"

class CoreWorker
{
public:
    CoreWorker(int id, int delayPerExec, int quantumSlice = 0);
    ~CoreWorker();

    std::shared_ptr<Process> getCurrentProcess();
    void setProcess(std::shared_ptr<Process> process);
    void runProcess();
    void finishProcess();
    
    bool isAvailable();
    int getID();


    void start();
    void stop();

private:
    int id;
    std::thread coreThread;
    bool running;
    bool processAssigned;
    std::mutex coreMutex;
    std::shared_ptr<Process> currentProcess;

    int delayPerExec = 0;
    int quantumSlice = 0;

    void run();
};
