#pragma once

#include "AShard.h"
#include "ABlockDevice.h"
#include "DataChunk.h"
#include "StopWatch.h"

#include <stdint.h>
#include <stdio.h>	// SEEK_*


class AStream : public ABlockDevice
{
private:
	AStream(AStream &);

protected:
	bool m_BigEndian;
	uint64_t		m_Position;		// Where is the current 'cursor' positioned
	StopWatch	m_StopWatch;

	AStream();

public:
	// Virtual Destructor
	virtual ~AStream();

	// Configuring parameters
	virtual AStream & setBigEndian(bool isBigEndian);
	virtual bool getBigEndian();
	
	virtual bool Close();

	// Get various properties
	virtual uint64_t getPosition() const {return m_Position;};
	virtual size_t getBytesAvailable();

	// perform some actions
	virtual bool Skip(const size_t bytesToSkip, size_t *bytesSkipped = nullptr, size_t timeout = BD_DEFAULT_TIMEOUT, int *error=nullptr);


	// ABlockDevice overrides
	virtual bool ReadBytes(uint8_t *buff, const size_t length, size_t *bytesRead = nullptr, size_t timeout = BD_DEFAULT_TIMEOUT, int * error = nullptr) = 0;
	virtual bool WriteBytes(const uint8_t *buff, const size_t length, size_t *bytesWritten = nullptr, size_t timeout = BD_DEFAULT_TIMEOUT, int * error = nullptr) =0;

	// Reading
	virtual bool ReadByteBlock(uint8_t *buff, const size_t length, size_t timeout = BD_DEFAULT_TIMEOUT, int *error=nullptr);
	virtual bool readByte(uint8_t *abyte, size_t timeout = BD_DEFAULT_TIMEOUT, int *error = nullptr);
	virtual bool ReadLine(AShard &shard, size_t &bytesWritten, size_t timeout, int *error = nullptr);

	// Reading integers
	virtual bool readUIntN(size_t n, uint64_t *value, size_t timeout = BD_DEFAULT_TIMEOUT, int *error = nullptr);
	virtual bool readUInt16(uint16_t *, size_t timeout = BD_DEFAULT_TIMEOUT, int *error = nullptr);
	virtual bool readUInt24(uint32_t *, size_t timeout = BD_DEFAULT_TIMEOUT, int *error = nullptr);
	virtual bool readUInt32(uint32_t *, size_t timeout = BD_DEFAULT_TIMEOUT, int *error = nullptr);
	virtual bool readUInt64(uint64_t *, size_t timeout = BD_DEFAULT_TIMEOUT, int *error = nullptr);
	virtual bool ReadUIntPtr(uintptr_t *, size_t timeout = BD_DEFAULT_TIMEOUT, int *error = nullptr);

	// Writing
	virtual bool WriteByteBlock(const uint8_t *buff, const size_t length, int *error=nullptr);
	virtual bool writeByte(const uint8_t abyte);
	
	virtual bool writeUIntN(size_t n, uint64_t value, int *error = nullptr);
	virtual bool WriteUIntPtr(uintptr_t ptrvalue, int *error = nullptr);	
	
	virtual bool writeStringZ(const char *strz);
	virtual bool writeShard(AShard &shard);
	virtual bool WriteStream(AStream &source);
};


