#ifndef _ARLANG_MEMORY_POOL_H_
#define _ARLANG_MEMORY_POOL_H_

#include "arconf.h"

// A memory pool entry
typedef struct arMemoryPoolEntry {
	arByte bytes[ARLANG_MEMORY_POOL_SIZE];
	arInt32 bytesLeft;
	struct arMemoryPoolEntry* next;
} arMemoryPoolEntry;

// Memory pool which can be used to fetch memory that can be used
typedef struct arMemoryPool {
	arMemoryPoolEntry* first;
	arMemoryPoolEntry* last;
} arMemoryPool;

// Initialize the internal memory of a memory pool
ARLANG_API BOOL arMemoryPool_init(arMemoryPool* mp);

// Release the internal memory of a memory pool
ARLANG_API BOOL arMemoryPool_release(arMemoryPool* mp);

// Get memory from the memory pool
ARLANG_API arByte* arMemoryPool_get(arMemoryPool* mp, arInt32 size);

#endif

