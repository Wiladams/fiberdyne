#pragma once


#include "AShard.h"

#include <stdint.h>

class DataChunk : public AShard
{
public:
	void init(const uint8_t * data, size_t length);

	DataChunk() :AShard(){};
	DataChunk(AShard &other);
	DataChunk(const size_t length);
	DataChunk(const char *strz);
	DataChunk(const uint8_t *data, size_t length);
	virtual ~DataChunk();

	// Public Utilities
	AShard Sub(const size_t offset, const size_t length) const;

	// override from AShard
	virtual AShard & clear() override;
};




