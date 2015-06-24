#define ANL_LIB

#include "DataChunk.h"

#include <string.h>
#include <stdlib.h>
#include <assert.h>

void DataChunk::init(const uint8_t * data, size_t length)
{
	if (m_Data) {
		delete m_Data;
		m_Data = nullptr;
		m_Length = 0;
	}

	//m_Data = nullptr;
	m_Length = length;

	if (length > 0) {
		m_Data = new uint8_t [(size_t)length];
		if (data) {
			memcpy(m_Data, data, (size_t)length);
		}
	}
}

DataChunk::DataChunk(AShard &other)
{
	init(other.GetData(), other.GetLength());
}

DataChunk::DataChunk(const size_t length)
{
	init(nullptr, length);
}

DataChunk::DataChunk(const char *strz)
{
	size_t len = strz ? strlen(strz) : 0;
	init((uint8_t *)strz, len);
}


DataChunk::DataChunk(const uint8_t * data, size_t length)
{
	init(data, length);
}

DataChunk::~DataChunk()
{
	//printf("DataChunk::DELETE\n");
	
	if (m_Data)
    {
        delete m_Data;
        m_Data = nullptr;
    }
}

AShard DataChunk::Sub(const size_t offset, const size_t length) const
{

	// Check to see if we're in bounds
	if ((offset + length) > m_Length) {
		return AShard();
	}

	size_t maxLength = m_Length - offset;
	maxLength = maxLength < length ? maxLength : length;

	return AShard(m_Data, maxLength, offset);
}

AShard & DataChunk::clear()
{
	return *this;
}