#include "MemoryStream.h"
#include <memory>

using namespace WestCoastCode;
using namespace WestCoastCode::Builder;

MemoryStream::MemoryStream(I32 size)
	: _bytes((Bytes)malloc(size)), _ptr(_bytes)
{
	assert(_bytes != nullptr);
}

MemoryStream::~MemoryStream()
{
	if (_bytes)
		free(_bytes);
}

void MemoryStream::Write(const void* ptr, I32 size)
{
	memcpy(_ptr, (const Byte*)ptr, size);
	_ptr += size;
}

Bytes MemoryStream::Done()
{
	Bytes temp = _bytes;
	_bytes = nullptr;
	return temp;
}
