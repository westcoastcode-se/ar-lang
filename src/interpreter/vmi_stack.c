#include "vmi_stack.h"

void vmi_stack_init(vmi_stack* s)
{
	s->top = s->blocks;
	s->end = s->blocks + VM_STACK_DEFAULT_SIZE;
#ifndef NDEBUG
	memset(s->blocks, 0xFFFFFFFF, VM_STACK_DEFAULT_SIZE);
#endif
}

void vmi_stack_release(vmi_stack* s)
{
	// Do nothing for now
}

char* vmi_stack_push(vmi_stack* s, vm_int32 size)
{
	char* top = s->top;
	if (top + size >= s->end)
		return NULL;
	s->top += size;
	return top;
}

char* vmi_stack_pop(vmi_stack* s, vm_int32 size)
{
	s->top -= size;
	// Did we pop to much memory?
	if (s->top < s->blocks) {
		s->top += size;
		return NULL;
	}
	return s->top;
}

