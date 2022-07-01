#ifndef _VMP_LIST_TYPES_H_
#define _VMP_LIST_TYPES_H_

#include "../vm_string.h"

// List
struct vmp_list_types
{
	vm_int32 count;
	vm_int32 capacity;
	struct vmp_type** memory;
};
typedef struct vmp_list_types vmp_list_types;

// Initialize the list
extern BOOL vmp_list_types_init(vmp_list_types* l);

// Release the internal memory of list
extern void vmp_list_types_release(vmp_list_types* l);

// Add the supplied package
extern vm_int32 vmp_list_types_add(vmp_list_types* l, struct vmp_type* ptr);

// Search for a package with the supplied name
extern struct vmp_type* vmp_list_types_find(const vmp_list_types* l, const vm_string* name);

// Get a package of a specific index
extern struct vmp_type* vmp_list_types_get(const vmp_list_types* l, vm_int32 index);

#endif
