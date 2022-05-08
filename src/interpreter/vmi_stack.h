#ifndef _VMI_STACK_H_
#define _VMI_STACK_H_

#include "../vm_config.h"

// Manages the memory on the stack
struct vmi_stack
{
	vm_byte blocks[VM_STACK_DEFAULT_SIZE];
	vm_byte* top;
	const vm_byte* end;
};
typedef struct vmi_stack vmi_stack;

// Initialize the supplied stack
extern void vmi_stack_init(vmi_stack* s);

// Release the supplied stack
extern void vmi_stack_release(vmi_stack* s);

// Move the stack pointer forward and return a pointer to where we can store the number of blocks required
// Returns NULL if there is no more free memory on the stack
extern vm_byte* vmi_stack_push(vmi_stack* s, vm_int32 size);

// Move the stack backwards. This will return a pointer to where you can read
// the poped value from. Will return NULL if you've popped to much memory
extern vm_byte* vmi_stack_pop(vmi_stack* s, vm_int32 size);

// Get the capacity of the stack, in bytes
inline static vm_int32 vmi_stack_capacity(const vmi_stack* s)
{
	return (vm_int32)(s->end - s->blocks);
}

// Count how much bytes are reserved on the stack
inline static int vmi_stack_count(const vmi_stack* s)
{
	return (vm_int32)(s->top - s->blocks);
}

#endif
