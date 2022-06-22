#ifndef _VM_LIST_H_
#define _VM_LIST_H_

#include "vm_config.h"

// A list 
struct vm_list
{
	// The actual memory
	void** memory;
	// The number of items found in this list
	vm_int32 count;
	// The total capacity that this list is allowed to save until resizing
	vm_int32 capacity;
	// How many bytes the list will be resized with when resizing
	vm_int32 resize_count;
};
typedef struct vm_list vm_list;

// Initialize the list
extern void vm_list_init(vm_list* list);

// Initialize the list
extern void vm_list_initz(vm_list* list, vm_int32 capacity, vm_int32 resize_count);

// Destrot the internals of the supplied list
extern void vm_list_destroy(vm_list* list);

// Add a new item in the list. Returns the index of where the pointer is located
extern vm_int32 vm_list_add(vm_list* list, void* ptr);

// Get an item in the list
extern void* vm_list_get(vm_list* list, vm_int32 idx);

// Helper macro for fetching an item from the list
#define VM_LIST_GET(list, type, index) ((type*)vm_list_get(&list, index))

// Helper macro
#define VM_LIST_ADD(list, item) vm_list_add(&list, item)

#endif
