#include "TEXTask.h"

#include "TEX.h"


namespace fiberdyne {

void __stdcall TEXTask::Main(void *context)
{
	auto self = reinterpret_cast<TEXTask *>(context);

	self->operator()(self);

	// The routine finished, so make sure we 
	// switch back to the parent or the thread
	// running the fiber will exit.
	self->setStatus(TASK_DEAD);

	self->yield();
}



TEXTask::TEXTask()
	: m_FiberHandle(nullptr)
	, m_Continuation(nullptr)
	, m_Status(TASK_STARTING)
{
	char abuff[64];
	sprintf_s(abuff, "taskfinished-0x%x", (void *)this);
 	m_TaskFinishedAlert = abuff;

	m_FiberHandle =  ::CreateFiber(0, (LPFIBER_START_ROUTINE)TEXTask::Main, this);
	m_Status = TASK_READY;
}

TEXTask::~TEXTask()
{
}

void TEXTask::onTaskFinished()
{
	TEX::getSingleton()->alertAll(getTaskFinishedAlert());
}

void TEXTask::setStatus(const int status)
{
	m_Status = status;

	switch (status) {
		case TASK_DEAD:
			onTaskFinished();
		break;
	}
}

void TEXTask::resume()
{
	m_Continuation = ::GetCurrentFiber();
	::SwitchToFiber(m_FiberHandle);
}

void TEXTask::yield()
{
	if (m_Continuation) {
		::SwitchToFiber(m_Continuation);
	}
}


void TEXTask::suspend() 
{
	m_Status = TASK_SUSPENDED;
	yield();
}

void TEXTask::waitForFinish()
{
	TEX::getSingleton()->waitForAlert(getTaskFinishedAlert());
}
}