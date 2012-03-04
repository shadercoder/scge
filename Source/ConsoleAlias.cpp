#include "scge\Console\ConsoleAlias.h"

#include "scge\Console.h"

ConsoleAlias::ConsoleAlias(Console &console, const std::string &name, const std::string &command)
	: ConsoleCommand(name, command)
{
	initialise(console);
}

ConsoleAlias::ConsoleAlias(const std::string &name, const std::string &command)
	: ConsoleCommand(name, command)
{
}

void ConsoleAlias::parse(const std::vector<std::string> &arguments, bool silent)
{
	if(arguments.size() == 1)
		mConsole->execute(mComment, silent);
	else if(!silent)
		mConsole->printError(StringUtility::format("Alias % called with % too many arguments.", mName, arguments.size()-1));
}