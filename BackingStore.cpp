#include "BackingStore.h"
#include <iostream>

BackingStore::BackingStore(const std::string& filename) : filename(filename) {
    openFiles();
}

BackingStore::~BackingStore() {
    closeFiles();
}

void BackingStore::openFiles() {
    outputFile.open(filename, std::ios::out | std::ios::app);
    inputFile.open(filename, std::ios::in);
}

void BackingStore::closeFiles() {
    if (outputFile.is_open()) {
        outputFile.close();
    }
    if (inputFile.is_open()) {
        inputFile.close();
    }
}

void BackingStore::storeProcess(std::shared_ptr<Process> process) {
    if (outputFile.is_open()) {
        outputFile << process->getID() << " "
            << process->getName() << " "
            << process->getTotalInstructions() << " "
            << process->getMemorySize() << std::endl;
        storedProcesses[process->getID()] = process;
    }
}

std::shared_ptr<Process> BackingStore::loadProcess(int pid) {
    auto it = storedProcesses.find(pid);
    if (it != storedProcesses.end()) {
        return it->second;
    }
    else {
        std::shared_ptr<Process> process = std::make_shared<Process>();
        if (inputFile.is_open()) {
            inputFile.clear();
            inputFile.seekg(0, std::ios::beg); // Reset to beginning
            int id;
            std::string name;
            float totalInstructions, memorySize;
            while (inputFile >> id >> name >> totalInstructions >> memorySize) {
                if (id == pid) {
                    process = std::make_shared<Process>(name, id, totalInstructions, memorySize, 1);
                    storedProcesses[id] = process;
                    return process;
                }
            }
        }
        throw std::runtime_error("Process not found in backing store.");
    }
}

void BackingStore::removeProcess(int pid) {
    storedProcesses.erase(pid);
    // Updating the file is complex; usually not done in this simple example
}

void BackingStore::displayContents() const {
    if (storedProcesses.empty()) {
        std::cout << "Backing store is empty." << std::endl;
        return;
    }

    std::cout << "Backing Store Contents:" << std::endl;
    for (const auto& entry : storedProcesses) {
        std::shared_ptr<Process> process = entry.second;
        std::cout << "Process ID: " << process->getID()
            << ", Name: " << process->getName()
            << ", Memory Size: " << process->getMemorySize()
            << ", Instructions: " << process->getTotalInstructions() << std::endl;
    }
}
