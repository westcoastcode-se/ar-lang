#ifndef _ARLANG_UTILS_BYTESTREAM_H_
#define _ARLANG_UTILS_BYTESTREAM_H_

#include "arconf.h"

// Bytestream which can be used to stream data into a memory buffer
typedef struct arByteStream
{
	arByte* memory;
	arByte* end;
	arByte* current;
} arByteStream;

// Initialize the supplied bytestream
ARLANG_API void arByteStream_init(arByteStream* b);

// Release the internals of the supplied bytestream. This is only needed if the bytestream is initialized in the first place
ARLANG_API void arByteStream_release(arByteStream* b);

// Detach the bytecode built by the bytestream. This ensures that the internals are not deleted when releasing the stream.
// This means that the one calling this method is responsible for freeing the memory themselves
ARLANG_API arByte* arByteStream_detach(arByteStream* b);

// Initialize the supplied bytestream by manually set the internals. You should not release this bytestream if you
// are using this method
ARLANG_API void arByteStream_set(arByteStream* b, arByte* memory, arByte* end);

// Resize the bytestream so that the supplied size fits the memory
ARLANG_API arInt32 arByteStream_resize(arByteStream* b, arInt32 size);

// Reserve memory and get a pointer to where you can write data to
ARLANG_API arByte* arByteStream_reserve(arByteStream* b, arInt32 size);

// Write the supplied memory using the supplied bytes. Returns a pointer to where we've written the data
ARLANG_API arByte* arByteStream_write(arByteStream* b, const void* bytes, arInt32 size);

// Write a null value
ARLANG_API void arByteStream_write_null(arByteStream* b);

// Get the size allocated with bytestream
ARLANG_API arInt32 arByteStream_get_size(arByteStream* b);

// Fetch memory at the supplied offset location
ARLANG_API arByte* arByteStream_get(arByteStream* b, arInt32 offset);

// Get the position in the memory stream where we are
ARLANG_API arInt32 arByteStream_pos(arByteStream* b);

// Get the capacity of bytestream
ARLANG_API arInt32 arByteStream_get_capacity(arByteStream* b);

#endif
