#pragma once

#include "TEXTask.h"
#include "Scheduler.h"

#include <stdint.h>
#include <list>
#include <string>
#include <unordered_map>
#include <memory>

namespace fiberdyne {

class WaitForEvent
{
private:
	Scheduler &m_Scheduler;
	std::unordered_map<std::string, std::shared_ptr<std::list<std::shared_ptr<TEXTask> > > > m_SuspendedTasks;

protected:
public:
	WaitForEvent(Scheduler &);

	bool tasksArePending();
	size_t tasksPending();

	// public interface
	void yieldForEvent(std::string eventName, const std::shared_ptr<TEXTask> &atask);
	void alertOne(std::string alertName);
	void alertAll(std::string alertName);
};

}
