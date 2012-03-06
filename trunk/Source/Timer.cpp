#include "scge\Timer.h"

#include <algorithm>

#if defined(SCGE_SYSTEM_WINDOWS)
#include <windows.h>

uint64 getSystemTime()
{
	static LARGE_INTEGER sFrequency;
	static bool sUseHighPerformanceTimer = QueryPerformanceFrequency(&sFrequency) != 0;

	if(sUseHighPerformanceTimer)
	{
		LARGE_INTEGER currentTime;
		QueryPerformanceCounter(&currentTime);

		return currentTime.QuadPart * 1000 / sFrequency.QuadPart;
	}
	else
		return GetTickCount64();
}

#else

#include <sys/time.h>

uint64 getSystemTime()
{
	timeval time = {0, 0};
	gettimeofday(&time, nullptr);

	return time.tv_sec * 1000 + time.tv_usec / 1000;
}

#endif

Timer::Timer()
	: mStartTime(getSystemTime())
{}

uint64 Timer::getElapsed() const
{
	return getSystemTime() - mStartTime;
}

void Timer::Reset()
{
	mStartTime = getSystemTime();
}

uint64 Timer::getElapsedReset()
{
	uint64 storedTime = getSystemTime();

	std::swap(storedTime, mStartTime);

	return mStartTime - storedTime;
}
