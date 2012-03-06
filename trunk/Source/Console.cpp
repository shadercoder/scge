#include "scge\Console.h"

#include "scge\FileSystem.h"

#include <functional>
#include <fstream>

#include "scge\Config.h"
#if defined(SCGE_DEBUG)
	#include <windows.h>
#endif

Console::Console(FileSystem &fileSystem, bool useSettings)
	: mFileSystem(fileSystem)
	, mUseSettings(useSettings)
	, mLoadedValues(false)
	, mMaxCommandHistory("Console.MaxCommandHistory", "Number of previous commands entered the console remembers.")
	, mMaxOutputHistory("Console.MaxOutputHistory", "Maximum number of lines in the console.")
	, mCommandPrint("Console.Print", "Prints a string to the console.")
	, mCreateAlias("Console.CreateAlias", "Creates an alias for a set of commands.")
	, mRemoveAlias("Console.RemoveAlias", "Removes an alias.")
	, mExecute("Console.Execute", "Loads a config file")
{
	mMaxCommandHistory.initialise(*this, 0, 100, 10, [this](){ updateCommandHistory(); });
	mMaxOutputHistory.initialise(*this, 10, 1000, 100, [this](){ updateOutputHistory(); });
	mCommandPrint.initialise(*this, [this](const std::vector<std::string> &args, bool){ print(args[1]); }, 1, 1);
	mCreateAlias.initialise(*this, [this](const std::vector<std::string> &args, bool){ createAlias(args[1], args[2]); }, 2, 2);
	mRemoveAlias.initialise(*this, [this](const std::vector<std::string> &args, bool){ deleteAlias(args[1]); }, 1, 1);
	mExecute.initialise(*this, [this](const std::vector<std::string> &args, bool silent){ executeFile(args[1], silent, false); }, 1, 1);

	if(mUseSettings)
		loadConfigFile();
	executeFile("autoexec.cfg", true, true);
}

Console::~Console()
{
	mExecute.release();
	mRemoveAlias.release();
	mCreateAlias.release();
	mCommandPrint.release();
	mMaxOutputHistory.release();
	mMaxCommandHistory.release();

	for(const auto & alias : mAliases)
	{
		if(alias.second->getComment().find_first_of("'") == std::string::npos)
			storeCommand(mCreateAlias.getName(), mCreateAlias.getName() + " " + alias.second->getName() + " '" + alias.second->getComment() + "'");
		else
			storeCommand(mCreateAlias.getName(), mCreateAlias.getName() + " " + alias.second->getName() + " \"" + alias.second->getComment() + "\"");
	}
	mAliases.clear();

	auto it = mConsoleCommands.begin(), end = mConsoleCommands.end();
	while(it != end)
	{
		printError(StringUtility::format("Command % lifetime longer then the console", it->first));
		(it++)->second->release();
	}

	if(mUseSettings)
		saveConfigFile();
}

void Console::update()
{
	boost::mutex::scoped_lock lock(mQueuedCommandsMutex);

	while(!mQueuedCommands.empty())
	{
		auto command = mQueuedCommands.front();
		mQueuedCommands.pop();

		if(std::get<0>(command))
			print(std::get<1>(command));
		else
			execute(std::get<1>(command), std::get<2>(command), std::get<3>(command));
	}
}

void Console::print(std::string text)
{
#if defined(SCGE_DEBUG)
	OutputDebugStringA(("Console: " + text + "\n").c_str());
#endif

	mOutputHistory.push_back(std::move(text));

	if(mOutputHistory.size() > mMaxOutputHistory.getValue())
		mOutputHistory.pop_front();
}

void Console::printError(std::string text)
{
	print("Error: " + std::move(text));
}

void Console::execute(std::string text, bool silent, bool canStoreCommand)
{
	StringUtility::foreachTokenSection(text, [this, silent, canStoreCommand](const std::vector<std::string> &arguments, const std::string &line)
	{
		auto command = mConsoleCommands.find(arguments[0]);
		if(command == mConsoleCommands.end())
		{
			if(canStoreCommand)
				storeCommand(arguments[0], line);
			else if(!silent)
				printError("Not known command " + arguments[0]);
		}
		else
			command->second->parse(arguments, silent);
	});

	if(!silent)
	{
		mCommandHistory.push_back(std::move(text));
		while(mCommandHistory.size() > mMaxCommandHistory.getValue())
			mCommandHistory.pop_front();
	}
}

void Console::storeCommand(std::string name, std::string command)
{
	mStoredCommands.insert(std::make_pair(std::move(name), std::move(command)));
}

void Console::updateCommandHistory()
{
	mMaxCommandHistory.updateValue();
	while(mCommandHistory.size() > mMaxCommandHistory.getValue())
		mCommandHistory.pop_front();
}

void Console::updateOutputHistory()
{
	mMaxOutputHistory.updateValue();
	while(mOutputHistory.size() > mMaxOutputHistory.getValue())
		mOutputHistory.pop_front();
}

bool Console::registerCommand(ConsoleCommand &command)
{
	const std::string &name = command.getName();
	if(mConsoleCommands.count(name) != 0)
		return false;

	if(name.empty())
		return false;

	mConsoleCommands[name] = &command;

	auto stored = mStoredCommands.equal_range(name);
	for(auto it = stored.first; it != stored.second; ++it)
		execute(it->second, true);
	mStoredCommands.erase(stored.first, stored.second);

	return true;
}

void Console::removeCommand(ConsoleCommand &command)
{
	mConsoleCommands.erase(command.getName());
}

void Console::createAlias(const std::string &name, std::string command)
{
	auto it = mAliases.find(name);
	if(it != mAliases.end())
		mAliases.erase(it);

	std::unique_ptr<ConsoleAlias> newAlias(new ConsoleAlias(name, std::move(command)));
	if(newAlias->initialise(*this))
		mAliases[name] = std::move(newAlias);
	else
		printError("Failed to create alias " + name);
}

void Console::deleteAlias(const std::string &name)
{
	mAliases.erase(name);
}

bool Console::executeFile(const std::string &file, bool silent, bool canStoreCommand)
{
	std::ifstream fileStream = mFileSystem.OpenFileRead(file);
	if(!fileStream || !fileStream.is_open())
	{
		if(!silent)
			printError("Failed to open file " + file);
		return false;
	}

	while(!fileStream.eof())
	{
		std::string line;
		getline(fileStream, line);
		if(line.length() == 0 || line.substr(0, 2) == "//" || line[0] == '#')
			continue;

		execute(line, silent, canStoreCommand);
	}

	return true;
}

void Console::saveConfigFile()
{
	std::ofstream fileStream = mFileSystem.OpenFileWrite("settings.cfg", true);
	if(!fileStream)
	{
		printError("Failed to open file settings.cfg for saving");
		return;
	}

	for(auto it = mStoredCommands.begin(), end = mStoredCommands.end(); it != end; ++it)
		fileStream << it->second << std::endl;

	fileStream.close();
}

void Console::addDefaultCommand(const std::string &command)
{
	if(!mLoadedValues)
		execute(command, true, true);
}

std::vector<std::string> Console::getSuggestions(const std::string &text, unsigned int max) const
{
	std::vector<std::string> posibleValues;
	for(const auto & consoleCommand : mConsoleCommands)
	{
		if(consoleCommand.first.find(text) != std::string::npos)
		{
			posibleValues.push_back(consoleCommand.first);
			if(max != 0 &&  posibleValues.size() >= max)
				break;
		}
	}

	return posibleValues;
}

void Console::threadSafePrint(std::string text)
{
	boost::mutex::scoped_lock lock(mQueuedCommandsMutex);

	mQueuedCommands.push(std::make_tuple(true, std::move(text), false, false));
}

void Console::threadSafePrintError(std::string text)
{
	threadSafePrint("Error: " + std::move(text));
}

void Console::threadSafeExecute(std::string text, bool silent, bool canStoreCommand)
{
	boost::mutex::scoped_lock lock(mQueuedCommandsMutex);

	mQueuedCommands.push(std::make_tuple(false, std::move(text), silent, canStoreCommand));
}