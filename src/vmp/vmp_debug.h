#ifndef _VMP_DEBUG_H_
#define _VMP_DEBUG_H_

#include <stdlib.h>
#include "../vm_config.h"

#ifndef VMD_DEBUG_MEMORY
#	ifndef NDEBUG
#		define VMD_DEBUG_MEMORY 1
#	endif
#endif

// Allocate memory
extern void* vmp_malloc1(size_t size, const char* filename, vm_int32 line);

// Reallocate memory
extern void* vmp_realloc1(void* ptr, size_t new_size, const char* filename, vm_int32 line);

// Free memory
extern void vmp_free1(void* mem);

// Check to see how many bytes have not been freed
extern BOOL vmp_memory_test_bytes_left();

#ifdef VMD_DEBUG_MEMORY
#define vmp_malloc(size) vmp_malloc1(size, __FILE__, __LINE__)
#define vmp_realloc(ptr, new_size) vmp_realloc1(ptr, new_size, __FILE__, __LINE__)
#define vmp_free(ptr) vmp_free1(ptr)
#else
#define vmp_malloc(size) malloc(size)
#define vmp_realloc(ptr, new_size) realloc(ptr, new_size)
#define vmp_free(ptr) free(ptr)
#endif

#endif
