
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "MemoryStream.h"

MemoryStream::MemoryStream(AShard &shard)
	:AStream()
	, m_Shard(shard)
	, m_BytesWritten(shard.GetLength())
{
}

MemoryStream::MemoryStream(const std::shared_ptr<AShard> &shard)
	:AStream()
	, m_Shard(*shard)
	, m_BytesWritten(0)
{
	if (nullptr != shard) {
		m_BytesWritten = shard->GetLength();
	}
}


MemoryStream::MemoryStream(const std::shared_ptr<AShard> &shard, const uint64_t alreadyWritten)
  : AStream(),
  m_Shard(*shard),
  m_BytesWritten(0)
{
	if (shard) {
		m_BytesWritten = (size_t)alreadyWritten;
	}
}


// Virtual Destructor
MemoryStream::~MemoryStream()
{
}

bool MemoryStream::Seek(const int offset, const int origin, int *error)
{
	uint64_t newpos;

	switch (origin) {
		case SEEK_SET:
			newpos = offset;
			break;
		case SEEK_CUR:
			newpos = m_Position + offset;
			break;

		case SEEK_END:
			newpos = m_BytesWritten - offset - 1;
			break;
	}

	if ((newpos < 0) || (newpos > m_BytesWritten)) {
		return false;
	}

	m_Position = newpos;

	return true;
}


bool MemoryStream::Skip(const size_t bytesToSkip, size_t *bytesSkipped, size_t timeout, int *error)
{
    uint64_t newpos = m_Position + bytesToSkip;

    if (newpos >= m_BytesWritten) {
		if (bytesSkipped) {
			*bytesSkipped = (size_t)(m_BytesWritten - m_Position);
		}
		m_Position = m_BytesWritten;

		return false;
	}

    m_Position = newpos;

	return true;
}


size_t MemoryStream::getBytesAvailable()
{
  //printf("getBytesAvailable: %d, %d\n", self->DataLength, self->Position);
    return (size_t)(m_BytesWritten - m_Position);
}

bool MemoryStream::ReadBytes(uint8_t *buff, const size_t length, size_t *bytesRead, size_t timeout, int * error)
{
  size_t remaining = getBytesAvailable();
  size_t maxbytes = remaining < length ? remaining : length;

  if (maxbytes == 0) {
	  if (error) {
		  *error = BD_EOF;
	  }
	  return false;  // 'eof'
  }
  

//printf("MemoryStream_readBytes, maxbytes: %d\n", maxbytes);

  memcpy(buff, m_Shard.GetData()+m_Position,  (size_t)maxbytes);
  if (bytesRead) {
	  *bytesRead = maxbytes;
  }

  m_Position = m_Position + maxbytes;

  return true;
}


bool MemoryStream::WriteBytes(const uint8_t *buff, const size_t length, size_t *bytesWritten, size_t timeout, int * error)
{
    size_t remaining = (size_t)(m_Shard.GetLength() - m_Position);
    size_t maxbytes = remaining < length ? remaining : length;

    if (maxbytes <= 0)
    {
        return false;
    }

    memcpy((m_Shard.GetData()+m_Position), buff, (size_t)maxbytes);
	if (bytesWritten) {
		*bytesWritten = maxbytes;
	}
    
    m_Position = m_Position + maxbytes;

    if (m_Position > m_BytesWritten)
    {
      m_BytesWritten = (size_t)m_Position;
    }

    return true;
}
