#include "vmc_bytestream.h"
#include <memory.h>
#include <stdlib.h>
#include <stdio.h>

void vmc_bytestream_init(vmc_bytestream* b)
{
	b->memory = b->current = (vm_byte*)malloc(VMC_BYTESTREAM_RESIZE_SIZE);
	b->end = b->memory + VMC_BYTESTREAM_RESIZE_SIZE;
}

void vmc_bytestream_release(vmc_bytestream* b)
{
	free(b->memory);
}

vm_int32 vmc_bytestream_resize(vmc_bytestream* b, vm_int32 size)
{
	vm_int32 count;
	vm_int32 capacity;
	vm_int32 new_capacity_bytes;
	vm_byte* new_memory;

	count = vmc_bytestream_get_size(b);
	capacity = vmc_bytestream_get_capacity(b);

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

vm_byte* vmc_bytestream_reserve(vmc_bytestream* b, vm_int32 size)
{
	b->current += size;
	if (b->current >= b->end) {
		b->current -= size;
		if (vmc_bytestream_resize(b, size) == -1)
			return NULL;
		b->current += size;
	}
	return b->current - size;
}

vm_byte* vmc_bytestream_write(vmc_bytestream* b, void* bytes, vm_int32 size)
{
	vm_byte* dest = vmc_bytestream_reserve(b, size);
	if (dest == NULL)
		return NULL;
	memcpy(dest, bytes, size);
	return dest;
}

void vmc_bytestream_write_null(vmc_bytestream* b)
{
	vm_byte* dest = vmc_bytestream_reserve(b, 1);
	if (dest == NULL)
		return;
	*dest = 0;
}
