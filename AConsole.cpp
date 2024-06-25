#include "AConsole.h"

AConsole::AConsole(std::string name)
{
	this->name = name;
}

AConsole::~AConsole()
{
}

std::string AConsole::getName()
{
	return this->name;
}


