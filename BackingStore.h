#pragma once
#include <unordered_map>
#include <memory>
#include <string>
#include <fstream>
#include "Process.h"

class BackingStore {
public:
    BackingStore(const std::string& filename);
    ~BackingStore();

    void storeProcess(std::shared_ptr<Process> process);
    std::shared_ptr<Process> loadProcess(int pid);
    void removeProcess(int pid);

private:
    std::string filename;
    std::unordered_map<int, std::shared_ptr<Process>> storedProcesses;
    std::ofstream outputFile;
    std::ifstream inputFile;
};
