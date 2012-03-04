#include "scge\Console\ConsoleFunction.h"

#include "scge\Console.h"

ConsoleFunction::ConsoleFunction(Console &console, const std::string &name, const std::string &comment, std::function<void(const std::vector<std::string> &, bool)> function, unsigned int minArguments, unsigned int maxArguments)
	: ConsoleCommand(name, comment)
	, mFunction(function)
	, mMinArguments(minArguments)
	, mMaxArguments(maxArguments)
{
	if(mMinArguments > mMaxArguments)
		std::swap(mMinArguments, mMaxArguments);

	initialise(console);
}

ConsoleFunction::ConsoleFunction(const std::string &name, const std::string &comment)
	: ConsoleCommand(name, comment)
	, mFunction()
	, mMinArguments()
	, mMaxArguments()
{
}

void ConsoleFunction::parse(const std::vector<std::string> &arguments, bool silent)
{
	if(arguments.size()-1 < mMinArguments)
	{
		if(!silent)
		{
			mConsole->printError("To Few Arguments");
			if(!mComment.empty())
				mConsole->print(mComment);
		}
	}
	else if(arguments.size()-1 > mMaxArguments)
	{
		if(!silent)
		{
			mConsole->printError("To Many Arguments");
			if(!mComment.empty())
				mConsole->print(mComment);
		}
	}
	else
		mFunction(arguments, silent);
}

bool ConsoleFunction::initialise(Console &console, std::function<void(const std::vector<std::string> &, bool)> function, unsigned int minArguments, unsigned int maxArguments)
{
	mMinArguments = minArguments;
	mMaxArguments = maxArguments;
	if(mMinArguments > mMaxArguments)
		std::swap(mMinArguments, mMaxArguments);

	mFunction = function;

	return initialise(console);
}