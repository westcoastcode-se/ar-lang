#ifndef _VMP_LIST_LOCALS_H_
#define _VMP_LIST_LOCALS_H_

#include "../vm_config.h"

// List
struct vmp_list_locals
{
	vm_int32 count;
	vm_int32 capacity;
	struct vmp_local** memory;
};
typedef struct vmp_list_locals vmp_list_locals;

// Initialize the list
extern BOOL vmp_list_locals_init(vmp_list_locals* l);

// Release the internal memory of list
extern void vmp_list_locals_release(vmp_list_locals* l);

// Add a new local and return the index where the local exists
extern vm_int32 vmp_list_locals_add(vmp_list_locals* l, struct vmp_local* local);

// Get an local using the supplied index
extern struct vmp_local* vmp_list_locals_get(const vmp_list_locals* l, vm_int32 index);

#endif
