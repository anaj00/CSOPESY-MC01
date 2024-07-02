#include "CoreWorker.h"
#include <iostream>

CoreWorker::CoreWorker(int id) : id(id), running(false), processAssigned(false) {}

CoreWorker::~CoreWorker() {
    stop();
    if (coreThread.joinable()) {
        coreThread.join();
    }
}

void CoreWorker::setProcess(std::shared_ptr<Process> process) {
    std::lock_guard<std::mutex> lock(coreMutex);
    currentProcess = process;
    processAssigned = true;
}

void CoreWorker::runProcess() {
    while (currentProcess && !currentProcess->isFinished()) {
        currentProcess->execute();
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Adjust the sleep duration as needed
    }
    if (currentProcess && currentProcess->isFinished()) {
        finishProcess();
    }
}

void CoreWorker::finishProcess() {
    std::lock_guard<std::mutex> lock(coreMutex);
    currentProcess.reset();
    processAssigned = false;
}

bool CoreWorker::isAvailable() {
    std::lock_guard<std::mutex> lock(coreMutex);
    return !processAssigned;
}

int CoreWorker::getID() {
    return id;
}

void CoreWorker::start() {
    running = true;
    coreThread = std::thread(&CoreWorker::run, this);
}

void CoreWorker::stop() {
    {
        std::lock_guard<std::mutex> lock(coreMutex);
        running = false;
    }
    if (coreThread.joinable()) {
        coreThread.join();
    }
}

void CoreWorker::run() {
    while (true) {
        {
            std::lock_guard<std::mutex> lock(coreMutex);
            if (!running) {
                break;
            }
        }

        if (processAssigned && currentProcess) {
            runProcess();
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); // Adjust the sleep duration as needed
    }
}
