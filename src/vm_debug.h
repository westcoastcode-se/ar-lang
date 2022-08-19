#ifndef _VM_DEBUG_H_
#define _VM_DEBUG_H_

#include "vm_config.h"

// Define used for enabling/disabling memory allocation tracking
#ifndef VM_DEBUG_MEMORY_TRACKING
#	ifndef NDEBUG
#		define VM_DEBUG_MEMORY_TRACKING 1
#	endif
#endif

// Allocate memory and add tracking to it
extern void* vm_malloc1(size_t size, const char* filename, vm_int32 line);

// Reallocate memory
extern void* vm_realloc1(void* ptr, size_t new_size, const char* filename, vm_int32 line);

// Free memory
extern void vm_free1(void* mem);

// Check to see how many bytes have not been freed
extern BOOL vm_memory_test_bytes_left();

// Clear the memory being tracked by the by the memory tracker
extern void vm_memory_test_clear();

#ifdef VM_DEBUG_MEMORY_TRACKING
#define vm_malloc(size) vm_malloc1(size, __FILE__, __LINE__)
#define vm_realloc(ptr, new_size) vm_realloc1(ptr, new_size, __FILE__, __LINE__)
#define vm_free(ptr) vm_free1(ptr)
#else
#define vm_malloc(size) malloc(size)
#define vm_realloc(ptr, new_size) realloc(ptr, new_size)
#define vm_free(ptr) free(ptr)
#endif

// Do an allocate and return if out-of-memory
#define vm_safe_malloc(TYPE) (TYPE*)vm_malloc(sizeof(TYPE)); if (p == NULL) return p

#endif
