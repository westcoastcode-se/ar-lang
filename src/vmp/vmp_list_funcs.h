#ifndef _VMP_LIST_FUNCS_H_
#define _VMP_LIST_FUNCS_H_

#include "../vm_string.h"

// List
struct vmp_list_funcs
{
	vm_int32 count;
	vm_int32 capacity;
	struct vmp_func** memory;
};
typedef struct vmp_list_funcs vmp_list_funcs;

// Initialize the list
extern BOOL vmp_list_funcs_init(vmp_list_funcs* l);

// Release the internal memory of list
extern void vmp_list_funcs_release(vmp_list_funcs* l);

// Add the supplied package
extern vm_int32 vmp_list_funcs_add(vmp_list_funcs* l, struct vmp_func* ptr);

// Search for a package with the supplied name
extern struct vmp_func* vmp_list_funcs_find(const vmp_list_funcs* l, const vm_string* name);

// Get a package of a specific index
extern struct vmp_func* vmp_list_funcs_get(const vmp_list_funcs* l, vm_int32 index);

#endif
