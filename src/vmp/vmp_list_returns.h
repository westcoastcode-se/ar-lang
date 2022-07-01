#ifndef _VMP_LIST_RETURNS_H_
#define _VMP_LIST_RETURNS_H_

#include "../vm_config.h"

// List
struct vmp_list_returns
{
	vm_int32 count;
	vm_int32 capacity;
	struct vmp_return** memory;
};
typedef struct vmp_list_returns vmp_list_returns;

// Initialize the list
extern BOOL vmp_list_returns_init(vmp_list_returns* l);

// Release the internal memory of list
extern void vmp_list_returns_release(vmp_list_returns* l);

// Add a new argument and return the index where the argument exists
extern vm_int32 vmp_list_returns_add(vmp_list_returns* l, struct vmp_return* arg);

// Get an argument using the supplied index
extern struct vmp_return* vmp_list_returns_get(const vmp_list_returns* l, vm_int32 index);

#endif
