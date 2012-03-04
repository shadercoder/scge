#ifndef WindowHandle_h__
#define WindowHandle_h__

#include "scge\Config.h"

#if defined(SCGE_SYSTEM_WINDOWS)
	typedef void* WindowHandle;
#elif defined(SCGE_SYSTEM_LINUX) || defined(SCGE_SYSTEM_FREEBSD)
	typedef unsigned long WindowHandle;
#elif defined(SCGE_SYSTEM_MACOS)
	typedef void* WindowHandle;
#endif

#endif // WindowHandle_h__