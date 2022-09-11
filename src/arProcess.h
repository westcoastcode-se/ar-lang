#ifndef _arProcess_H_
#define _arProcess_H_

#include "arconf.h"
#include "interpreter/lists.h"
#include "arString.h"

// Represents a unique id
typedef arInt32 arObjectId;

// Information of a function inside a package
typedef struct arFunction
{
	// Unique id for this function
	arObjectId id;

	// Signature of the function
	arString signature;

	// A short-hand version of the signature. This doesn't contain any information of the package
	arString short_signature;

	// Name of the function
	arString name;

	// Pointer to where the function is located
	arIP ptr;

	// How many bytes are expected to be pushed for this function to work
	arInt32 expected_stack_size;
} arFunction;

// Create a new function
ARLANG_API arFunction* arFunction_new();

// Destroy the supplied function
ARLANG_API void arFunction_destroy(arFunction* f);

// Package information
typedef struct arPackage
{
	// Unique id for this package
	arObjectId id;

	// Process this package is part of
	struct arProcess* process;

	// Signature of the package
	arString signature;

	// Name of the package
	arString name;

	// All functions part of this package
	vmi_list_functions functions;
} arPackage;

// Create a new package
ARLANG_API arPackage* arPackage_new();

// Destroy the supplied package
ARLANG_API void arPackage_destroy(arPackage* p);

// The actual virtual machine
typedef struct arProcess
{
	// Bytecode in raw memory
	const arByte* bytecode;

	// All packages
	vmi_list_packages packages;

	// All functions part of this list
	const arFunction** functions;
	arUint32 functions_count;

	// The first thread managed by this process
	struct arThread* first_thread;

	// Current thread that we are executing
	struct arThread* current_thread;
} arProcess;

// Create a new process
ARLANG_API arProcess* arProcess_new();

// Delete the supplied process
ARLANG_API void arProcess_destroy(arProcess* p);

// Load process
ARLANG_API arInt32 arProcess_load(arProcess* p, const arByte* bytecode);

// Execute the supplied thread
ARLANG_API arInt32 arProcess_exec(arProcess* p, struct arThread* t, const arFunction* func);

// Search for a package with the supplied signature
ARLANG_API const arPackage* arProcess_find_package(const arProcess* p, const char* signature, int len);

// Search for a function with the supplied name
ARLANG_API const arFunction* arPackage_find_function(const arPackage* p, const char* name, int len);

// Get a package with the supplied unique id. Returns NULL if no such package exists
ARLANG_API const arPackage* arProcess_get_package(const arProcess* p, arObjectId id);

// Get a function with the supplied unique id. Returns NULL if no such function exists
ARLANG_API const arFunction* arProcess_get_function(const arProcess* p, arObjectId id);

#endif
