#ifndef Warning_h__
#define Warning_h__

#include "scge\Config.h"
#include "scge\Utility.h"

//-----------------------------------//

namespace Warning
{
	enum class Type
	{
		Alert,
		Assertion,
		Error
	};

	enum class Result
	{
		Okay,
		Continue,
		Debug,
		Exit
	};

	Result displayBox(const std::string & message, Type type = Type::Alert);
}

//-----------------------------------//

#if defined(SCGE_DEBUG)
#	define SCGE_ASSERT(x) do { if(!(x)) { Warning::Result result = Warning::displayBox(StringUtility::format("(%) Failed!\nIn function % at line % in file %", #x, __FUNCTION__, __LINE__, __FILE__), Warning::Type::Assertion); if(result == Warning::Result::Debug) __debugbreak(); else if(result == Warning::Result::Exit) exit(-1); } } while(false)
#	define SCGE_ASSERT_MESSAGE(x, y) do { if(!(x)) { Warning::Result result = Warning::displayBox(StringUtility::format("(%) Failed!\n%\nIn function % at line % in file %", #x, y, __FUNCTION__, __LINE__, __FILE__), Warning::Type::Assertion); if(result == Warning::Result::Debug) __debugbreak(); else if(result == Warning::Result::Exit) exit(-1); } } while(false)
#	define SCGE_ERROR(x) do { Warning::Result result = Warning::displayBox(StringUtility::format("%\nIn function % at line % in file %", x, __FUNCTION__, __LINE__, __FILE__), Warning::Type::Error); if(result == Warning::Result::Debug) __debugbreak(); else if(result == Warning::Result::Exit) exit(-1); } while(false)
#else
#	define SCGE_ASSERT(x) 
#	define SCGE_ASSERT_MESSAGE(x, y) 
#	define SCGE_ERROR(x) 
#endif

//-----------------------------------//

#endif // Warning_h__