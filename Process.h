#pragma once
#include <chrono>
#include <string>

class Process
{
public:
	Process(std::string name, int id, int totalInstructions, int memorySize);

	std::string getName() const;
	int getID() const;
	int getCurrentInstruction() const;
	int getTotalInstructions() const;
	int getRemainingInstructions() const;
	int getCore();
	void setCore(int coreID);
	int getMemorySize() const;
	bool isFinished() const;
	std::string getCreationTime() const;

	void execute();

private:
	std::string processName;
	int id;
	int currentInstruction;
	int totalInstructions;
	int core;
	int memorySize;
	bool finished;
	std::chrono::system_clock::time_point creationTime;
};

