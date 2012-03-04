#include "scge\Console\ConsoleCommand.h"

#include "scge\Console.h"

ConsoleCommand::ConsoleCommand(const std::string &name, const std::string &comment)
	: mName(name)
	, mComment(comment)
	, mConsole(nullptr)
{
}

bool ConsoleCommand::initialise(Console &console)
{
	if(mConsole && mConsole != &console)
	{
		mConsole->removeCommand(*this);
		mConsole = nullptr;
	}

	if(mConsole == nullptr)
	{
		if(console.registerCommand(*this))
			mConsole = &console;
	}

	return mConsole != nullptr;
}

void ConsoleCommand::release()
{
	if(mConsole)
	{
		mConsole->removeCommand(*this);
		mConsole = nullptr;
	}
}

ConsoleCommand::~ConsoleCommand()
{
	if(mConsole)
		mConsole->removeCommand(*this);
	mConsole = nullptr;
}

void ConsoleCommand::storeCommand(const std::string &command) const
{
	if(mConsole)
		mConsole->storeCommand(mName, command);
}