#include "scge\InputDevice\KeyBinding.h"

#include "scge\Console\ConsoleEvent.h"
#include "scge\Console.h"

void KeyBinding::setConsoleEvent(ConsoleEvent &consoleEvent)
{
	mBinding = &consoleEvent;
}

void KeyBinding::setRawKeyDown(const std::string &keyDown)
{
	if(auto rawBinding = boost::get<std::pair<std::string, std::string>>(&mBinding))
		rawBinding->second = keyDown;
	else
		mBinding = std::make_pair(std::string(), keyDown);
}

void KeyBinding::setRawKeyUp(const std::string &keyUp)
{
	if(auto rawBinding = boost::get<std::pair<std::string, std::string>>(&mBinding))
		rawBinding->first = keyUp;
	else
		mBinding = std::make_pair(keyUp, std::string());
}

void KeyBinding::setSavedBinding(const std::string &savedBinding)
{
	mBinding = savedBinding;
}

void KeyBinding::onKeyDown(Console &console)
{
	if(auto consoleEvent = boost::get<ConsoleEvent*>(&mBinding))
		(*consoleEvent)->setDownState();
	else if(auto rawBinding = boost::get<std::pair<std::string, std::string>>(&mBinding))
	{
		if(!rawBinding->second.empty())
			console.execute(rawBinding->second);
	}
}

void KeyBinding::onKeyUp(Console &console)
{
	if(auto consoleEvent = boost::get<ConsoleEvent*>(&mBinding))
		(*consoleEvent)->setUpState();
	else if(auto rawBinding = boost::get<std::pair<std::string, std::string>>(&mBinding))
	{
		if(!rawBinding->first.empty())
			console.execute(rawBinding->first);
	}
}

void KeyBinding::onKeyHold()
{
	if(auto consoleEvent = boost::get<ConsoleEvent*>(&mBinding))
		(*consoleEvent)->setHoldState();
}

void KeyBinding::onRegisterConsoleEvent(ConsoleEvent &consoleEvent)
{
	if(auto savedBinding = boost::get<std::string>(&mBinding))
	{
		if(*savedBinding == consoleEvent.getName())
			setConsoleEvent(consoleEvent);
	}
}

void KeyBinding::onUnregisterConsoleEvent(ConsoleEvent &consoleEvent)
{
	if(auto currentConsoleEvent = boost::get<ConsoleEvent*>(&mBinding))
	{
		if(*currentConsoleEvent == &consoleEvent)
			setSavedBinding(consoleEvent.getName());
	}
}

void KeyBinding::saveKeyBind(Console &console, const std::string &bindName, const std::string &keyName) const
{
	if(auto consoleEvent = boost::get<ConsoleEvent*>(&mBinding))
		console.storeCommand(bindName, bindName + " " + keyName + " " + (*consoleEvent)->getName());
	else if(auto savedBinding = boost::get<std::string>(&mBinding))
		console.storeCommand(bindName, bindName + " " + keyName + " " + *savedBinding);
	else if(auto rawBinding = boost::get<std::pair<std::string, std::string>>(&mBinding))
	{
		if(!rawBinding->first.empty())
			console.storeCommand(bindName, "-" + bindName + " " + keyName + " " + rawBinding->first);
		if(!rawBinding->second.empty())
			console.storeCommand(bindName, "+" + bindName + " " + keyName + " " + rawBinding->second);
	}
}