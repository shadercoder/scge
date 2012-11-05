#ifndef Config_h__
#define Config_h__

#if defined(_WIN32) || defined(__WIN32__)
#	define SCGE_SYSTEM_WINDOWS
#	ifndef WIN32_LEAN_AND_MEAN
#		define WIN32_LEAN_AND_MEAN
#	endif
#	ifndef NOMINMAX
#		define NOMINMAX
#	endif
#elif defined(linux) || defined(__linux)
#	define SCGE_SYSTEM_LINUX
#elif defined(__APPLE__) || defined(MACOSX) || defined(macintosh) || defined(Macintosh)
#	define SCGE_SYSTEM_MACOS
#elif defined(__FreeBSD__) || defined(__FreeBSD_kernel__)
#	define SCGE_SYSTEM_FREEBSD
#else
#	error This operating system is not supported by scge
#endif

#if !defined(NDEBUG)
#	define SCGE_DEBUG
#endif

#include <climits>
#include <stdint.h>

typedef int8_t int8;
typedef uint8_t uint8;

typedef int16_t int16;
typedef uint16_t uint16;

typedef int32_t int32;
typedef uint32_t uint32;

typedef int64_t int64;
typedef uint64_t uint64;

typedef float float32;
typedef double float64;

static_assert((CHAR_BIT / sizeof(char)) == 8, "On this platform there are not 8 bits in a byte");

static_assert(sizeof(int8) == 1 && sizeof(uint8) == 1, "On this platform, uint8 and int8 are not 8 bit");
static_assert(sizeof(int16) == 2 && sizeof(uint16) == 2, "On this platform, uint16 and int16 are not 16 bit");
static_assert(sizeof(int32) == 4 && sizeof(uint32) == 4, "On this platform, uint32 and int32 are not 32 bit");
static_assert(sizeof(int64) == 8 && sizeof(uint64) == 8, "On this platform, int64 and uint64 are not 64 bit");

static_assert(sizeof(float32) == 4, "On this platform, float32 is not 32 bit");
static_assert(sizeof(float64) == 8, "On this platform, float64 is not 64 bit");

#endif // Config_h__