#ifndef _VM_STRING_POOL_H_
#define _VM_STRING_POOL_H_

#include "vm_string.h"

// A string entry
typedef struct vm_string_pool_entry
{
	// The actual string
	vm_string value;

	// String index
	vm_uint32 index;

	// Offset where the string starts if all strings are located in the same memory block
	vm_uint32 offset;

	// Next entry
	struct vm_string_pool_entry* next;
} vm_string_pool_entry;

// Pool containing all strings to be saved in the bytecode during the link process
typedef struct vm_string_pool
{
	// List
	vm_string_pool_entry* first;
	vm_string_pool_entry* last;
} vm_string_pool;

// Initialize the supplied string pool
extern void vm_string_pool_init(vm_string_pool* p);

// Destroy the internal properties of the supplied string pool
extern void vm_string_pool_destroy(vm_string_pool* p);

// Search for the supplied string in the string pool. If string is missing then it will be added
extern const vm_string* vm_string_pool_stringsz(vm_string_pool* p, const char* str, int len);

// Search for the supplied string in the string pool. If string is missing then it will be added
extern const vm_string* vm_string_pool_string(vm_string_pool* p, const vm_string* s);

#endif
