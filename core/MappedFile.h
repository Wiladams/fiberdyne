#pragma once


#include "AShard.h"

#include <WTypes.h>

#include <stdint.h>

#include <memory>
#include <string>


class MappedFile : public AShard
{
public:
	MappedFile(HANDLE fileHandle, HANDLE mappingHandle, uint8_t *data, uint64_t length);
	virtual ~MappedFile();

	static std::shared_ptr<MappedFile> try_create(const std::string &filename);

protected:

private:
	HANDLE m_FileHandle;
	HANDLE m_MappingHandle;
};

