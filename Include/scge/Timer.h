#ifndef Timer_h__
#define Timer_h__

#include "scge\Config.h"

class Timer
{
public:
	Timer();

	uint64 getElapsed() const;
	uint64 getElapsedReset();
	void Reset();

private:
	uint64 mStartTime;
};

#endif // Timer_h__