#ifndef __ConsoleFunction_h__
#define __ConsoleFunction_h__

#include "scge\Console\ConsoleCommand.h"

#include <functional>

static_assert(_MSC_VER == 1700, "Look into templating this so that proper functions can be added and as it knows the values it can parse them all and pass them properly into the function");

class ConsoleFunction: public ConsoleCommand
{
public:
	ConsoleFunction(Console &console, const std::string &name, const std::string &comment, std::function<void(const std::vector<std::string> &, bool)> function, unsigned int minArguments = 0, unsigned int maxArguments = 0);
	ConsoleFunction(const std::string &name, const std::string &comment);

	void parse(const std::vector<std::string> &arguments, bool silent);

	bool initialise(Console &console) { return ConsoleCommand::initialise(console); }
	bool initialise(Console &console, std::function<void(const std::vector<std::string> &, bool)> function, unsigned int minArguments = 0, unsigned int maxArguments = 0);

private:
	std::function<void(const std::vector<std::string> &, bool)> mFunction;
	unsigned int mMinArguments;
	unsigned int mMaxArguments;
};

#endif // __ConsoleFunction_h__