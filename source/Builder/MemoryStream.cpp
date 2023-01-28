#include "MemoryStream.h"
#include "LinkError.h"
#include "../Interpreter/Instructions.h"
#include "../Memory.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Builder;

MemoryStream::MemoryStream(I32 size)
	: _bytes((Bytes)malloc(size)), _ptr(_bytes), _end(_bytes + size)
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
	if (_ptr + size > _end)
		throw LinkErrorInvalidBytecodeSize();

	memcpy(_ptr, (const Byte*)ptr, size);
	_ptr += size;
}

Bytes MemoryStream::Done()
{
	if (_ptr != _end)
		throw LinkErrorInvalidBytecodeSize();

	Bytes temp = _bytes;
	_bytes = nullptr;
	return temp;
}

void MemoryStream::Eoe()
{
	Interpreter::InstrEoe eoe;
	eoe.opcode = 0;
	eoe.incode = Interpreter::Incode::Eoe;
	Write(&eoe);
}
