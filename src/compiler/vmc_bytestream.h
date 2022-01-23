#ifndef _VMC_STREAM_H_
#define _VMC_STREAM_H_

#include "../vm_config.h"

#ifndef VMC_BYTESTREAM_RESIZE_SIZE
#	define VMC_BYTESTREAM_RESIZE_SIZE 32768
#endif

// Bytestream which can be used to stream data into a memory buffer
struct vmc_bytestream
{
	vm_byte* memory;
	vm_byte* end;
	vm_byte* current;
};

typedef struct vmc_bytestream vmc_bytestream;

// Initialize the supplied bytestream
extern void vmc_bytestream_init(vmc_bytestream* b);

// Release the internal bytestream
extern void vmc_bytestream_release(vmc_bytestream* b);

// Resize the bytestream so that the supplied size fits the memory
extern vm_int32 vmc_bytestream_resize(vmc_bytestream* b, vm_int32 size);

// Reserve memory and get a pointer to where you can write data to
extern vm_byte* vmc_bytestream_reserve(vmc_bytestream* b, vm_int32 size);

// Write the supplied memory using the supplied bytes. Returns a pointer to where we've written the data
extern vm_byte* vmc_bytestream_write(vmc_bytestream* b, void* bytes, vm_int32 size);

// Write a null value
extern void vmc_bytestream_write_null(vmc_bytestream* b);

// Get the size allocated with bytestream
inline static vm_int32 vmc_bytestream_get_size(vmc_bytestream* b)
{
	return (vm_int32)(b->current - b->memory);
}

// Fetch memory at the supplied offset location
inline static vm_byte* vmc_bytestream_get(vmc_bytestream* b, vm_int32 offset)
{
	if (vmc_bytestream_get_size(b) < offset)
		return NULL;
	return b->memory + offset;
}

// Get the position in the memory stream where we are
inline static vm_int32 vmc_bytestream_pos(vmc_bytestream* b)
{
	return vmc_bytestream_get_size(b);
}

// Get the capacity of bytestream
inline static vm_int32 vmc_bytestream_get_capacity(vmc_bytestream* b)
{
	return (vm_int32)(b->end - b->memory);
}

#endif
