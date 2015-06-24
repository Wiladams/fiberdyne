#pragma once

#include "ABlockDevice.h"

#include <assert.h>
#include <stdint.h>

#include <memory>

class  AShard  : public ABlockDevice 
{
private:
protected:
	uint8_t *	m_Data;
	size_t	m_Length;
	size_t	m_Offset;

public:
	// Constructors
	AShard();
	AShard(const char *);
	AShard(uint8_t *data, size_t length, size_t offset);

	// Virtual Destructor
	virtual ~AShard() {};

	// Operator Overloads
	//virtual AShard & operator= (const AShard & rhs);

	// Properties
	virtual AShard &		Set(uint8_t *data, size_t length, size_t offset) {
        assert(!m_Data);

		m_Data = data;
		m_Length = length;
		m_Offset = offset;

		return *this;
	}

	virtual uint8_t *	GetData() const {return &m_Data[m_Offset];}
	virtual AShard &	SetData(uint8_t *data) {m_Data = data; return *this;}
	
	virtual size_t		GetLength() const {return m_Length;}
	virtual AShard &	SetLength(size_t length) {m_Length = length; return *this;}

	virtual size_t		GetOffset() const {return m_Offset;}
	virtual AShard &	SetOffset(size_t offset) {m_Offset = offset; return *this;}

	// Sub-Class Specifics
	virtual bool ReadBytes(uint8_t *buff, const size_t length, size_t *bytesRead = nullptr, size_t timeout = 0, int * error = nullptr) override;
	virtual bool WriteBytes(const uint8_t *buff, const size_t length, size_t *bytesWritten = nullptr, size_t timeout = 0, int * error = nullptr) override;


	// Member functions
	virtual AShard &	clear();
	int			compare(const uint8_t *buff, const size_t length);
	size_t samebytes(const uint8_t *buff, const size_t length);

	AShard &	first(AShard &front, AShard &rest, uint8_t delim) const;
	bool		indexOfChar(const uint8_t achar, size_t &idx) const;
	bool		indexOfShard(const AShard &target, size_t &idx);
	bool 		isEmpty() const;
	void		print() const;
	bool		rebase();
	char *		tostringz() const;
	AShard &	trimfrontspace();
};




