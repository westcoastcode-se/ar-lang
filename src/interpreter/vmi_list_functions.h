#ifndef _VMI_LIST_FUNCTIONS_H_
#define _VMI_LIST_FUNCTIONS_H_

#include "debug.h"
#include "../vm_string.h"

// List
struct vmi_list_functions
{
	vm_int32 count;
	vm_int32 capacity;
	struct vmi_function** memory;
};
typedef struct vmi_list_functions vmi_list_functions;

// Initialize the list
extern BOOL vmi_list_functions_init(vmi_list_functions* l);

// Release the internal memory of list
extern void vmi_list_functions_release(vmi_list_functions* l);

// Add the supplied function
extern vm_int32 vmi_list_functions_add(vmi_list_functions* l, struct vmi_function* ptr);

// Search for a function with the supplied name
extern struct vmi_function* vmi_list_functions_find(const vmi_list_functions* l, const vm_string* name);

// Get a function of a specific index
extern struct vmi_function* vmi_list_functions_get(const vmi_list_functions* l, vm_int32 index);

// Get the first package of a specific index
extern struct vmi_function* vmi_list_functions_first(const vmi_list_functions* l);


#endif
