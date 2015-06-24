#pragma once

#include <stdint.h>

typedef enum BlockDeviceErrors {
	BD_NOERROR = 0,
	BD_EOF,
	BD_TIMEOUT,
	BD_INSUFFICIENT_BUFFER,
	BD_INSUFFICIENT_DATA,
	BD_INVALID_DESTINATION,
	BD_INVALID_OFFSET,
};

static const uint32_t BD_INFINITE = 0xFFFFFFFF;  // Infinite timeout
static const size_t BD_DEFAULT_TIMEOUT = (size_t)BD_INFINITE;
//static const size_t BD_DEFAULT_TIMEOUT = 100;

class ABlockDevice
{
public:
	virtual bool ReadBytes(uint8_t *buff, const size_t length, size_t *bytesRead = nullptr, size_t timeout = BD_DEFAULT_TIMEOUT, int * error = nullptr) = 0;
	virtual bool WriteBytes(const uint8_t *buff, const size_t length, size_t *bytesWritten = nullptr, size_t timeout = BD_DEFAULT_TIMEOUT, int * error = nullptr) =0;
};

