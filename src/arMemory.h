#ifndef _VM_DEBUG_H_
#define _VM_DEBUG_H_

#include "arconf.h"

// Allocate memory and add tracking to it
ARLANG_API void* arDebugMemory_malloc(size_t size, const char* filename, arInt32 line);

// Reallocate memory
ARLANG_API void* arDebugMemory_realloc(void* ptr, size_t new_size, const char* filename, arInt32 line);

// Free memory
ARLANG_API void arDebugMemory_free(void* mem);

// Check to see how many bytes have not been freed
ARLANG_API BOOL arDebugMemory_verify();

// Clear the memory being tracked by the by the memory tracker
ARLANG_API void arDebugMemory_clear();

// Helper utility to track memory allocations so that we can track any potential
// memory leaks in the library
#ifdef ARLANG_MEMORY_TRACKING
#define arMalloc(size) arDebugMemory_malloc(size, __FILE__, __LINE__)
#define arRealloc(ptr, new_size) arDebugMemory_realloc(ptr, new_size, __FILE__, __LINE__)
#define arFree(ptr) arDebugMemory_free(ptr)
#else
#define arMalloc(size) malloc(size)
#define arRealloc(ptr, new_size) realloc(ptr, new_size)
#define arFree(ptr) free(ptr)
#endif

// Do an allocate and return if out-of-memory
#define arSafeMalloc(TYPE) (TYPE*)arMalloc(sizeof(TYPE)); if (p == NULL) return p

#endif
