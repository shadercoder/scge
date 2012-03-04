#include <scge\Window.h>

static unsigned int gExtraHeight = GetSystemMetrics(SM_CXFRAME)*2 + GetSystemMetrics(SM_CYCAPTION);
static unsigned int gExtraWidth = GetSystemMetrics(SM_CYFRAME)*2;

unsigned int Window::GetDesktopWidth()
{
	return GetSystemMetrics(SM_CXSCREEN);
}

unsigned int Window::GetDesktopHeight()
{
	return GetSystemMetrics(SM_CYSCREEN);
}

LRESULT CALLBACK StaticWindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	Window* pWindow = nullptr;

	if(msg == WM_NCCREATE)
	{
		pWindow = reinterpret_cast<Window*>((reinterpret_cast<LPCREATESTRUCT>(lParam))->lpCreateParams);

		SetWindowLong(hwnd, GWL_USERDATA, reinterpret_cast<long>(pWindow));

		pWindow->SetHwnd(hwnd);
	}
	else
		pWindow = reinterpret_cast<Window*>(GetWindowLong(hwnd, GWL_USERDATA));

	if(pWindow)
		return pWindow->WindowProc(msg, wParam, lParam);
	else
		return DefWindowProc(hwnd, msg, wParam, lParam);
}

Window::Window(const std::string &name, Console& console, ResourceManager& resourceManager, FileSystem &fileSystem, unsigned int defaultWidth, unsigned int defaultHeight, bool defaultWindowed, bool defaultMaximised)
	: mWidth(console, "Window.Width", "Width of the Game Window", 640, GetDesktopWidth(), defaultWidth)
	, mHeight(console, "Window.Height", "Height of the Game Window", 480, GetDesktopHeight(), defaultHeight)
	, mWindowed(console, "Window.Windowed", "Whether the window fill the screen or not", defaultWindowed)
	, mMaximised(console, "Window.Maximised", "Causes windowed mode to look like fullscreen mode", defaultMaximised)
	, mOverrideAltF4(console, "Window.OverrideAltF4", "Stops Alt+F4 closing the game", true)
	, mOverrideWindowsKey(console, "Window.OverrideWindowsKey", "Stops the windows key from minimising the game", false)
	, mMouseClippedDesired(console, "Window.LockMouse", "Locks the mouse to inside the window", false)
	, mWindowName(name)
	, mHwnd(nullptr)
	, mHInstance(GetModuleHandle(nullptr))
	, mInitialised(false)
	, mShouldExit(false)
	, mActive(false)
	, mHideMouse(false)
	, mMouseClipped(false)
	, mResourceManager(resourceManager)
	, mInputDevice(console)
	, mConsole(console)
	, mGraphicsDevice(console, resourceManager, fileSystem)
{
}

Window::~Window()
{
	if(mInitialised)
		Release();
}

bool Window::Initialise()
{
	if(mInitialised)
		return false;

	std::wstring windowName;
	windowName.assign(mWindowName.begin(), mWindowName.end());

	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = StaticWindowProc;
	wc.hInstance = mHInstance;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = reinterpret_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	wc.hIcon = LoadIcon(mHInstance, MAKEINTRESOURCE(IDI_APPLICATION));
	wc.lpszClassName = windowName.c_str();
	RegisterClassEx(&wc);

	mWidth.updateValue();
	mHeight.updateValue();
	mWindowed.updateValue();
	mMaximised.updateValue();

	if(mWindowed && !mMaximised)
	{
		RECT rect;
		rect.left = (GetSystemMetrics(SM_CXSCREEN) - static_cast<int>(mWidth + gExtraWidth)) / 2;
		rect.top = (GetSystemMetrics(SM_CYSCREEN) - static_cast<int>(mHeight + gExtraHeight)) / 2;
		rect.right = rect.left + mWidth;
		rect.bottom = rect.top + mHeight;
		AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, false, 0);

		CreateWindowEx(0,
			windowName.c_str(), windowName.c_str(),
			WS_OVERLAPPEDWINDOW,
			rect.left, rect.top,
			rect.right - rect.left, rect.bottom - rect.top,
			nullptr, nullptr,
			mHInstance, this);
	}
	else
	{
		CreateWindowEx(0,
			windowName.c_str(), windowName.c_str(),
			WS_POPUP | WS_VISIBLE,
			0, 0,
			mWidth, mHeight,
			nullptr, nullptr,
			mHInstance, this);
	}

	if(!mHwnd)
	{
		mConsole.printError("CreateWindowEx() failed");
		return true;
	}

	mActive = true;
	mMouseClipped = false;

	RAWINPUTDEVICE Rid = {0};
	Rid.usUsagePage = 0x01; //Generic
	Rid.usUsage = 0x02; //Mouse
	Rid.dwFlags = 0;
	Rid.hwndTarget = mHwnd;
	RegisterRawInputDevices(&Rid, 1, sizeof(Rid));

	mInputDevice.ResetState(true);

	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(mHwnd, &pt);
	mInputDevice.CreateEventMousePosition(static_cast<unsigned short>(pt.x), static_cast<unsigned short>(pt.y));

	if(mGraphicsDevice.Initialise(mHwnd, mWindowed, mWidth, mHeight))
	{
		DestroyWindow(mHwnd);
		PumpWindowMessages();
		return true;
	}

	ShowWindow(mHwnd, SW_SHOWNORMAL);

	mInitialised = true;
	mShouldExit = false;

	return false;
}

void Window::Release()
{
	if(!mInitialised)
		return;

	mGraphicsDevice.Release();
	mInputDevice.ResetState(true);

	if(mHwnd)
	{
		DestroyWindow(mHwnd);
		PumpWindowMessages();
		mHwnd = nullptr;
	}

	std::wstring windowName;
	windowName.assign(mWindowName.begin(), mWindowName.end());
	UnregisterClass(windowName.c_str(), mHInstance);

	mInitialised = false;
}

void Window::PumpWindowMessages()
{
	MSG msg = {};
	while(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

LRESULT Window::WindowProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if(!mInitialised)
		return DefWindowProc(mHwnd, msg, wParam, lParam);

	switch(msg)
	{
	case WM_DESTROY:
		mShouldExit = true;
		return false;

	case WM_MOVE:
		ClipCursor(nullptr);
		mMouseClipped = false;
		break;

	case WM_SIZING:
		{
			LPRECT lprc = reinterpret_cast<LPRECT>(lParam);

			unsigned int minWidth = mWidth.getMinValue();
			unsigned int maxWidth = mWidth.getMaxValue();
			unsigned int minHeight = mHeight.getMinValue();
			unsigned int maxHeight = mHeight.getMaxValue();

			if(mWindowed && !mMaximised)
			{
				minWidth += gExtraWidth;
				minHeight += gExtraHeight;
			}

			unsigned int newWidth = lprc->right - lprc->left;
			int widthChange = 0;
			if(newWidth < minWidth)
				widthChange = minWidth - newWidth;
			else if(newWidth > maxWidth)
				widthChange = -int(newWidth - maxWidth);

			if(widthChange != 0)
			{
				if(wParam == WMSZ_TOPLEFT || wParam == WMSZ_LEFT || wParam == WMSZ_BOTTOMLEFT)
					lprc->left -= widthChange;
				else if(wParam == WMSZ_TOPRIGHT || wParam == WMSZ_RIGHT || wParam == WMSZ_BOTTOMRIGHT)
					lprc->right += widthChange;
			}

			unsigned int newHeight = lprc->bottom - lprc->top;
			int heightChange = 0;
			if(newHeight < minHeight)
				heightChange = minHeight - newHeight;
			else if(newHeight > maxHeight)
				heightChange = -int(newHeight - maxHeight);

			if(heightChange != 0)
			{
				if(wParam == WMSZ_TOPLEFT || wParam == WMSZ_TOP || wParam == WMSZ_TOPRIGHT)
					lprc->top -= heightChange;
				else if(wParam == WMSZ_BOTTOMLEFT || wParam == WMSZ_BOTTOM || wParam == WMSZ_BOTTOMRIGHT)
					lprc->bottom += heightChange;
			}

			return heightChange != 0 || widthChange != 0;
		}
		
	case WM_SIZE:
		mWidth.setValue(LOWORD(lParam));
		mHeight.setValue(HIWORD(lParam));

		ClipCursor(nullptr);
		mMouseClipped = false;
		return false;

	case WM_ACTIVATE:
		{
			ClipCursor(nullptr);
			mMouseClipped = false;

			bool bActivating = (LOWORD(wParam) != WA_INACTIVE) && (HIWORD(wParam) == 0);
			if(mActive != bActivating)
				FocusChange(bActivating);

			return false;
		}

	case WM_SETCURSOR:
		ClipCursor(nullptr);
		mMouseClipped = false;

		if(mActive && LOWORD(lParam) == HTCLIENT && mHideMouse)
		{
			SetCursor(nullptr);
			return true;
		}
		break;

	case WM_SYSKEYDOWN:
		mOverrideAltF4.updateValue();
		if(wParam == VK_F4 && !mOverrideAltF4)
			mShouldExit = true;
		case WM_KEYDOWN:
		if((HIWORD(lParam) & KF_REPEAT) == 0)
			mInputDevice.CreateEventKeyDown(VirtualKeyCodeToSombre(wParam, lParam));
		return false;

	case WM_SYSKEYUP:
	case WM_KEYUP:
		mInputDevice.CreateEventKeyUp(VirtualKeyCodeToSombre(wParam, lParam));
		return false;

	case WM_LBUTTONDOWN:
		SetFocus(mHwnd);
		mInputDevice.CreateEventKeyDown(Key::LButton);
		return false;
	case WM_MBUTTONDOWN:
		SetFocus(mHwnd);
		mInputDevice.CreateEventKeyDown(Key::MButton);
		return false;
	case WM_RBUTTONDOWN:
		SetFocus(mHwnd);
		mInputDevice.CreateEventKeyDown(Key::RButton);
		return false;
	case WM_XBUTTONDOWN:
		SetFocus(mHwnd);
		if(GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
			mInputDevice.CreateEventKeyDown(Key::Button4);
		else if(GET_XBUTTON_WPARAM(wParam) == XBUTTON2)
			mInputDevice.CreateEventKeyDown(Key::Button5);
		return false;

	case WM_LBUTTONDBLCLK:
	case WM_MBUTTONDBLCLK:
	case WM_RBUTTONDBLCLK:
	case WM_XBUTTONDBLCLK:
		return false;

	case WM_LBUTTONUP:
		mInputDevice.CreateEventKeyUp(Key::LButton);
		return false;
	case WM_MBUTTONUP:
		mInputDevice.CreateEventKeyUp(Key::MButton);
		return false;
	case WM_RBUTTONUP:
		mInputDevice.CreateEventKeyUp(Key::RButton);
		return false;
	case WM_XBUTTONUP:
		if(GET_XBUTTON_WPARAM(wParam) == XBUTTON1)
			mInputDevice.CreateEventKeyUp(Key::Button4);
		else if(GET_XBUTTON_WPARAM(wParam) == XBUTTON2)
			mInputDevice.CreateEventKeyUp(Key::Button5);
		return false;

	case WM_MOUSEMOVE:
		mInputDevice.CreateEventMousePosition(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
		return false;

	case WM_MOUSEWHEEL:
		if(GET_WHEEL_DELTA_WPARAM(wParam) > 0)
		{
			mInputDevice.CreateEventKeyDown(Key::WheelUp);
			mInputDevice.CreateEventKeyUp(Key::WheelUp);
		}
		else
		{
			mInputDevice.CreateEventKeyDown(Key::WheelDown);
			mInputDevice.CreateEventKeyUp(Key::WheelDown);
		}
		return false;

	case WM_INPUT:
		{
			HRAWINPUT rawInput = reinterpret_cast<HRAWINPUT>(lParam);

			UINT size = 0;
			GetRawInputData(rawInput, RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER));

			if(size == 0)
				return false;

			std::unique_ptr<BYTE[]> bytes(new BYTE[size]);

			GetRawInputData(rawInput, RID_INPUT, bytes.get(), &size, sizeof(RAWINPUTHEADER));

			RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(bytes.get());

			if(raw->header.dwType == RIM_TYPEMOUSE)
			{
				if((raw->data.mouse.usFlags & MOUSE_MOVE_RELATIVE) == MOUSE_MOVE_RELATIVE)
					mInputDevice.CreateEventMouseMove(static_cast<float>(raw->data.mouse.lLastX), static_cast<float>(raw->data.mouse.lLastY), 0.0f);

				if((raw->data.mouse.usButtonFlags & RI_MOUSE_WHEEL) == RI_MOUSE_WHEEL)
					mInputDevice.CreateEventMouseMove(0.0f, 0.0f, static_cast<float>(raw->data.mouse.usButtonData));
			}
		}
		return false;

	case WM_SYSCOMMAND:
		if(wParam == SC_CLOSE)
		{
			mShouldExit = true;
			return false;
		}
		else if(wParam == SC_MAXIMIZE || wParam == SC_RESTORE)
		{
			ClipCursor(nullptr);
			mMouseClipped = false;

			RECT rcWindow = {};
			GetClientRect(mHwnd, &rcWindow);
			mWidth.setValue(rcWindow.right-rcWindow.left);
			mHeight.setValue(rcWindow.bottom-rcWindow.top);
		}
		else if(wParam == SC_MINIMIZE)
		{
			ClipCursor(nullptr);
			mMouseClipped = false;
		}
		break;
	}

	return DefWindowProc(mHwnd, msg, wParam, lParam);
}

void Window::FocusChange(bool active)
{
	mActive = active;

	if(!mActive)
		mInputDevice.ResetState(true);
}

bool Window::SizeChange()
{
	bool widthChanged = mWidth.hasValueChanged();
	bool heightChanged = mHeight.hasValueChanged();
	bool windowedChanged = mWindowed.hasValueChanged();
	bool maximisedChanged = mMaximised.hasValueChanged();

	bool currentBorders = mWindowed && !mMaximised;

	mWindowed.updateValue();
	mMaximised.updateValue();
	mWidth.updateValue();
	mHeight.updateValue();

	bool shouldHaveBorders = mWindowed && !mMaximised;

	if(widthChanged || heightChanged || windowedChanged)
	{
		if(mGraphicsDevice.onResize(mWindowed, mWidth, mHeight))
			return true;
	}

	if(windowedChanged || maximisedChanged)
	{
		if(shouldHaveBorders)
		{
			if(!currentBorders)
				SetWindowLong(mHwnd, GWL_STYLE, WS_OVERLAPPEDWINDOW);
		}
		else
		{
			if(currentBorders)
				SetWindowLong(mHwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);
		}
	}

	RECT rcWindow = {};
	GetClientRect(mHwnd, &rcWindow);
	if(mWidth != rcWindow.right-rcWindow.left || mHeight != rcWindow.bottom-rcWindow.top || shouldHaveBorders != currentBorders)
	{
		if(mWindowed && !mMaximised)
		{
			RECT rect;
			rect.left = (GetSystemMetrics(SM_CXSCREEN) - static_cast<int>(mWidth + gExtraWidth)) / 2;
			rect.top = (GetSystemMetrics(SM_CYSCREEN) - static_cast<int>(mHeight + gExtraHeight)) / 2;
			rect.right = rect.left + mWidth;
			rect.bottom = rect.top + mHeight;

			AdjustWindowRectEx(&rect, WS_OVERLAPPEDWINDOW, false, 0);

			SetWindowPos(mHwnd, HWND_TOP, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, SWP_NOZORDER | SWP_SHOWWINDOW);
		}
		else
			SetWindowPos(mHwnd, HWND_TOP, 0, 0, mWidth, mHeight, SWP_NOZORDER | SWP_SHOWWINDOW);
	}

	return false;
}

bool Window::Update()
{
	mInputDevice.ResetState();
	PumpWindowMessages();

	if(mWidth.hasValueChanged() || mHeight.hasValueChanged() || mWindowed.hasValueChanged() || mMaximised.hasValueChanged())
	{
		if(SizeChange())
			return true;
	}

	mMouseClippedDesired.updateValue();
	if((mMouseClipped != mMouseClippedDesired) && mActive)
		adjustClipping();

	return mShouldExit;
}

bool Window::Render()
{
	return mShouldExit || mGraphicsDevice.Render();
}

void Window::adjustClipping()
{
	if(mMouseClippedDesired)
	{
		RECT rct;
		GetClientRect(mHwnd, &rct);
		ClientToScreen(mHwnd, reinterpret_cast<LPPOINT>(&rct.left));
		ClientToScreen(mHwnd, reinterpret_cast<LPPOINT>(&rct.right));
		ClipCursor(&rct);
	}
	else
		ClipCursor(nullptr);

	mMouseClipped = mMouseClippedDesired;
}

Key Window::VirtualKeyCodeToSombre(WPARAM VirtualKey, LPARAM Flags)
{
	switch (VirtualKey)
	{
	case VK_SHIFT :
		{
			static UINT LShift = MapVirtualKey(VK_LSHIFT, MAPVK_VK_TO_VSC);
			UINT scancode = (Flags & (0xFF << 16)) >> 16;
			return scancode == LShift ? Key::LShift : Key::RShift;
		}

	case VK_MENU : return (HIWORD(Flags) & KF_EXTENDED) ? Key::RAlt : Key::LAlt;
	case VK_CONTROL : return (HIWORD(Flags) & KF_EXTENDED) ? Key::RCtrl : Key::LCtrl;

	case VK_NUMPAD0 : return Key::Numpad0;
	case VK_NUMPAD1 : return Key::Numpad1;
	case VK_NUMPAD2 : return Key::Numpad2;
	case VK_NUMPAD3 : return Key::Numpad3;
	case VK_NUMPAD4 : return Key::Numpad4;
	case VK_NUMPAD5 : return Key::Numpad5;
	case VK_NUMPAD6 : return Key::Numpad6;
	case VK_NUMPAD7 : return Key::Numpad7;
	case VK_NUMPAD8 : return Key::Numpad8;
	case VK_NUMPAD9 : return Key::Numpad9;

	case VK_F1 :  return Key::F1;
	case VK_F2 :  return Key::F2;
	case VK_F3 :  return Key::F3;
	case VK_F4 :  return Key::F4;
	case VK_F5 :  return Key::F5;
	case VK_F6 :  return Key::F6;
	case VK_F7 :  return Key::F7;
	case VK_F8 :  return Key::F8;
	case VK_F9 :  return Key::F9;
	case VK_F10 : return Key::F10;
	case VK_F11 : return Key::F11;
	case VK_F12 : return Key::F12;
	case VK_F13 : return Key::F13;
	case VK_F14 : return Key::F14;
	case VK_F15 : return Key::F15;

	case VK_PAUSE :  return Key::Pause;
	case VK_APPS :   return Key::Menu;
	case VK_ESCAPE : return Key::Escape;

	case VK_LWIN : return Key::LSystem;
	case VK_RWIN : return Key::RSystem;

	case VK_LEFT :  return Key::Left;
	case VK_RIGHT : return Key::Right;
	case VK_UP :    return Key::Up;
	case VK_DOWN :  return Key::Down;

	case '0' : return Key::Num0;
	case '1' : return Key::Num1;
	case '2' : return Key::Num2;
	case '3' : return Key::Num3;
	case '4' : return Key::Num4;
	case '5' : return Key::Num5;
	case '6' : return Key::Num6;
	case '7' : return Key::Num7;
	case '8' : return Key::Num8;
	case '9' : return Key::Num9;

	case VK_OEM_4 :      return Key::LBracket;
	case VK_OEM_6 :      return Key::RBracket;
	case VK_OEM_1 :      return Key::SemiColon;
	case VK_OEM_COMMA :  return Key::Comma;
	case VK_OEM_PERIOD : return Key::Period;
	case VK_OEM_7 :      return Key::Quote;
	case VK_OEM_2 :      return Key::Slash;
	case VK_OEM_5 :      return Key::BackSlash;
	case VK_OEM_3 :      return Key::Tilde;
	case VK_OEM_PLUS :   return Key::Equal;
	case VK_OEM_MINUS :  return Key::Dash;
	case VK_SPACE :      return Key::Space;
	case VK_RETURN :     return Key::Enter;
	case VK_BACK :       return Key::BackSpace;
	case VK_TAB :        return Key::Tab;

	case VK_INSERT : return Key::Insert;
	case VK_DELETE : return Key::Delete;
	case VK_HOME :   return Key::Home;
	case VK_END :    return Key::End;
	case VK_PRIOR :  return Key::PageUp;
	case VK_NEXT :   return Key::PageDown;

	case VK_CAPITAL : return Key::CapsLock;
	case VK_NUMLOCK : return Key::NumLock;
	case VK_SCROLL :  return Key::ScrollLock;
	
	
	case VK_ADD :      return Key::Add;
	case VK_SUBTRACT : return Key::Subtract;
	case VK_MULTIPLY : return Key::Multiply;
	case VK_DIVIDE :   return Key::Divide;

	case 'A' : return Key::A;
	case 'B' : return Key::B;
	case 'C' : return Key::C;
	case 'D' : return Key::D;
	case 'E' : return Key::E;
	case 'F' : return Key::F;
	case 'G' : return Key::G;
	case 'H' : return Key::H;
	case 'I' : return Key::I;
	case 'J' : return Key::J;
	case 'K' : return Key::K;
	case 'L' : return Key::L;
	case 'M' : return Key::M;
	case 'N' : return Key::N;
	case 'O' : return Key::O;
	case 'P' : return Key::P;
	case 'Q' : return Key::Q;
	case 'R' : return Key::R;
	case 'S' : return Key::S;
	case 'T' : return Key::T;
	case 'U' : return Key::U;
	case 'V' : return Key::V;
	case 'W' : return Key::W;
	case 'X' : return Key::X;
	case 'Y' : return Key::Y;
	case 'Z' : return Key::Z;
	}

	return Key::Unknown;
}

void Window::SetMousePosition(int iXpos, int iYpos)
{
	POINT pt;
	pt.x = iXpos;
	pt.y = iYpos;

	ClientToScreen(mHwnd, &pt);
	SetCursorPos(pt.x, pt.y);
}