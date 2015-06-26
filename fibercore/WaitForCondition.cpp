
#include "WaitForCondition.h"

namespace fiberdyne {

WaitForCondition::WaitForCondition(Scheduler &sched)
	: m_ContinueRunning(true)
	, m_Scheduler(sched)
{
}

size_t WaitForCondition::tasksPending()
{
	return m_FibersAwaitingCondition.size();
}

bool WaitForCondition::tasksArePending()
{
	return tasksPending()>0;
}


void WaitForCondition::yieldForCondition( std::function<bool()> f, const std::shared_ptr<TEXTask> &atask)
{
	//printf("waitForCondition.yieldForCondition: \n");
	
	m_FibersAwaitingCondition.push(std::make_pair(atask, f));
	atask->suspend();
}

// each time we get CPU cycles, perform the following
void WaitForCondition::operator()()
{
	size_t nPredicates = m_FibersAwaitingCondition.size();

//print("waitForCondition.step ==> ", nPredicates)

	// BUGBUG - Not actually popping off of queue
	// so we're doubling the size each time through
	while (nPredicates) {
		// Get the current front entry
		auto entry = m_FibersAwaitingCondition.front();
		m_FibersAwaitingCondition.pop();

		if (entry.second()) {
			entry.first->setStatus(TASK_READY);
			m_Scheduler.ScheduleTask(entry.first);
			//printf("Conditional FIBER To Be RESCHEDULED\n");
		} else {
			// stick the fiber back in the queue if it does not
			// indicate true as yet.
			m_FibersAwaitingCondition.push(entry);
		}

		nPredicates--;
	}
}

/*
void WaitForCondition::operator()(AProcessor *proc)
{
	while (m_ContinueRunning) {
		this->operator()();
		proc->yield();
	}
}
*/
} // namespace fiberdyne