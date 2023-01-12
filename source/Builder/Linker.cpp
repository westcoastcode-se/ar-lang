#include "Linker.h"
#include "../interpreter/Bytecode.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Builder;
using namespace WestCoastCode::Interpreter;

Linker::~Linker()
{
	for (auto& p : _packages)
		delete p;
}

Bytes Linker::Link()
{
	// Figure out the header size
	const I32 totalHeaderSize = CalculateTotalHeaderSize();
	// Figure out where the read-only memory blocks are located
	const I32 readOnlyOffset = ResolveWrite(totalHeaderSize);
	// Figure out the total instructions body size
	const I32 totalSize = ResolveInstructions(readOnlyOffset);

	// Header
	MemoryStream stream(totalSize);
	ProcessHeader* const header = stream.Reserve<ProcessHeader>();
	header->header[0] = 'A';
	header->header[1] = 'R';
	header->header[2] = 'L';
	header->header[3] = '0';
	header->version = VM_VERSION;
	header->codeOffset = readOnlyOffset;
	header->codeSize = totalSize - readOnlyOffset;
	header->dataOffset = totalHeaderSize;
	header->dataSize = totalSize - readOnlyOffset - totalHeaderSize;
	header->packagesCount = (I32)_packages.size();
	header->packageOffset = sizeof(ProcessHeader);

	// Serialize all content
	SerializeHeaders(stream);
	SerializeWrite(stream);
	SerializeReadOnly(stream);
	return stream.Done();
}

Package* Linker::AddPackage(Package* package)
{
	_packages.push_back(package);
	return package;
}

I32 Linker::CalculateTotalHeaderSize() const
{
	I32 size = sizeof(ProcessHeader);
	// The size for each package
	for (auto& p : _packages) {
		size += sizeof(ProcessPackageHeader);
		size += (I32)p->GetSignature().size();

		// Calculate the size for each global
		for (auto& t : p->GetTypes()) {
			size += sizeof(ProcessGlobalHeader);
			size += (I32)t->GetSignature().size();
		}

		// Calculate the size for each global
		for (auto& g : p->GetGlobals()) {
			size += sizeof(ProcessGlobalHeader);
			size += (I32)g->GetSignature().size();
		}

		// Calculate the size for each function
		for (auto& f : p->GetFunctions()) {
			size += sizeof(ProcessFunctionHeader);
			size += (I32)f->GetSignature().size();
		}
	}
	return size;
}

I32 Linker::ResolveWrite(I32 offset)
{
	for (auto& p : _packages)
		offset = p->ResolveWriteMemory(offset);
	return offset;
}

I32 Linker::ResolveInstructions(I32 offset)
{
	for (auto& p : _packages)
		offset = p->ResolveReadOnlyMemory(offset);
	return offset;
}

void Linker::SerializeHeaders(MemoryStream& stream)
{
	for (auto& p : _packages)
		p->SerializeHeader(stream);
}

void Linker::SerializeWrite(MemoryStream& stream)
{
	for (auto& p : _packages)
		p->Serialize(stream);
}

void Linker::SerializeReadOnly(MemoryStream& stream)
{
	for (auto& p : _packages)
		p->SerializeReadOnly(stream);
}
