#ifndef _VMI_LOCALS_H_
#define _VMI_LOCALS_H_

#include "../vm_config.h"

// A local block containing memory for local variable data
struct vmi_locals_block
{
	vm_byte* memory;
	vm_byte* top;
	vm_byte* end;

	struct vmi_locals_block* prev;
	struct vmi_locals_block* next;
};
typedef struct vmi_locals_block vmi_locals_block;

// Local variables
struct vmi_locals
{
	vmi_locals_block* current;
	vmi_locals_block* free_block;
};
typedef struct vmi_locals vmi_locals;

// Initialize the locals container
extern BOOL vmi_locals_init(vmi_locals* l);

// Release locals memory
extern void vmi_locals_release(vmi_locals* l);

// Take the supplied number of bytes as local variables
extern vm_byte* vmi_locals_take(vmi_locals* l, vm_int32 size);

// Give back local variable memory
extern BOOL vmi_locals_give_back(vmi_locals* l, vm_int32 size);

#endif
