#ifndef _VMC_STRING_POOL_H_
#define _VMC_STRING_POOL_H_

#include "../vm_string.h"

// A string entry
struct vmc_string_pool_entry
{
	// The actual string
	vm_string value;

	// String index
	vm_uint32 index;

	// Offset where the string starts if all strings are located in the same memory block
	vm_uint32 offset;

	// Next entry
	struct vmc_string_pool_entry* next;
};
typedef struct vmc_string_pool_entry vmc_string_pool_entry;

// Pool containing all strings to be saved in the bytecode during the link process
struct vmc_string_pool
{
	// List
	vmc_string_pool_entry* first;
	vmc_string_pool_entry* last;
};
typedef struct vmc_string_pool vmc_string_pool;

// Initialize the supplied string pool
extern void vmc_string_pool_init(vmc_string_pool* p);

// Destroy the internal properties of the supplied string pool
extern void vmc_string_pool_destroy(vmc_string_pool* p);

// Search for the supplied string in the string pool. If string is missing then it will be added
extern const vm_string* vmc_string_pool_stringsz(vmc_string_pool* p, const char* str, int len);

// Search for the supplied string in the string pool. If string is missing then it will be added
extern const vm_string* vmc_string_pool_string(vmc_string_pool* p, const vm_string* s);

#endif
