#ifndef _arStringPool_H_
#define _arStringPool_H_

#include "arString.h"

// A string entry
typedef struct arStringPool_entry
{
	// The actual string
	arString value;

	// String index
	arUint32 index;

	// Offset where the string starts if all strings are located in the same memory block
	arUint32 offset;

	// Next entry
	struct arStringPool_entry* next;
} arStringPool_entry;

// Pool containing all strings to be saved in the bytecode during the link process
typedef struct arStringPool
{
	// List
	arStringPool_entry* first;
	arStringPool_entry* last;
} arStringPool;

// Initialize the supplied string pool
ARLANG_API void arStringPool_init(arStringPool* p);

// Destroy the internal properties of the supplied string pool
ARLANG_API void arStringPool_destroy(arStringPool* p);

// Search for the supplied string in the string pool. If string is missing then it will be added
ARLANG_API const arString* arStringPool_stringsz(arStringPool* p, const char* str, int len);

// Search for the supplied string in the string pool. If string is missing then it will be added
ARLANG_API const arString* arStringPool_string(arStringPool* p, const arString* s);

#endif
