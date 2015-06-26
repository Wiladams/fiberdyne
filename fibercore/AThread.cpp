#define ANL_LIB

#include "AThread.h"

#include <Windows.h>
#include <errno.h>
#include <process.h>
#include <stdio.h>
#include <assert.h>

AThread::AThread()
	: m_ThreadHandle(0)
	, m_ThreadID(0)
{
	m_ThreadID = GetCurrentThreadId();
	m_ThreadHandle = GetCurrentThread();
}

AThread::AThread(unsigned int (__stdcall * start_address)(void *param), void *value)
	: m_ThreadHandle(0)
	, m_ThreadID(0)
{
	unsigned initflag = 0;
	//unsigned initflag = CREATE_SUSPENDED;

	void *security = nullptr;
	unsigned int stack_size = 0;

	uintptr_t result = _beginthreadex( // NATIVE CODE
   		security,
   		stack_size,
   		start_address,
   		value,
   		initflag,
   		&m_ThreadID);

	//printf("Computicle::Computicle(), beginthreadex RESULT: 0x%x\n", result);

	if (!result) {
		// If result == 0, then there 
		// was an error
		// check errno, throw an exception
		fprintf(stderr, "Computicle::Computicle(), beginthreadex, result == 0, error: %d\n", errno);

		assert(false);
	}

	if (result == (uintptr_t)-1L) {
		//errno == EAGAIN // too many threads
		//errno == EINVAL // invalid argument
		//errno == EACCES // insufficient resources
		fprintf(stderr, "Computicle::Computicle(), beginthreadex RESULT: -1, Error: %d\n", errno);
		assert(false);
	}

	m_ThreadHandle = reinterpret_cast<HANDLE>(result);

}

AThread::~AThread()
{
	//assert(m_ThreadHandle == nullptr);
}

void AThread::join(size_t timeout)
{
	DWORD dwWaitResult;
	dwWaitResult = WaitForSingleObject( 
        m_ThreadHandle, // thread handle
        (DWORD)timeout);    // indefinite wait

	CloseHandle(m_ThreadHandle);
	m_ThreadHandle = nullptr;
}