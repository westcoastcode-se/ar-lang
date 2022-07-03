#ifndef _VMP_LIST_INHERITS_FROM_H_
#define _VMP_LIST_INHERITS_FROM_H_

#include "../vm_string.h"

// List
struct vmp_list_inherits_from
{
	vm_int32 count;
	vm_int32 capacity;
	struct vmp_type** memory;
};
typedef struct vmp_list_inherits_from vmp_list_inherits_from;

// Initialize the list
extern BOOL vmp_list_inherits_from_init(vmp_list_inherits_from* l);

// Release the internal memory of list
extern void vmp_list_inherits_from_release(vmp_list_inherits_from* l);

// Add the supplied type
extern vm_int32 vmp_list_inherits_from_add(vmp_list_inherits_from* l, struct vmp_type* ptr);

// Search for a type with the supplied name
extern struct vmp_type* vmp_list_inherits_from_find(const vmp_list_inherits_from* l, const vm_string* name);

// Get a type of a specific index
extern struct vmp_type* vmp_list_inherits_from_get(const vmp_list_inherits_from* l, vm_int32 index);

#endif
