#pragma once
#include <chrono>
#include <string>

class Process
{
public:
	Process(std::string name, int id, int totalInstructions);

	std::string getName() const;
	int getID() const;
	int getCurrentInstruction() const;
	int getTotalInstructions() const;
	int getRemainingInstructions() const;
	int getCore();
	void setCore(int coreID);
	bool isFinished() const;
	std::string getCreationTime() const;
	int getMemorySize() const;

	void execute();

private:
	std::string processName;
	int id;
	int currentInstruction;
	int totalInstructions;
	int core;
	bool finished;
	std::chrono::system_clock::time_point creationTime;
	int memorySize;
};

