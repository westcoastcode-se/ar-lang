#ifndef _VMP_LIST_ARGS_H_
#define _VMP_LIST_ARGS_H_

#include "../vm_config.h"

// List
struct vmp_list_args
{
	vm_int32 count;
	vm_int32 capacity;
	struct vmp_arg** memory;
};
typedef struct vmp_list_args vmp_list_args;

// Initialize the list
extern BOOL vmp_list_args_init(vmp_list_args* l);

// Release the internal memory of list
extern void vmp_list_args_release(vmp_list_args* l);

// Add a new argument and return the index where the argument exists
extern vm_int32 vmp_list_args_add(vmp_list_args* l, struct vmp_arg* arg);

// Get an argument using the supplied index
extern struct vmp_arg* vmp_list_args_get(const vmp_list_args* l, vm_int32 index);

#endif
