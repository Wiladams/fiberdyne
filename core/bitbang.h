#pragma once

#include <stdint.h>




namespace bitbang {
// Calculate the minimum number of bytes needed
// to represent the integer value.
inline size_t BS32BytesNeeded(uint32_t val)
{
	uint32_t mask;
	size_t size = 4;

	for (mask = 0xff000000; mask > 0x000000ff; mask >>=8) {
		if (val & mask) {
			break;
		}
		size--;
	}

	return size;
}


// Returns the count of bits set in the input for types up to 128 bits
// use it with unsigned types like this:
// BitCount((uint32_t)0x10110000);
template<class T> inline T BitCount(T v)
{
	// From Stanford Bit Twiddling Hacks collection
	// http://graphics.stanford.edu/~seander/bithacks.html#CountBitsSetParallel
	v = v - ((v >> 1) & (T)~(T)0/3);
	v = (v & (T)~(T)0/15*3) + ((v >> 2) & (T)~(T)0/15*3);
	v = (v + (v >> 4)) & (T)~(T)0/255*15;
	return (T)(v * ((T)~(T)0/255)) >> ((sizeof(v) - 1) * 8);
}

// Some convenient type specific functions for bit counting
inline size_t BitCount8(uint8_t v) { return BitCount((uint8_t)v); }
inline size_t BitCount16(uint16_t v) { return BitCount((uint16_t)v); }

// specialize for 32-bit int
// saves on a few instructions
size_t BitCount32(uint32_t v)
{
	v = v - ((v >> 1) & 0x55555555);                    // reuse input as temporary
	v = (v & 0x33333333) + ((v >> 2) & 0x33333333);     // temp
	return ((v + (v >> 4) & 0xF0F0F0F) * 0x1010101) >> 24; // count
}


size_t BitCount64(uint64_t v) 
{ 
	return (size_t)BitCount((uint64_t)v); 
}



// Determine whether current machine is bigendian
// for integer values
bool IsBigEndian() 
{
	int i=1; 
	return ! *((char *)&i);
}

bool IsLittleEndian() 
{ 
	return !IsBigEndian(); 
}



// Low level bit manipulations
template<typename T>
T clearbit(T & value, size_t bit)
{
	value = ((T)value & ~((T)(((T)1) << bit)));	// Clear a bit in a single byte
	return value;
}

template<typename T>
bool isset(T value, size_t bit)
{
	return	(value & (T)(((T)1) << bit)) != 0;
}

template<typename T>
T setbit(T & value, size_t bit)
{
	value = (value | (((T)1) << (bit)));
	return value;
}


int getbitbyteoffset(const int bitnumber, int bigendian, int *whichbyte)
{
    int bitoffset = 0;
    whichbyte[0] = bitnumber >> 3;  //math.floor(bitnumber /8)

    if (bigendian) {
        bitoffset = 7 - (bitnumber % 8);
    } else {
        bitoffset = bitnumber % 8;
    }

    return bitoffset;
}

uint64_t getbitsfrombytes(const uint8_t *bytes, const int startbit, const int bitcount, const int bigendian)
{
    uint64_t value = 0;
    int byteoffset, bitoffset;
    int i = 1;

    while (i <= bitcount) {
        bitoffset = getbitbyteoffset(startbit+i-1, bigendian, &byteoffset);
        auto bitval = isset(bytes[byteoffset], bitoffset);

        if (bitval) {
            if (bigendian) {
                value = setbit(value, bitcount - i);
            } else {
                value = setbit(value, i-1);
            }
        }
        i = i + 1;
    }

    return value;
}


/*
    READING

    Read various fixed sized integers out of a byte array

    Comment: Can deal with big or little endian
*/

uint64_t bb_readUIntN(const size_t n, const uint8_t *bytes, const int offset, const bool bigendian)
{
    //std::cout << "readUIntN: " << n << "\tOffset: " << offset << "BIGENDIAN: " << (int)bigendian << std::endl;

    uint64_t value = 0;

	if (bigendian) {
		for (size_t i=0; i<n; i++) {
			value = (value << 8) + bytes[offset+i];            
		}
	} else {
		for (size_t i=n-1; i>=0; i--) {
			value = (value << 8) + bytes[offset+i];            
		}
	}

    return value;
}


uint8_t readUInt8(const uint8_t * bytes, int offset, bool bigendian)
{
    return bytes[offset];
}

uint16_t readUInt16(const uint8_t *bytes, int offset, bool bigendian)
{
    return (uint16_t)bb_readUIntN(2, bytes, offset, bigendian);
}


uint32_t readUInt32(const uint8_t * bytes, int offset, bool bigendian)
{
    return (uint32_t)bb_readUIntN(4, bytes, offset, bigendian);
}


uint64_t readUInt64(const uint8_t * bytes, int offset, bool bigendian)
{
    return (uint64_t)bb_readUIntN(8, bytes, offset, bigendian);
}


//
//  Write number to buffer
//
bool bb_writeUIntN(const size_t n, uint64_t value, uint8_t *bytes, int offset, int bigendian)
{
    size_t i = 0;
    while (i<n)
    {
        if (bigendian){
            bytes[offset+(n-i-1)] = (value >> (8*i)) & 0xff;
        } else {
            bytes[offset+i] = (value >> (8*i)) & 0xff;
        }
        i = i + 1;
    }

    return true;
}

int writeUInt8(uint8_t value, uint8_t *bytes, int offset, int bigendian)
{
    bytes[offset] = value;
    return 1;
}

int writeUint16(uint16_t value, uint8_t *bytes, int offset, int bigendian)
{
    if (bigendian) {
        bytes[offset+0] = (uint8_t)((value & 0xff00)>>8);
        bytes[offset+1] = (uint8_t)((value & 0x00ff));
    } else
    {
        bytes[offset+1] = (uint8_t)((value & 0xff00)>>8);
        bytes[offset+0] = (uint8_t)((value & 0x00ff));        
    }

    return 1;
}


int writeUInt32(uint32_t value, uint8_t *bytes, int offset, int bigendian)
{
    if (bigendian) {
        bytes[offset+0] = (uint8_t)((value & 0xff00)>>24);
        bytes[offset+1] = (uint8_t)((value & 0xff00)>>16);
        bytes[offset+2] = (uint8_t)((value & 0xff00)>>8);
        bytes[offset+3] = (uint8_t)((value & 0x00ff));
    } else
    {
        bytes[offset+3] = (uint8_t)((value & 0xff00)>>24);
        bytes[offset+2] = (uint8_t)((value & 0xff00)>>16);
        bytes[offset+1] = (uint8_t)((value & 0xff00)>>8);
        bytes[offset+0] = (uint8_t)((value & 0x00ff));
    }

    return 1;
}


/*
 public override double ReadDouble()
{
    if (Endianness == Endianness.LittleEndian)
    {
        return base.ReadDouble();
    }
 
    byte[] bytes = base.ReadBytes(8);
 
    ulong low = 0x0000000ffffffff & (ulong)(((bytes[7] | (bytes[6] << 8)) | (bytes[5] << 0x10)) | (bytes[4] << 0x18));
    ulong high = 0x0000000ffffffff & (ulong)(((bytes[3] | (bytes[2] << 8)) | (bytes[1] << 0x10)) | (bytes[0] << 0x18));
    ulong result = (high << 0x20) | low;
 
    double value = *(((double*)&result);
 
    return value;
}
*/

}

