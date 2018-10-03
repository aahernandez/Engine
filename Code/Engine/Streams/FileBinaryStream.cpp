#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Streams/FileBinaryStream.hpp"

FileBinaryStream::FileBinaryStream()
	: filePointer(nullptr)
{}

FileBinaryStream::~FileBinaryStream()
{
	Close();
}

bool FileBinaryStream::OpenForRead(char const *filename)
{
// 	ASSERT_OR_DIE(!IsOpen(), "File already open");
	filePointer = fopen(filename, "rb");
	return IsOpen();
}

bool FileBinaryStream::OpenForWrite(char const *filename)
{
// 	ASSERT_OR_DIE(!IsOpen(), "File already open");
	filePointer = fopen(filename, "wb");
	return IsOpen();
}

void FileBinaryStream::Close()
{
	if (IsOpen())
	{
		fclose(filePointer);
		filePointer = nullptr;
	}
}

unsigned int FileBinaryStream::ReadBytes(void *outBuffer, unsigned int const count)
{
	unsigned int bytesRead = 0;

	if (IsOpen())
	{
		bytesRead = (unsigned int)fread(outBuffer, 1, count, filePointer);
	}

	return bytesRead;
}

unsigned int FileBinaryStream::WriteBytes(void const* buffer, unsigned int const size)
{
	unsigned int bytesRead = 0;

	if (IsOpen())
	{
		bytesRead = (unsigned int)fwrite(buffer, 1, size, filePointer);
	}

	return bytesRead;
}