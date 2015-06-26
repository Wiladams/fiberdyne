
#include "WaitForEvent.h"

namespace fiberdyne {

WaitForEvent::WaitForEvent(Scheduler &sched)
	: m_Scheduler(sched)
{
}

bool WaitForEvent::tasksArePending()
{
	return tasksPending() > 0;
}

size_t WaitForEvent::tasksPending()
{
	return m_SuspendedTasks.size();
}


/*
	This is called from within the context of some task
	so, 'currentTask' represents the currently running task,
	which should be different from 'this'.
*/
void WaitForEvent::yieldForEvent(std::string alertName, const std::shared_ptr<TEXTask> &atask)
{

	// add the fiber to the list of suspended tasks
	if (!m_SuspendedTasks.count(alertName)) {
		auto newlist = std::make_shared<std::list<std::shared_ptr<TEXTask>>>();
		m_SuspendedTasks.insert(std::make_pair(alertName, newlist));
	}

	m_SuspendedTasks[alertName]->push_back(atask);
	
	// Suspend the calling task
	atask->suspend();
}

void WaitForEvent::alertOne(std::string alertName)
{
	if (0 == m_SuspendedTasks.count(alertName)) {
		// No events waiting for the specific alert
		return ;
	}

	auto nTasks = m_SuspendedTasks[alertName]->size();

	if (nTasks < 1) {
		// The alert is registered, but there are no tasks
		// waiting for it.
		return ;
	}

	auto atask = m_SuspendedTasks[alertName]->front();
	m_SuspendedTasks[alertName]->pop_front();

	m_Scheduler.ScheduleTask(atask);

	return ;
}

void WaitForEvent::alertAll(std::string alertName)
{
	if (0 == m_SuspendedTasks.count(alertName)) {
		// No events waiting for the specific alert
		return ;
	}

	auto nTasks = m_SuspendedTasks[alertName]->size();

	if (nTasks < 1) {
		// The alert is registered, but there are no tasks
		// waiting for it.
		return ;
	}

	do {
		auto atask = m_SuspendedTasks[alertName]->front();
		if (nullptr != atask) {
			atask->setStatus(TASK_READY);
			m_Scheduler.ScheduleTask(atask);
		}
		m_SuspendedTasks[alertName]->pop_front();
	} while (!m_SuspendedTasks[alertName]->empty());

	return ;
}


}

