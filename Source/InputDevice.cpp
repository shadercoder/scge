#include "scge\InputDevice.h"

#include "scge\Console\ConsoleEvent.h"

#include "scge\Config.h"
#include <Windows.h>
#include <windowsx.h>



template <typename T>
inline void fillBiMap(T &mKeyNameBiMap)
{
	#define SCGE_ADD_KEY(key) mKeyNameBiMap.insert(T::value_type(#key, Key::key));

	SCGE_ADD_KEY(Numpad0);
	SCGE_ADD_KEY(Numpad1);
	SCGE_ADD_KEY(Numpad2);
	SCGE_ADD_KEY(Numpad3);
	SCGE_ADD_KEY(Numpad4);
	SCGE_ADD_KEY(Numpad5);
	SCGE_ADD_KEY(Numpad6);
	SCGE_ADD_KEY(Numpad7);
	SCGE_ADD_KEY(Numpad8);
	SCGE_ADD_KEY(Numpad9);

	SCGE_ADD_KEY(F1);
	SCGE_ADD_KEY(F2);
	SCGE_ADD_KEY(F3);
	SCGE_ADD_KEY(F4);
	SCGE_ADD_KEY(F5);
	SCGE_ADD_KEY(F6);
	SCGE_ADD_KEY(F7);
	SCGE_ADD_KEY(F8);
	SCGE_ADD_KEY(F9);
	SCGE_ADD_KEY(F10);
	SCGE_ADD_KEY(F11);
	SCGE_ADD_KEY(F12);

	SCGE_ADD_KEY(LButton);
	SCGE_ADD_KEY(RButton);
	SCGE_ADD_KEY(MButton);
	SCGE_ADD_KEY(Button4);
	SCGE_ADD_KEY(Button5);
	SCGE_ADD_KEY(WheelUp);
	SCGE_ADD_KEY(WheelDown);

	SCGE_ADD_KEY(Pause);
	SCGE_ADD_KEY(Menu);
	SCGE_ADD_KEY(Escape);
	SCGE_ADD_KEY(CapsLock);
	SCGE_ADD_KEY(LCtrl);
	SCGE_ADD_KEY(LShift);
	SCGE_ADD_KEY(LAlt);
	SCGE_ADD_KEY(LSystem);
	SCGE_ADD_KEY(RCtrl);
	SCGE_ADD_KEY(RShift);
	SCGE_ADD_KEY(RAlt);
	SCGE_ADD_KEY(RSystem);

	SCGE_ADD_KEY(Left);
	SCGE_ADD_KEY(Right);
	SCGE_ADD_KEY(Up);
	SCGE_ADD_KEY(Down);

	SCGE_ADD_KEY(Num0);
	SCGE_ADD_KEY(Num1);
	SCGE_ADD_KEY(Num2);
	SCGE_ADD_KEY(Num3);
	SCGE_ADD_KEY(Num4);
	SCGE_ADD_KEY(Num5);
	SCGE_ADD_KEY(Num6);
	SCGE_ADD_KEY(Num7);
	SCGE_ADD_KEY(Num8);
	SCGE_ADD_KEY(Num9);

	SCGE_ADD_KEY(LBracket);
	SCGE_ADD_KEY(RBracket);
	SCGE_ADD_KEY(SemiColon);
	SCGE_ADD_KEY(Comma);
	SCGE_ADD_KEY(Period);
	SCGE_ADD_KEY(Quote);
	SCGE_ADD_KEY(Slash);
	SCGE_ADD_KEY(BackSlash);
	SCGE_ADD_KEY(Tilde);
	SCGE_ADD_KEY(Equal);
	SCGE_ADD_KEY(Dash);
	SCGE_ADD_KEY(Space);
	SCGE_ADD_KEY(Enter);
	SCGE_ADD_KEY(Tab);

	SCGE_ADD_KEY(Insert);
	SCGE_ADD_KEY(Delete);
	SCGE_ADD_KEY(Home);
	SCGE_ADD_KEY(End);
	SCGE_ADD_KEY(PageUp);
	SCGE_ADD_KEY(PageDown);

	SCGE_ADD_KEY(Add);
	SCGE_ADD_KEY(Subtract);
	SCGE_ADD_KEY(Multiply);
	SCGE_ADD_KEY(Divide);

	SCGE_ADD_KEY(A);
	SCGE_ADD_KEY(B);
	SCGE_ADD_KEY(C);
	SCGE_ADD_KEY(D);
	SCGE_ADD_KEY(E);
	SCGE_ADD_KEY(F);
	SCGE_ADD_KEY(G);
	SCGE_ADD_KEY(H);
	SCGE_ADD_KEY(I);
	SCGE_ADD_KEY(J);
	SCGE_ADD_KEY(K);
	SCGE_ADD_KEY(L);
	SCGE_ADD_KEY(M);
	SCGE_ADD_KEY(N);
	SCGE_ADD_KEY(O);
	SCGE_ADD_KEY(P);
	SCGE_ADD_KEY(Q);
	SCGE_ADD_KEY(R);
	SCGE_ADD_KEY(S);
	SCGE_ADD_KEY(T);
	SCGE_ADD_KEY(U);
	SCGE_ADD_KEY(V);
	SCGE_ADD_KEY(W);
	SCGE_ADD_KEY(X);
	SCGE_ADD_KEY(Y);
	SCGE_ADD_KEY(Z);

	#undef SCGE_ADD_KEY
}

InputDevice::InputDevice(Console& console)
	: mConsole(console)
	, mSensitivity(console, "Input.Sensitivity", "How mouse input is scaled", 0.125f)
	, mBind("Input.Bind", "Binds a key to a command")
	, mUnBind("Input.UnBind", "Removes a key binding")
	, mUnBindAll("Input.UnBindAll", "Removes all key bindings")
{
	fillBiMap(mKeyNameBiMap);

	mBind.initialise(console, [this](const std::vector<std::string> &arguments, bool silent){ bindKey(arguments[1], arguments[2], silent); }, 2, 2);
	mUnBind.initialise(console, [this](const std::vector<std::string> &arguments, bool silent){ unbindKey(arguments[1], silent); }, 1, 1);
	mUnBindAll.initialise(console, [this](const std::vector<std::string> &arguments, bool silent){ unbindAll(); });
}

InputDevice::~InputDevice()
{
	const std::string &bindName = mBind.getName();
	for(const auto &binding : mKeyBindings)
		binding.second.saveKeyBind(mConsole, bindName, GetKeyName(binding.first));
}

void InputDevice::Update()
{
	for(auto &consoleEvent : mAllEvents)
		consoleEvent.right->resetState();

	for(auto & binding : mKeyBindings)
	{
		if(IsKeyPress(binding.first)) binding.second.onKeyDown(mConsole);
		if(IsKeyRelease(binding.first)) binding.second.onKeyUp(mConsole);
		if(IsKeyDown(binding.first)) binding.second.onKeyHold();
	}
}

bool InputDevice::GetKeyToggleState(Key key) const
{
	switch(key)
	{
	case Key::CapsLock: return ((GetKeyState(VK_CAPITAL) & 0x0001) != 0);
	case Key::NumLock: return ((GetKeyState(VK_NUMLOCK) & 0x0001) != 0);
	case Key::ScrollLock: return ((GetKeyState(VK_SCROLL) & 0x0001) != 0);
	default: return false;
	}
}

bool InputDevice::IsKeyPress(Key key) const
{
	return (mKeyChangeStates[static_cast<int>(key)] & 1) != 0;
}

bool InputDevice::IsKeyRelease(Key key) const
{
	return (mKeyChangeStates[static_cast<int>(key)] & 2) != 0;
}

bool InputDevice::IsKeyDown(Key key) const
{
	return mKeyHoldStates[static_cast<int>(key)];
}

bool InputDevice::IsKeyUp(Key key) const
{
	return !mKeyHoldStates[static_cast<int>(key)];
}

Key InputDevice::getKey(const std::string &name) const
{
	auto key = mKeyNameBiMap.left.find(name);
	if(key == mKeyNameBiMap.left.end()) return Key::Unknown;
	else return key->second;
}

const std::string &InputDevice::GetKeyName(Key key) const
{
	static const std::string unknownKey("UnknownKey");

	auto name = mKeyNameBiMap.right.find(key);
	if(name == mKeyNameBiMap.right.end()) return unknownKey;
	else return name->second;
}

void InputDevice::CreateEventKeyDown(Key key)
{
	mKeyChangeStates[static_cast<int>(key)] |= 1;
	mKeyHoldStates[static_cast<int>(key)] = true;
}

void InputDevice::CreateEventKeyUp(Key key)
{
	mKeyChangeStates[static_cast<int>(key)] |= 2;
	mKeyHoldStates[static_cast<int>(key)] = false;
}

void InputDevice::CreateEventMousePosition(int x, int y)
{
	mCurrentXpos = x;
	mCurrentYpos = y;
}

void InputDevice::CreateEventMouseMove(float changedXpos, float changedYpos, float changedZpos)
{
	mChangedXpos += changedXpos * mSensitivity;
	mChangedYpos += changedYpos * mSensitivity;
	mChangedZpos += changedZpos;
}

void InputDevice::ResetState(bool fullReset)
{
	if(fullReset)
	{
		mKeyHoldStates.fill(false);

		mCurrentXpos = 0;
		mCurrentYpos = 0;
	}

	mKeyChangeStates.fill(0);

	mChangedXpos = 0;
	mChangedYpos = 0;
	mChangedZpos = 0;

	mSensitivity.updateValue();
}

bool InputDevice::hasKeyBind(Key key) const
{
	if(mKeyBindings.count(key) != 0)
		return true;

	return false;
}

void InputDevice::addDefaultBinding(Key key, ConsoleEvent &consoleEvent)
{
	if(key == Key::Unknown || key == Key::Count)
		return;

	mConsole.addDefaultCommand(mBind.getName() + " " + GetKeyName(key) + " " + consoleEvent.getName());
}

void InputDevice::bindKey(const std::string &key, const std::string &command, bool silent)
{
	if(key.empty())
		return;

	if(key[0] == '+' || key[0] == '-')
	{
		std::string actualKey = key.substr(1, key.length()-1);
		Key keyID = getKey(actualKey);
		if(keyID != Key::Unknown)
		{
			if(key[0] == L'-') bindKeyUp(keyID, command);
			else bindKeyDown(keyID, command);
		}
		else if(!silent)
			mConsole.printError("Unknown Key : " + actualKey);
	}
	else
	{
		Key keyID = getKey(key);
		if(keyID != Key::Unknown)
		{
			auto consoleCommand = mAllEvents.left.find(command);
			if(consoleCommand == mAllEvents.left.end()) mKeyBindings[keyID].setSavedBinding(command);
			else bindKey(keyID, *consoleCommand->second);
			
		}
		else if(!silent)
			mConsole.printError("Unknown Key : " + key);
	}
}

void InputDevice::bindKey(Key key, ConsoleEvent &consoleEvent)
{
	if(key == Key::Unknown || key == Key::Count)
		return;

	mKeyBindings[key].setConsoleEvent(consoleEvent);
}

void InputDevice::bindKeyUp(Key key, const std::string &command)
{
	if(key == Key::Unknown || key == Key::Count)
		return;

	mKeyBindings[key].setRawKeyUp(command);
}

void InputDevice::bindKeyDown(Key key, const std::string &command)
{
	if(key == Key::Unknown || key == Key::Count)
		return;

	mKeyBindings[key].setRawKeyDown(command);
}

void InputDevice::unbindKey(const std::string &key, bool silent)
{
	Key keyID = getKey(key);
	if(keyID != Key::Unknown)
		unbindKey(keyID);
	else if(!silent)
		mConsole.printError("Unknown Key : " + key);
}

void InputDevice::unbindKey(Key key)
{
	mKeyBindings.erase(key);
}

void InputDevice::unbindAll()
{
	mKeyBindings.clear();
}

bool InputDevice::registerConsoleEvent(ConsoleEvent &consoleEvent)
{
	if(!mAllEvents.insert(EventBiMap::value_type(consoleEvent.getName(), &consoleEvent)).second)
		return false;

	for(auto & keybinding : mKeyBindings)
		keybinding.second.onRegisterConsoleEvent(consoleEvent);

	return true;
}

void InputDevice::unregisterConsoleEvent(ConsoleEvent &consoleEvent)
{
	mAllEvents.right.erase(&consoleEvent);

	for(auto & keybinding : mKeyBindings)
		keybinding.second.onUnregisterConsoleEvent(consoleEvent);
}