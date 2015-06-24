#pragma once

#include <stdint.h>

class StopWatch
{
private:
	double	m_Frequency;
	uint64_t	m_StartCount;

protected:
public:
	StopWatch(const uint64_t startcount= 0);

	void Reset();
	uint64_t GetCurrentTicks();
	double Seconds();
	double Milliseconds();
};
