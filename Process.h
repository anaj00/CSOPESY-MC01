#pragma once
#include <chrono>
#include <string>

class Process
{
public:
	Process(std::string name, int id, float totalInstructions, float memorySize, float pageSize);

	std::string getName() const;
	int getID() const;
	int getCurrentInstruction() const;
	int getTotalInstructions() const;
	int getRemainingInstructions() const;
	int getCore();
	void setCore(int coreID);
	bool isFinished() const;
	std::string getCreationTime() const;
	float getMemorySize() const;
	float getPageSize() const;

	void execute(); // Executes one instruction

private:
	std::string processName;
	int id;
	int core;
	std::string status;
	std::chrono::system_clock::time_point creationTime;

	bool finished;
	int currentInstruction;
	int totalInstructions;
	
	float memorySize;
	float pageSize;
};

