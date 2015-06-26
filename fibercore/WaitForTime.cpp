#include "WaitForTime.h"

namespace fiberdyne {

WaitForTime::WaitForTime(Scheduler &sched)
	: m_ContinueRunning(true)
	, m_Scheduler(sched)
{
}

bool WaitForTime::tasksArePending()
{
	return tasksPending() > 0;
}

size_t WaitForTime::tasksPending()
{
	return m_TasksWaitingForTime.size();
}


void WaitForTime::yieldUntilTime(double atime, const std::shared_ptr<TEXTask> &atask)
{
	//printf("WaitForTime::yieldUntilTime: %f\n", atime);

	m_TasksWaitingForTime.push(std::make_pair(atime, atask));	

	atask->suspend();
//print("yieldUntilTime - END")
}

void WaitForTime::yieldForTime(double millis, const std::shared_ptr<TEXTask> &atask)
{
	double currentTime = m_Clock.Milliseconds();
	double nextTime =  currentTime + millis;

	//printf("waitForTime.yieldForTime, CLOCK: %f\tNext Time: %f\n", currentTime, nextTime);
	yieldUntilTime(nextTime, atask);
}

void WaitForTime::operator()()
{
	//printf("WaitForTime::step\n");

	// Nothing to process, so return immediately
	if (m_TasksWaitingForTime.empty()) {
		return ;
	}

	double currentTime = m_Clock.Milliseconds();

	do {
		// get the first item out of the queue
		auto kv = m_TasksWaitingForTime.top();

		// compare the time to the current time
		// if it's greater than current time, then break out of loop
		//printf("WaitForTime.step(), top: %f\tCurrent: %f\n", kv.first, currentTime);
		if (kv.first > currentTime) {
			break;
		}

		// otherwise, schedule the associated task
		//printf("WaitforTime.step(), rescheduling task: \n");
		kv.second->setStatus(TASK_READY);
		m_Scheduler.ScheduleTask(kv.second);
		m_TasksWaitingForTime.pop();
	} while (!m_TasksWaitingForTime.empty()) ;
}

/*
void WaitForTime::operator()(AProcessor *proc)
{
	//printf("== WaitForTime - BEGIN\n");

	while (m_ContinueRunning) {
		this->operator()();
		proc->yield();
	}

	// Do whatever cleanup is necessary
}
*/
} // namespace fiberdyne
