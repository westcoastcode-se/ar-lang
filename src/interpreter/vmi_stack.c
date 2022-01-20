#include "vmi_stack.h"

void vmi_stack_init(vmi_stack* s)
{
	s->top = s->blocks;
	s->end = s->blocks + VMI_STACK_NUM_BLOCKS;
#ifndef NDEBUG
	memset(s->blocks, 0xFFFFFFFF, VMI_STACK_NUM_BLOCKS * sizeof(vmi_stack_block));
#endif
}

void vmi_stack_release(vmi_stack* s)
{
	// Do nothing for now
}

vmi_stack_block* vmi_stack_push(vmi_stack* s, vm_int32 blocks)
{
	vmi_stack_block* top = s->top;
	if (top + blocks >= s->end)
		return NULL;
	s->top += blocks;
	return top;
}

vmi_stack_block* vmi_stack_pop(vmi_stack* s, vm_int32 blocks)
{
	s->top -= blocks;
	// Did we pop to much memory?
	if (s->top < s->blocks) {
		s->top += blocks;
		return NULL;
	}
	return s->top;
}

