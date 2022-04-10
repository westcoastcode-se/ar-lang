#include "vm_bytestream.h"
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

void vm_bytestream_init(vm_bytestream* b)
{
	b->memory = b->current = (vm_byte*)malloc(VMC_BYTESTREAM_RESIZE_SIZE);
	b->end = b->memory + VMC_BYTESTREAM_RESIZE_SIZE;
}

void vm_bytestream_release(vm_bytestream* b)
{
	free(b->memory);
}

void vm_bytestream_set(vm_bytestream* b, vm_byte* memory, vm_byte* end)
{
	b->memory = b->current = memory;
	b->end = end;
}

vm_int32 vm_bytestream_resize(vm_bytestream* b, vm_int32 size)
{
	vm_int32 count;
	vm_int32 capacity;
	vm_int32 new_capacity_bytes;
	vm_byte* new_memory;

	count = vm_bytestream_get_size(b);
	capacity = vm_bytestream_get_capacity(b);

	// Figure out the next size of the stack by resizing it in blocks of "VMC_BYTESTREAM_RESIZE_SIZE"
	for (new_capacity_bytes = capacity + VMC_BYTESTREAM_RESIZE_SIZE; new_capacity_bytes < count + size;
		new_capacity_bytes += VMC_BYTESTREAM_RESIZE_SIZE);

	// Try to reallocate the memory so that we have enough to fill the requested size
	new_memory = (char*)realloc(b->memory, new_capacity_bytes);
	if (new_memory == NULL) {
		return -1;
	}
	b->memory = new_memory;
	b->end = new_memory + new_capacity_bytes;
	b->current = b->memory + count;
	return new_capacity_bytes;
}

vm_byte* vm_bytestream_reserve(vm_bytestream* b, vm_int32 size)
{
	b->current += size;
	if (b->current >= b->end) {
		b->current -= size;
		if (vm_bytestream_resize(b, size) == -1)
			return NULL;
		b->current += size;
	}
	return b->current - size;
}

vm_byte* vm_bytestream_write(vm_bytestream* b, void* bytes, vm_int32 size)
{
	vm_byte* dest = vm_bytestream_reserve(b, size);
	if (dest == NULL)
		return NULL;
	memcpy(dest, bytes, size);
	return dest;
}

void vm_bytestream_write_null(vm_bytestream* b)
{
	vm_byte* dest = vm_bytestream_reserve(b, 1);
	if (dest == NULL)
		return;
	*dest = 0;
}
