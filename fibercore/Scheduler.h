#pragma once

#include "TEXTask.h"

#include <concurrent_queue.h>
#include <memory>

namespace fiberdyne {

class Scheduler
{
	//bool m_ContinueRunning;
	concurrency::concurrent_queue<std::shared_ptr<TEXTask>> 	m_ReadyList;	// The place we take input from
	std::shared_ptr<TEXTask>  m_CurrentTask;

protected:

public:
	Scheduler();

	virtual void ScheduleTask(const std::shared_ptr<TEXTask> &atask);	
	//virtual void Shutdown();

	virtual void operator()();

	std::shared_ptr<TEXTask> GetCurrentTask() const { return m_CurrentTask;}
};

}



