#include "scge\Warning.h"

#include "scge\Config.h"
#include <windows.h>

//-----------------------------------//

namespace Warning
{
	Result displayBox(const std::string & message, Type type)
	{
		std::wstring wMessage;
		wMessage.assign(message.begin(), message.end());

		switch(type)
		{
		case Type::Alert:
			::MessageBox(nullptr, wMessage.c_str(), L"Alert!", MB_ICONINFORMATION);
			return Result::Okay;
		case Type::Error:
		case Type::Assertion:
			wMessage += L"\nUse retry to debug.";
			switch(::MessageBox(nullptr, wMessage.c_str(), type == Type::Assertion ? L"Assertion!" : L"Error!", MB_ICONWARNING | MB_ABORTRETRYIGNORE))
			{
			case IDRETRY:
				return Result::Debug;
			case IDABORT:
				return Result::Exit;
			}
			return Result::Continue;
		}

		return Result::Okay;
	}
}

//-----------------------------------//