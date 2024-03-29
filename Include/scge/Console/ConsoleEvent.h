#ifndef __ConsoleEvent_h__
#define __ConsoleEvent_h__

#include "scge\Console\ConsoleCommand.h"

class InputDevice;
class ConsoleEvent : public ConsoleCommand
{
public:
#if defined(_MSC_VER)
static_assert(_MSC_VER == 1700, "Use variadic templates to allow multiple key to be passed in as default bindings");
#endif
	ConsoleEvent(Console &console, InputDevice &inputDevice, std::string name);
	virtual ~ConsoleEvent();

	virtual void parse(const std::vector<std::string> &arguments, bool silent) final;

	void resetState() { mEventUp = false; mEventDown = false; mEventHeld = false; }

	bool isRelease() const { return mEventUp; }
	bool isPress() const { return mEventDown; }
	bool isHeld() const { return mEventHeld; }

	void setUpState() { mEventUp = true; }
	void setDownState() { mEventDown = true; }
	void setHoldState() { mEventHeld = true; }

private:
	InputDevice &mInputDevice;
	bool mRegisteredForInput;

	bool mEventDown;
	bool mEventUp;
	bool mEventHeld;
};

#endif // __ConsoleEvent_h__
