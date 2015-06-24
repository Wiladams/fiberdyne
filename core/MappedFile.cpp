
#include "MappedFile.h"
#include "Handle.h"


/*
	A Mapped File is similar to a Shard or DataChunk in that it
	deals with memory pointers.

	The fact that it is a mapped file is what makes it convenient
	for reading and writing.

	Inheriting from AShard gives it the automatic ReadBytes/WriteBytes
	needed for it to play well with the AStream object.
*/
MappedFile::MappedFile(HANDLE fileHandle, HANDLE mappingHandle, uint8_t *data, uint64_t length)
	: AShard(data, (size_t)length, 0)
	, m_FileHandle(fileHandle)
	, m_MappingHandle(mappingHandle)
{
}

/*
MappedFile::MappedFile(const char *filename)
{
	// First, get the file handle
	DWORD dwDesiredAccess = GENERIC_READ;
	DWORD dwShareMode = FILE_SHARE_READ;
	LPSECURITY_ATTRIBUTES lpSecurityAttributes = nullptr;
	DWORD dwCreationDisposition = OPEN_EXISTING;
	DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL;
	HANDLE hTemplateFile = nullptr;

	m_FileHandle = CreateFileA(
		filename,
		dwDesiredAccess,
		dwShareMode,
		lpSecurityAttributes,
		dwCreationDisposition,
		dwFlagsAndAttributes,
		hTemplateFile);
//printf("File HANDLE: 0x%x\n", m_FileHandle);
	
	if (m_FileHandle == INVALID_HANDLE_VALUE) return;



	// Next, create the file mapping
	DWORD flProtect = PAGE_READONLY;
	DWORD dwMaximumSizeHigh = 0;
	DWORD dwMaximumSizeLow = 0;
	LPCTSTR lpName = nullptr;

	m_MappingHandle = CreateFileMapping(
		m_FileHandle,
		nullptr,
		flProtect,
		dwMaximumSizeHigh,
		dwMaximumSizeLow,
		lpName);
//printf("Mapping HANDLE: 0x%x\n", m_MappingHandle);
	if (m_MappingHandle == NULL) return ;

	// And finally, map the object into our address space
	DWORD dwFileOffsetHigh = 0;
	DWORD dwFileOffsetLow = 0;
	SIZE_T dwNumberOfBytesToMap = 0;
	dwDesiredAccess = FILE_MAP_READ;

	m_Data = (uint8_t *)MapViewOfFile(
		m_MappingHandle,
		dwDesiredAccess,
		dwFileOffsetHigh,
		dwFileOffsetLow,
		dwNumberOfBytesToMap);
//printf("DATA: 0x%x\n", m_Data);
	if (m_Data == NULL) return;

	// Get the size of the file
	DWORD lpFileSizeHigh[1];
	m_Length = GetFileSize(m_FileHandle,lpFileSizeHigh);
//printf("File SIZE: %d\n", m_Length);

}
*/

MappedFile::~MappedFile()
{
	//puts("FileMappedShard: DELETE");

	// Unmap the view
	BOOL unmapstatus = UnmapViewOfFile(GetData());

	// Close the file mapping
	CloseHandle(m_MappingHandle);

	// close the file handle
	CloseHandle(m_FileHandle);
}

std::shared_ptr<MappedFile> MappedFile::try_create(const std::string &filename)
{
	// First, get the file handle
	DWORD dwDesiredAccess = GENERIC_READ;
	DWORD dwShareMode = FILE_SHARE_READ;
	LPSECURITY_ATTRIBUTES lpSecurityAttributes = nullptr;
	DWORD dwCreationDisposition = OPEN_EXISTING;
	DWORD dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL;
	HANDLE hTemplateFile = nullptr;
	HANDLE l_FileHandle = nullptr;
	
	l_FileHandle = CreateFileA(
		filename.c_str(),
		dwDesiredAccess,
		dwShareMode,
		lpSecurityAttributes,
		dwCreationDisposition,
		dwFlagsAndAttributes,
		hTemplateFile);
//printf("File HANDLE: 0x%x\n", m_FileHandle);
	
	if (l_FileHandle == INVALID_HANDLE_VALUE) return nullptr;

	// Next, create the file mapping
	DWORD flProtect = PAGE_READONLY;
	DWORD dwMaximumSizeHigh = 0;
	DWORD dwMaximumSizeLow = 0;
	LPCTSTR lpName = nullptr;
	HANDLE l_MappingHandle = nullptr;

	l_MappingHandle = CreateFileMapping(
		l_FileHandle,
		nullptr,
		flProtect,
		dwMaximumSizeHigh,
		dwMaximumSizeLow,
		lpName);
//printf("Mapping HANDLE: 0x%x\n", m_MappingHandle);
	if (l_MappingHandle == NULL) {
		CloseHandle(l_FileHandle);
		return nullptr;
	}


	// And finally, map the object into our address space
	DWORD dwFileOffsetHigh = 0;
	DWORD dwFileOffsetLow = 0;
	SIZE_T dwNumberOfBytesToMap = 0;
	dwDesiredAccess = FILE_MAP_READ;

	uint8_t *l_Data = (uint8_t *)MapViewOfFile(
		l_MappingHandle,
		dwDesiredAccess,
		dwFileOffsetHigh,
		dwFileOffsetLow,
		dwNumberOfBytesToMap);

	
	if (l_Data == NULL) {
		CloseHandle(l_FileHandle);
		CloseHandle(l_MappingHandle);
		
		return nullptr;
	}

	// Get the size of the file
	DWORD lpFileSizeHigh[1];
	uint64_t l_Length = GetFileSize(l_FileHandle,lpFileSizeHigh);

	return std::make_shared<MappedFile>(l_FileHandle, l_MappingHandle, l_Data, l_Length);
}
