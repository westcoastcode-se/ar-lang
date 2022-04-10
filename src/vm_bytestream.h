#ifndef _VMC_STREAM_H_
#define _VMC_STREAM_H_

#include "vm_config.h"

#ifndef VMC_BYTESTREAM_RESIZE_SIZE
#	define VMC_BYTESTREAM_RESIZE_SIZE 32768
#endif

// Bytestream which can be used to stream data into a memory buffer
struct vm_bytestream
{
	vm_byte* memory;
	vm_byte* end;
	vm_byte* current;
};

typedef struct vm_bytestream vm_bytestream;

// Initialize the supplied bytestream
extern void vm_bytestream_init(vm_bytestream* b);

// Release the internals of the supplied bytestream. This is only needed if the bytestream is initialized in the first place
extern void vm_bytestream_release(vm_bytestream* b);

// Initialize the supplied bytestream by manually set the internals. You should not release this bytestream if you
// are using this method
extern void vm_bytestream_set(vm_bytestream* b, vm_byte* memory, vm_byte* end);

// Resize the bytestream so that the supplied size fits the memory
extern vm_int32 vm_bytestream_resize(vm_bytestream* b, vm_int32 size);

// Reserve memory and get a pointer to where you can write data to
extern vm_byte* vm_bytestream_reserve(vm_bytestream* b, vm_int32 size);

// Write the supplied memory using the supplied bytes. Returns a pointer to where we've written the data
extern vm_byte* vm_bytestream_write(vm_bytestream* b, void* bytes, vm_int32 size);

// Write a null value
extern void vm_bytestream_write_null(vm_bytestream* b);

// Get the size allocated with bytestream
inline static vm_int32 vm_bytestream_get_size(vm_bytestream* b)
{
	return (vm_int32)(b->current - b->memory);
}

// Fetch memory at the supplied offset location
inline static vm_byte* vm_bytestream_get(vm_bytestream* b, vm_int32 offset)
{
	if (vm_bytestream_get_size(b) < offset)
		return NULL;
	return b->memory + offset;
}

// Get the position in the memory stream where we are
inline static vm_int32 vm_bytestream_pos(vm_bytestream* b)
{
	return vm_bytestream_get_size(b);
}

// Get the capacity of bytestream
inline static vm_int32 vm_bytestream_get_capacity(vm_bytestream* b)
{
	return (vm_int32)(b->end - b->memory);
}

#endif
