
#include "StopWatch.h"

#include <Windows.h>

#include <iostream>

StopWatch::StopWatch(const uint64_t startcount)
{
	uint64_t freqbuff;

	if (!QueryPerformanceFrequency((LARGE_INTEGER *)&freqbuff))  {
		printf("NO High Performance Frequency!!\n");
	}

	m_Frequency = (double)(1.0 / freqbuff);

	Reset();
}


uint64_t StopWatch::GetCurrentTicks()
{
	uint64_t count = 0;

	if (QueryPerformanceCounter((LARGE_INTEGER *)&count)) {
		return count;
	}

	return 0;
}

/*
/// <summary>
/// Reset the startCount, which is the current tick count.
/// This will reset the elapsed time because elapsed time is the
/// difference between the current tick count, and the one that
/// was set here in the Reset() call.
/// </summary>
*/

void StopWatch::Reset()
{
	uint64_t countbuff;

	if (!QueryPerformanceCounter((LARGE_INTEGER *)&countbuff)) {
		printf("NO High Performance Counter!!\n");
	}

	m_StartCount = countbuff;
}

/*
// <summary>
// Return the number of seconds that elapsed since Reset() was called.
// </summary>
// <returns>The number of elapsed seconds.</returns>
*/

double StopWatch::Seconds()
{
	uint64_t currentCount;
	QueryPerformanceCounter((LARGE_INTEGER *)&currentCount);

	uint64_t ellapsed = currentCount - m_StartCount;
	double seconds = (double)(ellapsed * m_Frequency);

	//printf("   start: %llu\n", m_StartCount);
	//printf(" current: %llu\n", currentCount);
	//printf("ellapsed: %llu\n", ellapsed);
	//printf("frequency: %f\n", m_Frequency);

	return seconds;
}

double StopWatch::Milliseconds()
{
	return Seconds() * 1000;
}
