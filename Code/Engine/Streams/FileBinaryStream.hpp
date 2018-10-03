#pragma once
#include "Engine/Streams/BinaryStream.hpp"

class FileBinaryStream : public BinaryStream
{
public:
	FILE *filePointer;

	FileBinaryStream();

	~FileBinaryStream();

	bool OpenForRead(char const *filename);
	bool OpenForWrite(char const *filename);
	void Close();

	virtual unsigned int ReadBytes(void *outBuffer, unsigned int const count);
	virtual unsigned int WriteBytes(void const* buffer, unsigned int const size);

	inline bool IsOpen() const { return nullptr != filePointer; }
};