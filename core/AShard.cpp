
#include "AShard.h"

#include <ctype.h>		// isspace

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


AShard::AShard()
{
	m_Data = nullptr;
	m_Length = 0;
	m_Offset = 0;
}

AShard::AShard(const char *data)
{
	m_Data = (uint8_t *)data;
	m_Length = strlen(data);
	m_Offset = 0;
}

AShard::AShard(uint8_t *data, size_t length, size_t offset)
{
	m_Data = data;
	m_Length = length;
	m_Offset = offset;
}

/*
AShard & AShard::operator= (const AShard & rhs) 
{
	// Check for self assignment
	if (this == &rhs) {
		return *this;
	}

	m_Data = rhs.m_Data;
	m_Offset = rhs.m_Offset;
	m_Length = rhs.m_Length;

	return *this;
}
*/

// ABlockDevice
bool AShard::ReadBytes(uint8_t *buff, const size_t length, size_t *bytesRead, size_t timeout, int * error)
{
	uint8_t *ptr = GetData();
	if (!ptr) {
		if (bytesRead) {
			*bytesRead = 0;
		}
		if (error) {
			*error = BD_INVALID_DESTINATION;
		}
		return false;
	}

	size_t myLength = GetLength();
	size_t maxbytes = length < myLength ? length : myLength;

	memcpy(buff, ptr, maxbytes);

	if (bytesRead) {
		*bytesRead = maxbytes;
	}
	
	// report if we did not copy the amount requested
	if (maxbytes != length) {
		if (error) {
			*error = BD_INSUFFICIENT_BUFFER;
		}
		return false;
	}

	return true;
}

bool AShard::WriteBytes(const uint8_t *buff, const size_t length, size_t *bytesWritten, size_t timeout, int * error)
{
	uint8_t *ptr = GetData();
	if (!ptr) {
		if (nullptr != bytesWritten) {
			*bytesWritten = 0;
		}
		if (nullptr != error) {
			*error = -1;
		}

		return false;
	}

	size_t myLength = GetLength();
	size_t maxbytes = length < myLength ? length : myLength;

	memcpy(ptr, buff, maxbytes);

	if (bytesWritten) {
		*bytesWritten = maxbytes;
	}

	if ((maxbytes != length) && (error)) {
		*error = BD_INSUFFICIENT_BUFFER;
		return false;
	}

	return true;
}


AShard & AShard::clear()
{
	m_Data = nullptr;
	m_Length = 0;
	m_Offset = 0;

	return *this;
}

// Given this shard, and another buffer of bytes
// return how many bytes they have in common
// this is for prefix matching
size_t AShard::samebytes(const uint8_t *buff, const size_t length)
{
	uint8_t *p1 = GetData();
	const uint8_t *p2 = buff;
	size_t maxbytes = length < GetLength() ? length : GetLength();

    size_t bytes = 0;

	while ((p1[bytes] == p2[bytes]) && (bytes < maxbytes)) {
		bytes += 1;
	}

    return bytes;
}

int AShard::compare(const uint8_t *buff, const size_t length)
{
	size_t bytesMatched = samebytes(buff, length);
	if ((bytesMatched ==  length) && (length == GetLength())) {
		// exact match
		return 0;
	}

	if (bytesMatched == GetLength()) {
		// shard is shorter than buffer
		return -1;
	}

	// Buffer is longer than shard, and therefore "greater"

	return 1;
}

/*
	function: first

	Description: Split a shard into the 'first' part and the 'rest'
	based on a given single character delimeter.
*/
AShard & AShard::first(AShard &front, AShard &rest, uint8_t delim) const
{
	size_t idx;

	if (!this->indexOfChar(delim, idx)) {
		// In this case, the 'front' is the original
		// shard, and the 'rest' is nothing
		front = *this;
		rest.clear();
		
		return front;
	}

	size_t charCount = idx - m_Offset;
	front.m_Data = m_Data;
	front.m_Offset = m_Offset;
	front.m_Length = charCount;

	rest.m_Data = m_Data;
	rest.m_Offset = m_Offset + charCount + 1;
	rest.m_Length = m_Length - charCount - 1;

	return front;
}

bool AShard::indexOfChar(const uint8_t achar, size_t &idx) const
{
	if (isEmpty()) return false;

	idx = m_Offset;
	while ((idx - m_Offset) < m_Length) {
		if (achar == m_Data[idx]) {
			return true;
		}
		idx = idx + 1;
	}

	return false;
}

/*
	Function: indexOfShard

	Description: A simple brute force search for a target
*/
bool AShard::indexOfShard(const AShard &target, size_t &idx)
{
	idx = m_Offset;

	if (target.isEmpty()) {
		// An empty target would match any shard's beginning
		return true;
	}

	if (m_Length < target.m_Length) {
		return false;
	}

	while (idx < (m_Offset + m_Length)) {
		size_t foroff = 0;
		while (foroff < target.m_Length) {
			//printf("%d %c %c\n", idx, target.m_Data[target.m_Offset+foroff], m_Data[idx+foroff]);
			if (target.m_Data[target.m_Offset+foroff] != m_Data[idx+foroff]) {
				break;
			}
			foroff = foroff + 1;
		}

		// if we've reached the length of the target
		// then we've found our match
		if (foroff == target.m_Length) {
			return true;
		}

		// keep looking
		idx = idx + 1;
	}

	return false;
}


bool AShard::isEmpty() const
{
	return (m_Length == 0) || (m_Data == nullptr);
}

/*
	Function: rebase

	Description: Take the current data pointer and move it
	such that the offset == 0
	m_Data = m_Data+m_Offset
	m_Offset = 0
*/
bool AShard::rebase()
{
	if (isEmpty()) {
		return false;
	}

	m_Data = m_Data + m_Offset;
	m_Offset = 0;

	return true;
}

char * AShard::tostringz() const
{
	if (isEmpty()) return nullptr;

	// create a stringz
	char *stringz = new char[m_Length+1];
	memcpy(stringz, &m_Data[m_Offset], m_Length);
	stringz[m_Length] = 0;

	return stringz;
}

AShard & AShard::trimfrontspace()
{
	if (isEmpty()) return *this;

    size_t offset = 0;
    while((offset < m_Length) && isspace(m_Data[m_Offset + offset])) {
        offset = offset + 1;
    }
    m_Offset = m_Offset + offset;
    m_Length = m_Length - offset;
    
    return *this;
}


void AShard::print() const
{
	if (!isEmpty()) {
		char *str = tostringz();
		puts(str);

		delete str;
	} else {
		puts("");
	}
}