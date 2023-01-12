#pragma once

#include "../Common.h"

namespace WestCoastCode::Interpreter
{
	// Header that describes the bytecode. The header is always at the start of the binary blob
	struct ProcessHeader
	{
		// "ARL0"
		Byte header[4];

		// Compatibility version
		I32 version;

		// Offset where the code starts
		I32 codeOffset;

		// Size, in bytes, for the instructions block
		I32 codeSize;

		// Offset where data that we are allowed to write to is located
		I32 dataOffset;

		// How many bytes are reserved for the data block
		I32 dataSize;

		// Number of packages compiled and linked with the bytecode
		I32 packagesCount;

		// Offset where the first package is located
		I32 packageOffset;
	};
	static_assert(sizeof(ProcessHeader) == 32, "Expected header size to be 32 bytes");

	// Header for packages
	struct ProcessPackageHeader
	{
		// Size, in bytes, of the signature. The actual bytes 
		I32 signatureLength;

		// Offset where the signature bytes are found
		I32 signatureOffset;

		// Number of types part of this package
		I32 typesCount;

		// Where the first type is located
		I32 typesOffset;

		// Number of globals part of this package
		I32 globalsCount;

		// Where the first global is located
		I32 globalsOffset;

		// Number of functions part of this package
		I32 functionsCount;

		// Offset where the function headers are found
		I32 functionsOffset;

		// Offset to where the next package is located
		I32 offset;
	};
	static_assert(sizeof(ProcessPackageHeader) == 36, "Expected header size to be 36 bytes");

	// Header for a package in the bytecode
	struct ProcessTypeHeader
	{
		// Length of the name
		I32 signatureLength;

		// Offset where the signature bytes are found
		I32 signatureOffset;

		// How many bytes are expected to be pushed for this function to work
		I32 expectedStackSize;

		// Offset to the next type
		I32 offset;

		// Extra padding
		Byte padding[16];
	};
	static_assert(sizeof(ProcessTypeHeader) == 32, "Expected header size to be 32 bytes");

	// Header for a package in the bytecode
	struct ProcessGlobalHeader
	{
		// Length of the name
		I32 signatureLength;

		// Offset where the signature bytes are found
		I32 signatureOffset;

		// Offset where the global memory starts (in relation to the start of the bytecode)
		I32 ptr;

		// How many bytes the global variable takes 
		I32 expectedSize;

		// Offset to where the type header is located
		I32 typeOffset;

		// Offset to the next global
		I32 offset;

		// Extra padding
		Byte padding[8];
	};
	static_assert(sizeof(ProcessGlobalHeader) == 32, "Expected header size to be 32 bytes");

	// Header for a package in the bytecode
	struct ProcessFunctionHeader
	{
		// Length of the name
		I32 signatureLength;

		// Offset where the signature bytes are found
		I32 signatureOffset;

		// Offset where the function starts (in relation to the start of the bytecode)
		I32 entryOffset;

		// How many bytes are expected to be pushed as arguments for this function
		I32 expectedStackSize;

		// Offset to the next function
		I32 offset;

		// Extra padding
		Byte padding[12];
	};
	static_assert(sizeof(ProcessFunctionHeader) == 32, "Expected header size to be 32 bytes");
}
