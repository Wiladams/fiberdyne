#pragma once


#include <WTypes.h>


class AThread
{
private:
	HANDLE 					m_ThreadHandle;	// Our native handle
	unsigned int 			m_ThreadID;

protected:
public:
	AThread();
	AThread(unsigned int (__stdcall * startup)(void * start_address), void *param = nullptr);
	~AThread();

	// Some properties
	HANDLE		GetNativeHandle() {return m_ThreadHandle;}
	unsigned int GetThreadID() {return m_ThreadID;}

	// Essentially suspend until thread exits
	void join(size_t timeout = INFINITE);
};
