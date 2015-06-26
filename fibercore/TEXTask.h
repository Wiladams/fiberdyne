#pragma once


#include "WTypes.h"

#include <concurrent_unordered_map.h>
#include <functional>
#include <memory>
#include <string>


namespace fiberdyne {

typedef enum TaskStatus {
	TASK_STARTING = 0,	// Task is setting up
	TASK_READY,			// The task is ready to be run
	TASK_SUSPENDED,		// waiting for some sort of event
	TASK_DEAD = 255,	// task is complete
};

class TEXTask
{
private:
	int 	m_Status;
	HANDLE m_FiberHandle;
	void * m_Continuation;	// where to call next in yield
	std::string m_TaskFinishedAlert;

protected:
	static void __stdcall Main(void *context);

	virtual void onTaskFinished();

public:
	TEXTask();
	virtual ~TEXTask();	

	// Alerts
	// Properties
	int getStatus() const {return m_Status;}
	void setStatus(const int status);


	// Sub-Class MUST implement
	virtual void operator()(TEXTask *){};

	// Individual things to do to a task
	virtual void resume();
	void suspend();
	virtual void yield();


	// Alerts
	std::string getTaskFinishedAlert() {return m_TaskFinishedAlert;};
	void waitForFinish();

};


class RoutineTask : public TEXTask
{
	std::function<void(TEXTask *)> m_Routine;

public:
	RoutineTask(std::function<void(TEXTask *)> routine)
		: m_Routine(routine)
	{}

	virtual void operator()(TEXTask *) override
	{
		m_Routine(this);
	};

};

}
