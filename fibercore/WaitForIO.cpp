#include "WaitForIO.h"
#include "TEX.h"


namespace fiberdyne {

std::string pointerToString(void *ptr)
{
	char buff[64];
	sprintf_s(buff, "ioevent-0x%x", ptr);
	return std::string(buff);
}

WaitForIO::WaitForIO(WaitForEvent &alerter, const std::size_t quanta)
	: m_Alerter(alerter)
	, m_MessageQuanta(quanta)
	, m_ContinueRunning(true)
{
	// printf("waitForIO()\n");
}

WaitForIO & WaitForIO::setMessageQuanta(std::size_t quanta)
{
	m_MessageQuanta = quanta;
	return *this;
}

size_t WaitForIO::tasksPending()
{
	return m_EventFibers.size();
}

bool WaitForIO::tasksArePending()
{
	return tasksPending() > 0;
}

uint64_t WaitForIO::getNextOperationId()
{
	m_OperationId = m_OperationId + 1;
	return m_OperationId;
}

bool WaitForIO::WatchForIOEvents(HANDLE handle, uintptr_t key, int &error)
{
	//print("waitForIO.observeIOEvent, adding: ", handle, param)
	return m_IOEventQueue.addIoHandle(handle, key, error);
}

void WaitForIO::yieldForIO(HANDLE iohandle, const std::shared_ptr<IOOverlapped> &overlapped, const std::shared_ptr<TEXTask> &atask)
{
	//printf("== waitForIO.yield: BEGIN: \n");

	// Track the task based on the overlapped structure
	std::string alertName = pointerToString(overlapped.get());
	m_Alerter.yieldForEvent(alertName, atask);
}


bool WaitForIO::processIOEvent(uintptr_t key, int numbytes, OVERLAPPED *overlapped)
{
	//print("waitForIO.processIOEvent: ", key, numbytes, arch.pointerToString(overlapped))
	auto alertName = pointerToString(overlapped);

	IOOverlapped *ovl = (IOOverlapped *)overlapped;

	ovl->bytestransferred = numbytes;

	// Signal the tasks that are awaiting the event
	TEX::getSingleton()->alertAll(alertName);

	// remove the fiber from the index based on the
	// overlapped structure
	m_EventFibers.erase(alertName);

	return true;
}

void WaitForIO::operator()()
{
	// Check to see if there are any IO Events to deal with
	OVERLAPPED *overlapped;
	int transferred=0;
	ULONG_PTR key;

	int error = m_IOEventQueue.Dequeue(&overlapped, transferred, key, m_MessageQuanta);

	//printf("waitForIO.step, error: %d", error);

	// First check to see if we've got a timeout
	// if so, then just return immediately
	if (error) {
		if (error == WAIT_TIMEOUT) {
			return ;
		}

		// We want to process other 
		// errors, so continue along
	}

	bool success = processIOEvent(key, transferred, overlapped);

}

/*
void WaitForIO::operator()(AProcessor *proc)
{
	while (m_ContinueRunning) {
		this->operator()();
		proc->yield();
	}
}
*/
} // namespace fiberdyne