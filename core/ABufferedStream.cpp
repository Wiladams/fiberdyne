
#include "ABufferedStream.h"


ABufferedStream::ABufferedStream() 
	: AStream()
	, m_ReadingBuffer(nullptr)
	, m_Chunk(nullptr)
{
}

// pure virtual destructor
ABufferedStream::~ABufferedStream()
{
}



// Reading
bool ABufferedStream::ReadBytes(uint8_t *buff, const size_t length, size_t *bytesRead, size_t timeout, int * error)
{
	uint64_t nleft = length;
	uint64_t nread = 0;
	int refillerror=0;
	size_t refillTimeout = BD_DEFAULT_TIMEOUT;

	while (nleft > 0) {
		uint64_t bytesReady = RefillReadingBuffer(refillTimeout, refillerror);

		//printf("QueueStream::ReadBytes(), BytesReady: %d\tError: %d\n", bytesReady, refillerror);

		if (!bytesReady) {
			//printf("No Bytes Ready.\n");
			break;
		}

		// try to fill bytes from existing buffer
		uint64_t maxbytes = nleft < bytesReady ? nleft : bytesReady;

		//printf("QueueStream::ReadBytes(), maxbytes: %d\n", maxbytes);
		AShard shard(buff, maxbytes, length-nleft);
		nread = maxbytes;
		if (!m_ReadingBuffer->ReadBytes(shard.GetData(), nread)) {
			//printf(" QueueStream::ReadBytes(), m_ReadingBuffer->ReadBytes() FAILED\n");
			break;
		}

		//printf(" QueueStream::ReadBytes(), loop: %d\n", nread);

		nleft = nleft - nread;
	}

	uint64_t howmany = length - nleft;

//printf("HOW MANY: %d\t%d\n", howmany, length);

	if (bytesRead) {
		*bytesRead = howmany;
	}
	
	// Increment position so the consumer can know how many
	// bytes have been consumed.
	m_Position += howmany;

	if (howmany == length) {
		//printf("HOWMANY == LENGTH\n");
		return true;
	}

	if (error) {
		*error = (int)refillerror;
	}

	return false;
}

// Writing
bool ABufferedStream::WriteBytes(const uint8_t *buff, const size_t length, size_t *bytesWritten, size_t timeout, int * error)
{
	return false;
}
