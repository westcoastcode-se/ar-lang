#ifndef _TYPES_LIST_H_
#define _TYPES_LIST_H_

#include "../../vm_string.h"

#define VMC_TYPES_LIST_CAPACITY (8)
#define VMC_TYPES_LIST_RESIZE (8)

// List containing import statements
struct vmc_types_list
{
	struct vmc_type_header** memory;
	vm_int32 count;
	vm_int32 capacity;
};
typedef struct vmc_types_list vmc_types_list;

// Initialize list
extern BOOL vmc_types_list_init(vmc_types_list* l);

// Destroy the internal properties of the supplied list
extern void vmc_types_list_destroy(vmc_types_list* l);

// Add the supplied package to be allwoed to be imported
extern vm_int32 vmc_types_list_add(vmc_types_list* l, struct vmc_type_header* ptr);

// Search for a type with the supplied import name
extern struct vmc_type_header* vmc_types_list_find(vmc_types_list* l, const vm_string* name);

// Search for a function with the supplied signature
extern struct vmc_func* vmc_types_list_find_func(vmc_types_list* l, const vm_string* signature);

// Add the supplied package to be allwoed to be imported
extern vm_int32 vmc_types_list_count_type(vmc_types_list* l, vm_int32 type);

#endif
