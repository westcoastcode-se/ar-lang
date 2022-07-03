#ifndef _VMP_LIST_INHERITED_BY_H_
#define _VMP_LIST_INHERITED_BY_H_

#include "../vm_string.h"

// List
struct vmp_list_inherited_by
{
	vm_int32 count;
	vm_int32 capacity;
	struct vmp_type** memory;
};
typedef struct vmp_list_inherited_by vmp_list_inherited_by;

// Initialize the list
extern BOOL vmp_list_inherited_by_init(vmp_list_inherited_by* l);

// Release the internal memory of list
extern void vmp_list_inherited_by_release(vmp_list_inherited_by* l);

// Add the supplied type
extern vm_int32 vmp_list_inherited_by_add(vmp_list_inherited_by* l, struct vmp_type* ptr);

// Search for a type with the supplied name
extern struct vmp_type* vmp_list_inherited_by_find(const vmp_list_inherited_by* l, const vm_string* name);

// Get a type of a specific index
extern struct vmp_type* vmp_list_inherited_by_get(const vmp_list_inherited_by* l, vm_int32 index);

#endif
