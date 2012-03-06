#ifndef __ConsoleCommand_h__
#define __ConsoleCommand_h__

#include <string>
#include <vector>
#include <set>

class Console;
class ConsoleCommand
{
public:
	ConsoleCommand(std::string name, std::string comment);
	virtual ~ConsoleCommand();

	bool initialise(Console &console);
	void release();

	virtual void parse(const std::vector<std::string> &arguments, bool silent) = 0;

	const std::string &getName() const { return mName; }
	const std::string &getComment() const { return mComment; }

	void storeCommand(std::string command) const;

	bool isInitialised() const { return mConsole != nullptr; }

protected:
	Console *mConsole;

	const std::string mName;
	const std::string mComment;

private:
	ConsoleCommand(const ConsoleCommand &other);
	ConsoleCommand &operator=(const ConsoleCommand &other);
};

#endif // __ConsoleCommand_h__