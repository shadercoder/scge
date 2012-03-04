#include "scge\Console\ConsoleEvent.h"

#include "scge\Console.h"
#include "scge\InputDevice.h"

ConsoleEvent::ConsoleEvent(Console &console, InputDevice &inputDevice, const std::string &name)
	: ConsoleCommand(name, "")
	, mInputDevice(inputDevice)
	, mRegisteredForInput(false)
	, mEventDown(false)
	, mEventUp(false)
{
	if(initialise(console))
	{
		mInputDevice.registerConsoleEvent(*this);
		mRegisteredForInput = true;
	}
}

ConsoleEvent::~ConsoleEvent()
{
	if(mRegisteredForInput)
		mInputDevice.unregisterConsoleEvent(*this);
}

void ConsoleEvent::parse(const std::vector<std::string> &arguments, bool silent)
{
	if(arguments.size() != 1)
	{
		if(!silent)
			mConsole->printError("Console Events take no arguments");
	}
	else
	{
		mEventDown = true;
		mEventUp = true;
	}
}