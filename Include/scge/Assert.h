#ifndef Assert_h__
#define Assert_h__

#include "scge\Config.h"

#include <string>

namespace Messaging
{
	enum class Type
	{
		Alert,
		Assertion,
		Error
	};

	void showMessageBox(const std::string & message, Type messageType = Type::Alert);
}

#if defined(SCGE_DEBUG)
#define SCGE_ASSERT(x) if(!(x)) Messaging::showMessageBox(#x, Messaging::Type::Assertion)
#define SCGE_ASSERT_MESSAGE(x, y) if(!(x)) Messaging::showMessageBox(y, Messaging::Type::Assertion)
#define SCGE_ERROR(x) Messaging::showMessageBox(x, Messaging::Type::Error)
#define SCGE_ERROR_IF(x, y) if(x) Messaging::showMessageBox(y, Messaging::Type::Error)
#else
#define SCGE_ASSERT(x) 
#define SCGE_ASSERT_MESSAGE(x, y) 
#define SCGE_ERROR(x) 
#define SCGE_ERROR_IF(x, y) 
#endif

#endif // Assert_h__