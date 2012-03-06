#include "scge\Console\ConsoleAlias.h"

#include "scge\Console.h"

ConsoleAlias::ConsoleAlias(Console &console, std::string name, std::string command)
	: ConsoleCommand(std::move(name), std::move(command))
{
	initialise(console);
}

ConsoleAlias::ConsoleAlias(std::string name, std::string command)
	: ConsoleCommand(std::move(name), std::move(command))
{
}

void ConsoleAlias::parse(const std::vector<std::string> &arguments, bool silent)
{
	if(arguments.size() == 1)
		mConsole->execute(mComment, silent);
	else if(!silent)
		mConsole->printError(StringUtility::format("Alias % called with % too many arguments.", mName, arguments.size()-1));
}