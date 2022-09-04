#include "arByteStream.h"
#include "arMemory.h"
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

#define ARLANG_BYTESTREAM_RESIZE_SIZE 32768

void arByteStream_init(arByteStream* b)
{
	b->memory = b->current = (arByte*)arMalloc(ARLANG_BYTESTREAM_RESIZE_SIZE);
	b->end = b->memory + ARLANG_BYTESTREAM_RESIZE_SIZE;
}

void arByteStream_release(arByteStream* b)
{
	if (b->memory != NULL)
		arFree(b->memory);
}

void arByteStream_set(arByteStream* b, arByte* memory, arByte* end)
{
	b->memory = b->current = memory;
	b->end = end;
}

arByte* arByteStream_detach(arByteStream* b)
{
	arByte* const result = b->memory;
	b->memory = NULL;
	b->current = NULL;
	b->end = NULL;
	return result;
}

arInt32 arByteStream_resize(arByteStream* b, arInt32 size)
{
	arInt32 count;
	arInt32 capacity;
	arInt32 new_capacity_bytes;
	arByte* new_memory;

	count = arByteStream_get_size(b);
	capacity = arByteStream_get_capacity(b);

	// Figure out the next size of the stack by resizing it in blocks of "VMC_BYTESTREAM_RESIZE_SIZE"
	for (new_capacity_bytes = capacity + ARLANG_BYTESTREAM_RESIZE_SIZE; new_capacity_bytes < count + size;
		new_capacity_bytes += ARLANG_BYTESTREAM_RESIZE_SIZE);

	// Try to reallocate the memory so that we have enough to fill the requested size
	new_memory = (char*)arRealloc(b->memory, new_capacity_bytes);
	if (new_memory == NULL) {
		return -1;
	}
	b->memory = new_memory;
	b->end = new_memory + new_capacity_bytes;
	b->current = b->memory + count;
	return new_capacity_bytes;
}

arByte* arByteStream_reserve(arByteStream* b, arInt32 size)
{
	b->current += size;
	if (b->current >= b->end) {
		b->current -= size;
		if (arByteStream_resize(b, size) == -1)
			return NULL;
		b->current += size;
	}
	return b->current - size;
}

arByte* arByteStream_write(arByteStream* b, const void* bytes, arInt32 size)
{
	arByte* dest = arByteStream_reserve(b, size);
	if (dest == NULL)
		return NULL;
	memcpy(dest, bytes, size);
	return dest;
}

void arByteStream_write_null(arByteStream* b)
{
	arByte* dest = arByteStream_reserve(b, 1);
	if (dest == NULL)
		return;
	*dest = 0;
}

// Get the size allocated with bytestream
arInt32 arByteStream_get_size(arByteStream* b)
{
	return (arInt32)(b->current - b->memory);
}

// Fetch memory at the supplied offset location
arByte* arByteStream_get(arByteStream* b, arInt32 offset)
{
	if (arByteStream_get_size(b) < offset)
		return NULL;
	return b->memory + offset;
}

// Get the position in the memory stream where we are
arInt32 arByteStream_pos(arByteStream* b)
{
	return arByteStream_get_size(b);
}

// Get the capacity of bytestream
arInt32 arByteStream_get_capacity(arByteStream* b)
{
	return (arInt32)(b->end - b->memory);
}
