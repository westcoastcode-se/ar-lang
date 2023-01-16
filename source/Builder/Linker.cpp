#include "Linker.h"
#include "../interpreter/Bytecode.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Builder;
using namespace WestCoastCode::Interpreter;

Linker::~Linker()
{
	for (auto p : _packages)
		delete p;
}

Bytes Linker::Link()
{
	// Figure out the header size
	const I32 totalHeaderSize = CalculateTotalHeaderSize();
	// Figure out where the read-only memory blocks are located
	const I32 readOnlyOffset = ResolveGlobalVariables(totalHeaderSize);
	// Figure out the total instructions body size
	const I32 totalSize = ResolveFunctionBody(readOnlyOffset);

	// Header
	MemoryStream stream(totalSize);
	ProcessHeader* const header = stream.Reserve<ProcessHeader>();
	header->header[0] = 'A';
	header->header[1] = 'R';
	header->header[2] = 'L';
	header->header[3] = '0';
	header->version = ARLANG_INTERPRETER_VERSION;
	header->codeOffset = readOnlyOffset;
	header->codeSize = totalSize - readOnlyOffset;
	header->dataOffset = totalHeaderSize;
	header->dataSize = totalSize - (header->codeSize + totalHeaderSize);
	header->packagesCount = (I32)_packages.Size();
	header->packageOffset = sizeof(ProcessHeader);

	// Serialize all content
	SerializeHeaders(stream);
	SerializeWrite(stream);
	SerializeReadOnly(stream);
	return stream.Done();
}

Package* Linker::AddPackage(Package* package)
{
	_packages.Add(package);
	return package;
}

I32 Linker::CalculateTotalHeaderSize() const
{
	I32 size = sizeof(ProcessHeader);
	for (auto p : _packages)
		size = p->ResolveHeaderMemory(size);
	return size;
}

I32 Linker::ResolveGlobalVariables(I32 offset)
{
	for (auto p : _packages)
		offset = p->ResolveGlobalVariables(offset);
	return offset;
}

I32 Linker::ResolveFunctionBody(I32 offset)
{
	for (auto p : _packages)
		offset = p->ResolveFunctionBody(offset);
	return offset;
}

void Linker::SerializeHeaders(MemoryStream& stream)
{
	for (auto p : _packages)
		p->SerializeHeader(stream);
}

void Linker::SerializeWrite(MemoryStream& stream)
{
	for (auto p : _packages)
		p->Serialize(stream);
}

void Linker::SerializeReadOnly(MemoryStream& stream)
{
	for (auto p : _packages)
		p->SerializeReadOnly(stream);
}
