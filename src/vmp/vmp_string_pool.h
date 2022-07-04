#ifndef _VMP_STRING_POOL_H_
#define _VMP_STRING_POOL_H_

#include "../vm_string.h"

// A string entry
struct vmp_string_pool_entry
{
	// The actual string
	vm_string value;

	// String index
	vm_uint32 index;

	// Offset where the string starts if all strings are located in the same memory block
	vm_uint32 offset;

	// Next entry
	struct vmp_string_pool_entry* next;
};
typedef struct vmp_string_pool_entry vmp_string_pool_entry;

// Pool containing all strings to be saved in the bytecode during the link process
struct vmp_string_pool
{
	// List
	vmp_string_pool_entry* first;
	vmp_string_pool_entry* last;
};
typedef struct vmp_string_pool vmp_string_pool;

// Initialize the supplied string pool
extern void vmp_string_pool_init(vmp_string_pool* p);

// Destroy the internal properties of the supplied string pool
extern void vmp_string_pool_destroy(vmp_string_pool* p);

// Search for the supplied string in the string pool. If string is missing then it will be added
extern const vm_string* vmp_string_pool_stringsz(vmp_string_pool* p, const char* str, int len);

// Search for the supplied string in the string pool. If string is missing then it will be added
extern const vm_string* vmp_string_pool_string(vmp_string_pool* p, const vm_string* s);

#endif
