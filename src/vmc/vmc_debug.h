#ifndef _VMC_MEMORY_H_
#define _VMC_MEMORY_H_

#include <stdlib.h>
#include "../vm_config.h"

#ifndef VMC_DEBUG_MEMORY
#	ifndef NDEBUG
#		define VMC_DEBUG_MEMORY 1
#	endif
#endif

// Allocate memory
extern void* vmc_malloc1(size_t size, const char* filename, vm_int32 line);

// Reallocate memory
extern void* vmc_realloc1(void* ptr, size_t new_size, const char* filename, vm_int32 line);

// Free memory
extern void vmc_free1(void* mem);

// Check to see how many bytes have not been freed
extern BOOL vmc_memory_test_bytes_left();

#ifdef VMC_DEBUG_MEMORY
#define vmc_malloc(size) vmc_malloc1(size, __FILE__, __LINE__)
#define vmc_realloc(ptr, new_size) vmc_realloc1(ptr, new_size, __FILE__, __LINE__)
#define vmc_free(ptr) vmc_free1(ptr)
#else
#define vmc_malloc(size) malloc(size)
#define vmc_realloc(ptr, new_size) realloc(ptr, new_size)
#define vmc_free(ptr) free(ptr)
#endif

#endif
