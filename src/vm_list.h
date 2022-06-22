#ifndef _VM_LIST_H_
#define _VM_LIST_H_

#include "vm_config.h"

// A list 
struct vm_list
{
	// The actual memory
	void** memory;
	// The number of items found in this list
	int count;
	// The total capacity that this list is allowed to save until resizing
	int capacity;
	// How many bytes the list will be resized with when resizing
	int resize_count;
};
typedef struct vm_list vm_list;

// Initialize the list
extern void vm_list_init(vm_list* list);

// Initialize the list
extern void vm_list_initz(vm_list* list, int capacity, int resize_count);

// Add a new item in the list. Returns the index of where the pointer is located
extern int vm_list_add(vm_list* list, void* ptr);

// Get an item in the list
extern void* vm_list_get(vm_list* list, int idx);

#endif
