#ifndef __Window_h__
#define __Window_h__

#include "scge\InputDevice.h"
#include "scge\Console.h"
#include "scge\ResourceManager.h"
#include "scge\GraphicsDevice.h"

#include "scge\Config.h"
#include <Windows.h>
#include <windowsx.h>

class Window
{
public:
	Window(std::string name, Console& console, ResourceManager& resourceManager, FileSystem &fileSystem, unsigned int defaultWidth = GetDesktopWidth(), unsigned int defaultHeight = GetDesktopHeight(), bool defaultWindowed = true, bool defaultMaximised = true);
	~Window();

	LRESULT WindowProc(UINT msg, WPARAM wParam, LPARAM lParam);

	void SetHwnd(HWND Hwnd) { mHwnd = Hwnd; }

	bool Initialise();
	void Release();

	bool Update();
	bool Render();

	unsigned int GetWidth() { return mWidth; }
	unsigned int GetHeight() { return mHeight; }

	void SetWidth(int width) { mWidth.setValue(width); }
	void SetHeight(int height) { mHeight.setValue(height); }

	bool IsActive() { return mActive; }

	bool IsWindowed() { return mWindowed; }
	void SetWindowed(bool windowed) { mWindowed.setValue(windowed); }

	bool IsMaximised() { return mMaximised; }
	void SetMaximised(bool maximised) { mMaximised.setValue(maximised); }

	static unsigned int GetDesktopWidth();
	static unsigned int GetDesktopHeight();

	InputDevice &GetInputDevice() { return mInputDevice; }
	GraphicsDevice& GetGraphicsDevice() { return mGraphicsDevice; }

	void setMouseClipping(bool clip) { mMouseClippedDesired.setValue(clip); }
	bool isMouseClipped() const { return mMouseClippedDesired; }

	void SetMousePosition(int iXpos, int iYpos);

	void setMouseVisibility(bool isHidden) { mHideMouse = isHidden; }
	bool getMouseVisiblity() const { return mHideMouse; }

private:
	std::string mWindowName;
	HWND mHwnd;
	HINSTANCE mHInstance;

	ConsoleVariable<unsigned int, true> mWidth;
	ConsoleVariable<unsigned int, true> mHeight;
	ConsoleVariable<bool> mWindowed;
	ConsoleVariable<bool> mMaximised;
	ConsoleVariable<bool> mOverrideAltF4;
	ConsoleVariable<bool> mOverrideWindowsKey;
	ConsoleVariable<bool> mMouseClippedDesired;

	bool mInitialised;
	bool mShouldExit;
	bool mActive;
	bool mHideMouse;
	bool mMouseClipped;

	GraphicsDevice mGraphicsDevice;
	InputDevice mInputDevice;

	ResourceManager& mResourceManager;
	Console& mConsole;

	void PumpWindowMessages();
	void FocusChange(bool active);
	bool SizeChange();
	void adjustClipping();

	static Key VirtualKeyCodeToSombre(WPARAM VirtualKey, LPARAM Flags);

	Window(const Window &other);
	Window &operator=(const Window &other);
};

#endif // __Window_h__