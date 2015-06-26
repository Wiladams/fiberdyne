#pragma once

#include "Scheduler.h"
#include "../core/StopWatch.h"
#include "TEXTask.h"

#include <queue>
#include <functional>

namespace fiberdyne {

class WaitForTime
{
private:
	StopWatch	m_Clock;
	bool m_ContinueRunning;
	Scheduler &m_Scheduler;

	std::priority_queue<std::pair<double, std::shared_ptr<TEXTask>>, std::vector<std::pair<double, std::shared_ptr<TEXTask> >>, std::greater<std::pair<double,std::shared_ptr<TEXTask>>>> m_TasksWaitingForTime; // List of tasks waiting for time to expire


public:
	WaitForTime(Scheduler &);

	virtual void operator()();
	//virtual void operator()(AProcessor *proc);

	bool tasksArePending();
	size_t tasksPending();

	void yieldForTime(double millis, const std::shared_ptr<TEXTask> &atask);
	void yieldUntilTime(double atime, const std::shared_ptr<TEXTask> &atask);

};

}
