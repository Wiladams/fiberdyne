#pragma once

#include "AStream.h"
#include "DataChunk.h"
#include "MemoryStream.h"

#include <stdio.h>	// SEEK_*

class  ABufferedStream : public AStream
{
private:
	
	ABufferedStream(const ABufferedStream &);	// No copy constructor

protected:	
	std::shared_ptr<MemoryStream> m_ReadingBuffer;
	std::shared_ptr<DataChunk> m_Chunk;

	// constructor
	ABufferedStream();


	virtual uint64_t RefillReadingBuffer(size_t timeout, int &error) = 0;

public:

	// Virtual Destructor
	virtual ~ABufferedStream();


	virtual bool ReadBytes(uint8_t *buff, const size_t length, size_t *bytesRead = nullptr, size_t timeout = BD_DEFAULT_TIMEOUT, int * error = nullptr);
	virtual bool WriteBytes(const uint8_t *buff, const size_t length, size_t *bytesWritten = nullptr, size_t timeout = BD_DEFAULT_TIMEOUT, int * error = nullptr);

};


