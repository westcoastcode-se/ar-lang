#pragma once

#include "../Common.h"
#include "Symbols.h"

namespace WestCoastCode::Builder
{
	// Linker used to build bytecode that the virtual machine understands
	// based on symbol
	class ARLANG_API Linker
	{
	public:
		~Linker();

		// Link all symbols together and generates bytecode that can
		// be interpreted by the virtual machine. Raises LinkError if
		// this fails
		Bytes Link();

		// Add the supplied package to be linked
		Package* AddPackage(Package* package);

	private:
		// Calculate the total header size
		I32 CalculateTotalHeaderSize() const;

		// Resolve offsets of the write allowed area
		I32 ResolveGlobalVariables(I32 offset);

		// Resolve offsets of the read-only area
		I32 ResolveFunctionBody(I32 offset);

		// Serialize the header information of all symbols
		void SerializeHeaders(MemoryStream& stream);

		// Serialzie memory in which we are allowed to write to
		void SerializeWrite(MemoryStream& stream);

		// Serialize memory that we are not allowed to write to
		void SerializeReadOnly(MemoryStream& stream);

	private:
		Vector<Package*> _packages;
	};
}
