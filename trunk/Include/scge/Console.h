#ifndef __Console_h__
#define __Console_h__

#include "scge\FileSystem.h"

#include "scge\Console\ConsoleFunction.h"
#include "scge\Console\ConsoleVariable.h"
#include "scge\Console\ConsoleAlias.h"
#include "scge\Console\ConsoleEvent.h"

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <deque>
#include <tuple>
#include <queue>
#include <mutex>

class Console
{
public:
	Console(FileSystem &fileSystem, bool useSettings = true);
	~Console();

	void update();

	void print(std::string text);
	void printError(const std::string &text) { print("Error: " + text); }
	void execute(std::string text, bool silent = false, bool canStoreCommand = false);

	void threadSafePrint(std::string text);
	void threadSafePrintError(const std::string &text) { threadSafePrint("Error: " + text); }
	void threadSafeExecute(std::string text, bool silent = false, bool canStoreCommand = false);

	bool executeFile(const std::string &file, bool silent = false, bool canStoreCommand = false);

	void storeCommand(std::string name, std::string command);

	void updateCommandHistory();
	void updateOutputHistory();

	bool registerCommand(ConsoleCommand &command);
	void removeCommand(ConsoleCommand &command);

	void createAlias(const std::string &name, std::string command);
	void deleteAlias(const std::string &name);

	void addDefaultCommand(const std::string &command);

	std::vector<std::string> getSuggestions(const std::string &text, unsigned int max = 0) const;

private:
	FileSystem &mFileSystem;

	bool mUseSettings;
	bool mLoadedValues;

	std::unordered_map<std::string, ConsoleCommand*> mConsoleCommands;
	std::unordered_map<std::string, std::unique_ptr<ConsoleAlias>> mAliases;
	std::unordered_multimap<std::string, std::string> mStoredCommands;

	ConsoleVariable<size_t, true, true> mMaxOutputHistory;
	std::deque<std::string> mOutputHistory;

	ConsoleVariable<size_t, true, true> mMaxCommandHistory;
	std::deque<std::string> mCommandHistory;

	std::mutex mQueuedCommandsMutex;
	std::queue<std::tuple<bool, std::string, bool, bool>> mQueuedCommands;

	ConsoleFunction mCommandPrint;
	ConsoleFunction mCreateAlias;
	ConsoleFunction mRemoveAlias;
	ConsoleFunction mExecute;

	void loadConfigFile()
	{
		mLoadedValues = !executeFile("settings.cfg", true, true);
		if(!mLoadedValues)
			print("Failed to load config file, using default settings");
	}
	void saveConfigFile();

	Console(const Console &other);
	Console &operator=(const Console &other);
};

#endif // __Console_h__
