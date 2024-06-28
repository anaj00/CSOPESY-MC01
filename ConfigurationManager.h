#pragma once

#include <string>
#include <unordered_map>

class ConfigurationManager {
public:
    static ConfigurationManager& getInstance();

    bool runInitialize(); // Returns true if initialization was successful
    bool isInitialized() const; // Returns true if the configuration manager has been initialized

    int getMinInstructions() const; // Returns the minimum number of instructions a process can have
    int getMaxInstructions() const; // Returns the maximum number of instructions a process can have
    int getNumCPU() const;
    std::string getSchedulerAlgorithm() const;
    int getQuantumCycles() const;
    bool isPreemptive() const;
    int getBatchProcessFrequency() const;
    int getDelayPerExec() const;

private:
    ConfigurationManager();
    ~ConfigurationManager();

    void parseConfigFile(); // Parses the config file and sets the configuration values
    void printConfig(); // Prints the configuration values to the console

    bool initialized = false;

    int numCPU;
    std::string schedulerAlgorithm;
    int quantumCycles;
    bool preemptive;
    int batchProcessFrequency;
    int minInstructions;
    int maxInstructions;
    int delayPerExec;
};
