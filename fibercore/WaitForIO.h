#pragma once

#include "../core/IOCompletionPort.h"
#include "WaitForEvent.h"

#include <list>
#include <memory>
#include <unordered_map>

namespace fiberdyne {

typedef enum IOOps {
	GENERIC = 0,
	CONNECT = 1,
	ACCEPT = 2,
	READ = 3,
	WRITE = 4
};

struct IOOverlapped {
	OVERLAPPED OVL;

	// Data Buffer
	uint8_t * Buffer;
	int BufferLength;

	IOOps operation;
	int bytestransferred;
	int lastError;

	HANDLE deviceHandle;
} ;


class WaitForIO
{
private:
	WaitForEvent &m_Alerter;
	bool m_ContinueRunning;
	IOCompletionPort m_IOEventQueue;
	size_t m_MessageQuanta;
	uint64_t	m_OperationId;
	std::unordered_map<std::string, std::shared_ptr<TEXTask>>	m_EventFibers;

	uint64_t getNextOperationId();
	bool processIOEvent(uintptr_t key, int numbytes, OVERLAPPED *overlapped);
	
	WaitForIO(const WaitForIO &);

public:
	// Don't want to support copy constructor
	WaitForIO(WaitForEvent &, const std::size_t quanta =1);
	
	bool tasksArePending();
	std::size_t tasksPending();
	WaitForIO & setMessageQuanta(std::size_t quanta);

	bool WatchForIOEvents(HANDLE handle, uintptr_t key, int &error);
	void yieldForIO(HANDLE iohandle, const std::shared_ptr<IOOverlapped> &overlapped, const std::shared_ptr<TEXTask> &atask);

	virtual void operator()();
	//virtual void operator()(AProcessor *proc);


};

}
