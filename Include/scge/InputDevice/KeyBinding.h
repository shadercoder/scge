#ifndef KeyBinding_h__
#define KeyBinding_h__

#include "boost/variant.hpp"

class Console;
class ConsoleEvent;
class KeyBinding
{
public:
	void setConsoleEvent(ConsoleEvent &consoleEvent);
	void setRawKeyDown(const std::string &keyDown);
	void setRawKeyUp(const std::string &keyUp);
	void setSavedBinding(const std::string &savedBinding);

	void onKeyDown(Console &console);
	void onKeyUp(Console &console);
	void onKeyHold();

	void onRegisterConsoleEvent(ConsoleEvent &consoleEvent);
	void onUnregisterConsoleEvent(ConsoleEvent &consoleEvent);

	void saveKeyBind(Console &console, const std::string &bindName, const std::string &keyName) const;

private:
	boost::variant<ConsoleEvent*, std::string, std::pair<std::string, std::string>> mBinding;
};

#endif // KeyBinding_h__