#pragma once

#include "TEXTask.h"
#include "Scheduler.h"

#include <queue>
#include <functional>

#include <concurrent_queue.h>

namespace fiberdyne {

class WaitForCondition
{
private:
	Scheduler &m_Scheduler;
	bool m_ContinueRunning;
	std::queue<std::pair<std::shared_ptr<TEXTask>, std::function<bool()>>> m_FibersAwaitingCondition;
	//concurrency::concurrent_queue<std::pair<std::shared_ptr<TEXTask>, std::function<bool()>>> m_FibersAwaitingCondition;

public:
	WaitForCondition(Scheduler &);

	virtual void operator()();

	bool tasksArePending();
	size_t tasksPending();

	void yieldForCondition(std::function<bool()> f, const std::shared_ptr<TEXTask> &atask);

};

}
