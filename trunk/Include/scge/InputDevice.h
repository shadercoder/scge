#ifndef __InputDevice_h__
#define __InputDevice_h__

#include "scge\Console.h"
#include "scge\InputDevice\KeyBinding.h"

#include <array>
#include <map>
#include <boost\bimap.hpp>
#include <boost\bimap\unordered_set_of.hpp>

enum class Key
{
	Unknown = 0,

	Numpad0,
	Numpad1,
	Numpad2,
	Numpad3,
	Numpad4,
	Numpad5,
	Numpad6,
	Numpad7,
	Numpad8,
	Numpad9,

	F1,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12,
	F13,
	F14,
	F15,

	LButton,
	RButton,
	MButton,
	Button4,
	Button5,
	WheelUp,
	WheelDown,

	Pause,
	Menu,
	Escape,

	LCtrl,
	RCtrl,
	LShift,
	RShift,
	LAlt,
	RAlt,
	LSystem,
	RSystem,

	Left,
	Right,
	Up,
	Down,

	Num0 = '0',
	Num1 = '1',
	Num2 = '2',
	Num3 = '3',
	Num4 = '4',
	Num5 = '5',
	Num6 = '6',
	Num7 = '7',
	Num8 = '8',
	Num9 = '9',

	LBracket,
	RBracket,
	SemiColon,
	Comma,
	Period,
	Quote,
	Slash,
	BackSlash,
	Tilde,
	Equal,
	Dash,
	Space,
	Enter,
	BackSpace,
	Tab,
		
	Insert,
	Delete,
	Home,
	End,
	PageUp,
	PageDown,
		
	CapsLock,
	NumLock,
	ScrollLock,

	Add,
	Subtract,
	Multiply,
	Divide,

	A = 'a',
	B = 'b',
	C = 'c',
	D = 'd',
	E = 'e',
	F = 'f',
	G = 'g',
	H = 'h',
	I = 'i',
	J = 'j',
	K = 'k',
	L = 'l',
	M = 'm',
	N = 'n',
	O = 'o',
	P = 'p',
	Q = 'q',
	R = 'r',
	S = 's',
	T = 't',
	U = 'u',
	V = 'v',
	W = 'w',
	X = 'x',
	Y = 'y',
	Z = 'z',

	Count
};

class ConsoleEvent;
class InputDevice
{
public:
	InputDevice(Console& console);
	~InputDevice();

	void Update();

	void CreateEventKeyDown(Key key);
	void CreateEventKeyUp(Key key);

	void CreateEventMousePosition(int x, int y);
	void CreateEventMouseMove(float changedXpos, float changedYpos, float changedZpos);

	int GetMouseXPositionAbsolute() const { return mCurrentXpos; }
	int GetMouseYPositionAbsolute() const { return mCurrentYpos; }

	float GetMouseXPositionRelative() const { return mChangedXpos; }
	float GetMouseYPositionRelative() const { return mChangedYpos; }
	float GetMouseZPositionRelative() const { return mChangedZpos; }

	bool GetKeyToggleState(Key key) const;
	bool IsKeyPress(Key key) const;
	bool IsKeyRelease(Key key) const;
	bool IsKeyDown(Key key) const;
	bool IsKeyUp(Key key) const;

	Key getKey(const std::string &name) const;
	const std::string &GetKeyName(Key key) const;

	void ResetState(bool fullReset = false);

	bool hasKeyBind(Key key) const;

	void addDefaultBinding(Key key, ConsoleEvent &consoleEvent);

	void bindKey(const std::string &key, const std::string &command, bool silent);
	void bindKey(Key key, ConsoleEvent &consoleEvent);
	void bindKeyUp(Key key, const std::string &command);
	void bindKeyDown(Key key, const std::string &command);

	void unbindKey(const std::string &key, bool silent);
	void unbindKey(Key key);
	void unbindAll();

	bool registerConsoleEvent(ConsoleEvent &consoleEvent);
	void unregisterConsoleEvent(ConsoleEvent &consoleEvent);

private:
	Console& mConsole;

	int	mCurrentXpos;
	int	mCurrentYpos;

	float mChangedXpos;
	float mChangedYpos;
	float mChangedZpos;

	std::array<char, static_cast<unsigned int>(Key::Count)> mKeyChangeStates;
	std::array<bool, static_cast<unsigned int>(Key::Count)> mKeyHoldStates;

	boost::bimap<boost::bimaps::unordered_set_of<std::string>, boost::bimaps::set_of<Key>> mKeyNameBiMap;

	typedef boost::bimap<boost::bimaps::unordered_set_of<std::string>, boost::bimaps::set_of<ConsoleEvent*>> EventBiMap;
	EventBiMap mAllEvents;

	std::map<Key, KeyBinding> mKeyBindings;

	ConsoleVariable<float> mSensitivity;
	ConsoleFunction mBind;
	ConsoleFunction mUnBind;
	ConsoleFunction mUnBindAll;

	InputDevice(const InputDevice &other);
	InputDevice &operator=(const InputDevice &other);
};

#endif // __InputDevice_h__