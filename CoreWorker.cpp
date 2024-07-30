#include "CoreWorker.h"

#include <iostream>

CoreWorker::CoreWorker(int id, float delayPerExec, float quantumSlice) 
    : id(id), delayPerExec(delayPerExec), quantumSlice(quantumSlice), running(false), processAssigned(false) {}

CoreWorker::~CoreWorker() {
    stop();
    if (coreThread.joinable()) {
        coreThread.join();
    }
}

std::shared_ptr<Process> CoreWorker::getCurrentProcess() {
	std::lock_guard<std::mutex> lock(coreMutex);
	return currentProcess;
}

void CoreWorker::setProcess(std::shared_ptr<Process> process) {
    std::lock_guard<std::mutex> lock(coreMutex);
    currentProcess = process;
    processAssigned = true;
}

void CoreWorker::runProcess() {

    // For SJF and FCFS
    if (quantumSlice == 0) {
        // While there is still a process, run
        while (currentProcess && !currentProcess->isFinished()) {
            currentProcess->execute();
            std::this_thread::sleep_for(std::chrono::duration<float>(delayPerExec));
        }

        // Process is finished, notify Scheduler
        if (currentProcess && currentProcess->isFinished()) {
            finishProcess();
        }
    }

    // For RR
    else {
        for (int i = 0; i < quantumSlice; i++) {
            if (currentProcess->isFinished()) {
                finishProcess();
                return;
            }
            currentProcess->execute();
            std::this_thread::sleep_for(std::chrono::duration<float>(delayPerExec));
        }

        // Process is not finished, notify Scheduler
        if (processCompletionCallback) {
            processCompletionCallback(currentProcess);
        }

        // Reset the current process to ensure it gets requeued
        currentProcess.reset();
        processAssigned = false;
    }
}


void CoreWorker::finishProcess() {
    std::lock_guard<std::mutex> lock(coreMutex);
    currentProcess.reset(); // Reset the current process
    processAssigned = false; // No more process assigned
}

bool CoreWorker::isAvailable() {
    std::lock_guard<std::mutex> lock(coreMutex);
    return !processAssigned; // Core is available if no process is assigned
}

bool CoreWorker::isAssignedProcess() {
	std::lock_guard<std::mutex> lock(coreMutex);
	return processAssigned; // Has a process assigned
}

int CoreWorker::getID() {
    return id;
}

void CoreWorker::start() {
    running = true;
    coreThread = std::thread(&CoreWorker::run, this); // Start the core thread
}

void CoreWorker::stop() {
    running = false;
    {
        std::lock_guard<std::mutex> lock(coreMutex);
        running = false;
    }
    if (coreThread.joinable()) {
        coreThread.join();
    }
}

void CoreWorker::run() {
    // Core thread loop
    while (true) {
        {
            std::lock_guard<std::mutex> lock(coreMutex);
            if (!running) {
                break;
            }
        }

        if (processAssigned) {
            runProcess();
        }
    }
}

// Set the process completion callback
void CoreWorker::setProcessCompletionCallback(std::function<void(std::shared_ptr<Process>)> callback) {
    std::lock_guard<std::mutex> lock(coreMutex);
    processCompletionCallback = callback;
}
