#include "scge\Assert.h"

#include "scge\Config.h"

#include <windows.h>

void Messaging::showMessageBox(const std::string & message, Type messageType)
{
	std::wstring wMessage;
	wMessage.assign(message.begin(), message.end());

	switch(messageType)
	{
	case Type::Alert:
		::MessageBox(nullptr, wMessage.c_str(), L"Alert!", MB_ICONINFORMATION); break;
	case Type::Error:
		wMessage += L"\nWould you like to debug?";
		if(::MessageBox(nullptr, wMessage.c_str(), L"Error!", MB_ICONERROR | MB_YESNO) == IDYES)
			__debugbreak();
		else
			exit(-1);
		break;
	case Type::Assertion:
		wMessage += L"\nUse retry to debug.";
		switch(::MessageBox(nullptr, wMessage.c_str(), L"Assertion!", MB_ICONWARNING | MB_ABORTRETRYIGNORE))
		{
		case IDRETRY:
			__debugbreak(); break;
		case IDABORT:
			exit(-1);
		}
		break;
	}
}