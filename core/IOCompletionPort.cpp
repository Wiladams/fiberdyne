#define ANL_LIB

#include "IOCompletionPort.h"

#include <stdio.h>

IOCompletionPort::IOCompletionPort(HANDLE rawHandle)
	: m_Handle(rawHandle)
{
}

IOCompletionPort::IOCompletionPort()
{
	m_Handle = CreateIoCompletionPort(
		INVALID_HANDLE_VALUE,
		nullptr,
		0,
		0);

	if (m_Handle == nullptr) {
		int err = GetLastError();
		printf("IOCP:ctr, ERROR: %d\n", err);
		// throw a 'systemerror_exception'
	}
}

IOCompletionPort::~IOCompletionPort()
{
	// Assuming all associated file handle references are already 
	// closed, close this handle.
	if (m_Handle) {
		CloseHandle(m_Handle);
		m_Handle = nullptr;
	}
}

std::shared_ptr<IOCompletionPort> IOCompletionPort::try_create(uint32_t concurrentThreads, int &error)
{
	HANDLE rawhandle = CreateIoCompletionPort(
		INVALID_HANDLE_VALUE,
		nullptr,
		0,
		(DWORD)concurrentThreads);

	if (NULL == rawhandle) {
		error = GetLastError();
		return nullptr;
	}

	return std::make_shared<IOCompletionPort>(rawhandle);
}

bool IOCompletionPort::addIoHandle(HANDLE otherhandle, uintptr_t Key, int &error)
{
	error = 0;
	HANDLE rawhandle = CreateIoCompletionPort(
		otherhandle,
		m_Handle,
		Key,
		0);

	if (NULL == rawhandle) {
		error = GetLastError();
		return false;
	}

	return true;
}

int IOCompletionPort::Enqueue(LPOVERLAPPED lpOverlapped, int numberOfBytesTransferred, ULONG_PTR dwCompletionKey)
{
//	if (dwCompletionKey == nullptr) {
//		print("IOCompletionPort.enqueue(), NO KEY SPECIFIED")
//		return false, "no data specified"
//	}

	BOOL status = PostQueuedCompletionStatus(m_Handle,
		(DWORD)numberOfBytesTransferred, 
		dwCompletionKey, 
		lpOverlapped);
	
	if (!status) {
		return GetLastError();
	}

	return 0;
}

int IOCompletionPort::Dequeue(OVERLAPPED **overlapped, int &transferred, ULONG_PTR &completionKey, size_t dwMilliseconds)
{

	DWORD lpNumberOfBytesTransferred;
	ULONG_PTR lpCompletionKey;
	LPOVERLAPPED lpOverlapped;
	
	BOOL success = GetQueuedCompletionStatus(m_Handle,
    	&lpNumberOfBytesTransferred,
    	&lpCompletionKey,
    	&lpOverlapped,
    	(DWORD)dwMilliseconds);

	if (overlapped) {
		*overlapped = lpOverlapped;
	}

	transferred = lpNumberOfBytesTransferred;
	completionKey = lpCompletionKey;

	if (!success) {
		DWORD err = GetLastError();
		
		// If the dequeue failed, there can be two cases
		// In the first case, the lpOverlapped is nil,
		// in this case, nothing was dequeued, 
		// so just return whatever the reported error was.
		//if (lpOverlapped == nullptr) {
		//	return err;
		//}

		// if lpOverlapped ~= nil, then 
		// data was transferred, but there is an error
		// indicated in the underlying connection
		return err;
	}

	// For the remaining cases

	return 0;
}

