#include "scge\Console\ConsoleCommand.h"

#include "scge\Console.h"

ConsoleCommand::ConsoleCommand(std::string name, std::string comment)
	: mConsole(nullptr)
	, mName(std::move(name))
	, mComment(std::move(comment))
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

void ConsoleCommand::storeCommand(std::string command) const
{
	if(mConsole)
		mConsole->storeCommand(mName, std::move(command));
}
