#ifndef __ConsoleAlias_h__
#define __ConsoleAlias_h__

#include "scge\Console\ConsoleCommand.h"

class ConsoleAlias: public ConsoleCommand
{
public:
	ConsoleAlias(Console &console, const std::string &name, const std::string &command);
	ConsoleAlias(const std::string &name, const std::string &command);

	void parse(const std::vector<std::string> &arguments, bool silent);
};

#endif // __ConsoleAlias_h__