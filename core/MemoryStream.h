#pragma once

#include "AStream.h"
#include "AShard.h"

#include <memory>


class MemoryStream : public AStream 
{
public:
	// Constructors
	MemoryStream(AShard &shard);
	MemoryStream(const std::shared_ptr<AShard> &shard);
	MemoryStream(const std::shared_ptr<AShard> &shard, const uint64_t alreadyWritten);

	// Virtual Destructor
	virtual ~MemoryStream();

	// ABlockDevice overrides
	virtual bool ReadBytes(uint8_t *buff, const size_t length, size_t *bytesRead = nullptr, size_t timeout = BD_DEFAULT_TIMEOUT, int * error = nullptr)override;
	virtual bool WriteBytes(const uint8_t *buff, const size_t length, size_t *bytesWritten = nullptr, size_t timeout = BD_DEFAULT_TIMEOUT, int * error = nullptr)override;

	// AStream Overrides
	virtual bool Skip(const size_t bytesToSkip, size_t *bytesSkipped = nullptr, size_t timeout = BD_DEFAULT_TIMEOUT, int *error=nullptr) override;

	// Specialized
	virtual size_t getBytesAvailable();
	virtual bool Seek(const int offset, const int origin, int *error = nullptr);

	AShard GetBaseShard() {return m_Shard;}

protected:
	// Instance fields	
	AShard m_Shard;
	size_t m_BytesWritten;	//  bytes written sentinel
};

