#include "arMemoryPool.h"
#include "arMemory.h"

BOOL arMemoryPool_init(arMemoryPool* mp)
{
	mp->first = mp->last = (arMemoryPoolEntry*)arMalloc(sizeof(arMemoryPoolEntry));
	if (mp->first == NULL)
		return FALSE;
	mp->last->bytesLeft = ARLANG_MEMORY_POOL_SIZE;
	mp->last->next = NULL;
	memset(mp->last->bytes, 0, ARLANG_MEMORY_POOL_SIZE);
	return TRUE;
}

BOOL arMemoryPool_release(arMemoryPool* mp)
{
	arMemoryPoolEntry* entry = mp->first;
	while (entry) {
		arMemoryPoolEntry* next = entry->next;
		arFree(entry);
		entry = next;
	}
	return TRUE;
}

arByte* arMemoryPool_get(arMemoryPool* mp, arInt32 size)
{
	// Add more memory
	if (mp->last->bytesLeft - size < 0) {
		mp->last->next = (arMemoryPoolEntry*)arMalloc(sizeof(arMemoryPoolEntry));
		mp->last = mp->last->next;
		mp->last->bytesLeft = ARLANG_MEMORY_POOL_SIZE;
		mp->last->next = NULL;
		memset(mp->last->bytes, 0, ARLANG_MEMORY_POOL_SIZE);
	}
	arByte* bytes = &mp->last->bytes[ARLANG_MEMORY_POOL_SIZE - mp->last->bytesLeft];
	mp->last->bytesLeft -= size;
	return bytes;
}
