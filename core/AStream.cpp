#define ANL_LIB

#include "AStream.h"

//#include <Windows.h>

#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "bitbang.h"


AStream::AStream()
{
	m_Position = 0;
	m_BigEndian = bitbang::IsBigEndian();
}

// pure virtual destructor
AStream::~AStream()
{
}

// Properties
AStream & AStream::setBigEndian(bool bigendian)
{
	m_BigEndian = bigendian;

	return *this;
}

bool AStream::getBigEndian()
{
	return m_BigEndian;
}

size_t AStream::getBytesAvailable()
{
	return 0;
}

bool AStream::Close()
{
	return false;
}

// Reading

bool AStream::Skip(const size_t bytesToSkip, size_t *howManySkipped, size_t timeout, int *error) 
{ 
	DataChunk chunk(bytesToSkip);
	int readerror;
	size_t howManyRead = 0;

	if (!ReadBytes(chunk.GetData(), chunk.GetLength(), &howManyRead, timeout, &readerror)) {
		if (howManySkipped) {
			*howManySkipped = howManyRead;
		}

		if (error) {
			*error = readerror;
		}

		return false;
	}

	return true;
}

// Read bytes from the stream
// Until we've received the number of bytes desired
// or EOF
bool AStream::ReadByteBlock(uint8_t *buff, const size_t length, size_t timeout, int *error)
{
	size_t bytesRead = 0;
	return ReadBytes(buff, length, &bytesRead, BD_INFINITE, error);
/*
	size_t nleft = length;
	int refillerror=0;

	size_t bytesRead;

	m_StopWatch.Reset();
	double futureTime = m_StopWatch.Milliseconds() + timeout;

	while (nleft) {
		bytesRead = 0;
		bool success = ReadBytes(buff+length-nleft, nleft, &bytesRead, timeout, &refillerror);

		nleft -= bytesRead;

		if (success) {
			break;
		}

		if ((refillerror == BD_TIMEOUT) || (refillerror == BD_INSUFFICIENT_DATA))
		{
			// continue waiting
			// WAA - with proper task waiting, we don't need this
			// should be a yield instead
			if (timeout == 0) {
				break;
			}

			if (BD_DEFAULT_TIMEOUT != timeout) {
#ifdef _DEBUG_VERBOSE
				printf("ASTREAM NON INFINITE WAIT: %lu\n", timeout);
#endif
				if ((futureTime - m_StopWatch.Milliseconds()) < 50.0) {
					break;
				}
			}
			Sleep(50);
		} else {
			break;
		}
	}

	size_t howmany = length - nleft;

	if (howmany == length) {
		return true;
	}

	if (error) {
		*error = refillerror;
	}

	return false;
*/
}

bool AStream::ReadLine(AShard &shard, size_t &bytesWritten, size_t timeout, int *error)
{
	size_t nchars = 0;
	int bytesconsumed = 0;

	// Set where the data starts
	uint8_t *buff = shard.GetData();
	size_t length = shard.GetLength();

	uint8_t abyte = 0;

	while(readByte(&abyte, timeout, error) && (nchars < length))
	{
		bytesconsumed = bytesconsumed + 1;

		if (abyte == '\n') {
			break;
		} else if (abyte != '\r') {
			buff[nchars] = abyte;
			nchars += 1;
		}
	}

	// End of buffer, nothing consumed
	if (bytesconsumed == 0) {
		return false;
	}

	bytesWritten = nchars;

	return true;
}

bool AStream::readByte(uint8_t *abyte, size_t timeout, int *error)
{	
	return ReadByteBlock(abyte, 1, timeout, error);
}

bool AStream::readUIntN(size_t n, uint64_t *value, size_t timeout, int *error)
{
	if (n > 8) {
		return false;
	}

	// read the bytes 
	uint8_t bytes[8];
	size_t bytesToRead = n;
	int readerror=0;

	if (!this->ReadByteBlock(bytes, n, timeout, &readerror)) {
#ifdef _DEBUG_VERBOSE
		printf("AStream::readUIntN(), ReadByteBlock() ERROR: 0x%x\n", readerror);
#endif

		if (error) {
			*error = readerror;
		}

		return false;
	}

	// decode value
	*value = bitbang::bb_readUIntN(n, bytes, 0, m_BigEndian);

	return true;
}


bool AStream::readUInt16(uint16_t *value, size_t timeout, int *error)
{
	uint64_t uintvalue;
	if (!this->readUIntN(2, &uintvalue, timeout, error)) {
		return false;
	}

	*value = (uint16_t)uintvalue;

	return true;
}

bool AStream::readUInt24(uint32_t *value, size_t timeout, int *error)
{
	uint64_t uintvalue;
	if (!this->readUIntN(3, &uintvalue, timeout, error)) {
		return false;
	}

	*value = (uint32_t)uintvalue;

	return true;
}

bool AStream::readUInt32(uint32_t *value, size_t timeout, int *error)
{
	uint64_t uintvalue;
	if (!this->readUIntN(4, &uintvalue, timeout, error)) {
		return false;
	}

	*value = (uint32_t)uintvalue;

	return true;
}

bool AStream::readUInt64(uint64_t *value, size_t timeout, int *error)
{
	return readUIntN(8, value, timeout, error);
}

bool AStream::ReadUIntPtr(uintptr_t *ptrvalue, size_t timeout, int *error)
{
	uint64_t value;
	if (!readUIntN(sizeof(uintptr_t), &value, timeout, error)) {
		return false;
	}

	*ptrvalue = (uintptr_t)value;

	return true;
}

// Writing
bool AStream::WriteByteBlock(const uint8_t *buff, const size_t length, int *error)
{
	return false;
}

bool AStream::writeByte(uint8_t value)
{
	uint8_t abyte[1] = {value};
	size_t bytesWritten = 1;

	return this->WriteBytes(abyte, bytesWritten);	
}

bool AStream::writeUIntN(size_t n, uint64_t value, int *error)
{
	if (n > 8) {
		return false;
	}

	// check there are enough bytes remaining in the stream

	// write out the bytes
	size_t bytesWritten = n;
	uint8_t bytes[8];
	bitbang::bb_writeUIntN(n, value, bytes, 0, m_BigEndian);
	return this->WriteBytes(bytes, bytesWritten);
}

bool AStream::WriteUIntPtr(uintptr_t ptrvalue, int *error)
{
	return writeUIntN(sizeof(ptrvalue), ptrvalue, error);
}


bool AStream::writeStringZ(const char *strz)
{
	size_t bytesWritten =  strlen(strz);
	return this->WriteBytes((uint8_t *)strz, bytesWritten);
}

bool AStream::writeShard(AShard &shard)
{
	size_t bytesWritten = shard.GetLength();
	return WriteBytes(shard.GetData(), bytesWritten);
}

bool AStream::WriteStream(AStream &source)
{
	bool success;
	uint8_t transferBuffer[4096];
	size_t transferBufferSize = 4096;
	size_t bytesRead;
	size_t bytesWritten;

	int transfererror;

	do  {
		transfererror = 0;
		success = source.ReadBytes(transferBuffer, transferBufferSize, &bytesRead, BD_INFINITE, &transfererror);

		//printf("== AStream::WriteStream(), ReadBytes: SUCCESS: %d\tERROR: %d\n", success, transfererror);

		if (bytesRead == 0) {
			break;
		}

		success = WriteBytes(transferBuffer, bytesRead, &bytesWritten, BD_INFINITE, &transfererror);

		//printf("== AStream::WriteStream(), WriteBytes: SUCCESS: %d\tERROR: %d\tTRANSFER: %d\n", success, transfererror, bytesWritten);

	}	while(success); // do

	return true;
}
