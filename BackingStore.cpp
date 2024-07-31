#include "BackingStore.h"
#include <iostream>

BackingStore::BackingStore(const std::string& filename) : filename(filename) {
    outputFile.open(filename, std::ios::out | std::ios::binary | std::ios::app);
    inputFile.open(filename, std::ios::in | std::ios::binary);
}

BackingStore::~BackingStore() {
    if (outputFile.is_open()) {
        outputFile.close();
    }
    if (inputFile.is_open()) {
        inputFile.close();
    }
}

void BackingStore::storeProcess(std::shared_ptr<Process> process) {
    if (outputFile.is_open()) {
        outputFile.write(reinterpret_cast<char*>(process->getID()), sizeof(int));
        // Serialize and write the rest of the process data as needed
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
            inputFile.read(reinterpret_cast<char*>(&pid), sizeof(int));
            // Deserialize and read the rest of the process data as needed
        }
        storedProcesses[pid] = process;
        return process;
    }
}

void BackingStore::removeProcess(int pid) {
    storedProcesses.erase(pid);
    // Update the backing store file as needed
}
