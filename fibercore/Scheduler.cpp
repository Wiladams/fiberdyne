
#include "Scheduler.h"

#include <iostream>

namespace fiberdyne {

Scheduler::Scheduler()
	: m_CurrentTask(nullptr)
//	, m_ContinueRunning(true)
{
}

void Scheduler::operator()()
{
	// pull a task off the ready list
	if (!m_ReadyList.try_pop(m_CurrentTask)) {
		//std::cout <<"Scheduler::Step(), FAILED POP" << std::endl;
		m_CurrentTask = nullptr;

		// must continue at top of loop, since we don't have a task to process
		return;
	}

	if (m_CurrentTask->getStatus() == TASK_READY)
	{
		m_CurrentTask->resume();

		// check completion status
		// if it's still in a ready state, 
		// put it back on the ready list
		if (m_CurrentTask->getStatus() == TASK_READY) {
			ScheduleTask(m_CurrentTask);
		} else {
			//printf("Dropping task: %d\n", m_CurrentTask->getStatus());
		}
	}	
}


void Scheduler::ScheduleTask(const std::shared_ptr<TEXTask> &atask)
{
	//printf("ScheduleTask::ScheduleTask()\n");
	
	// add task to ready list
	m_ReadyList.push(atask);
}

/*
void Scheduler::Shutdown()
{
	m_ContinueRunning = false;
	//waitForFinish();
}
*/
}
