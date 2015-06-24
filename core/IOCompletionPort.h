#pragma once 


#include <wtypes.h>
#include <memory>


class IOCompletionPort
{
	HANDLE m_Handle;
	
public: 
	IOCompletionPort();
	explicit IOCompletionPort(HANDLE rawHandle);

	~IOCompletionPort();

	bool addIoHandle(HANDLE otherhandle, uintptr_t Key, int &error);

	int Dequeue(OVERLAPPED **overlapped, int &transferred, ULONG_PTR &key, size_t milliseconds = 0);
	int Enqueue(LPOVERLAPPED lpOverlapped, int transferred, ULONG_PTR key = 0);

	static std::shared_ptr<IOCompletionPort> try_create(uint32_t concurrentThreads, int &error);
};
