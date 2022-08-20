#include "stack.h"

void vmi_stack_init(vmi_stack* s)
{
	s->top = s->blocks;
	s->end = s->blocks + VM_STACK_DEFAULT_SIZE;
#ifdef VM_STACK_DEBUG
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
#ifdef VM_STACK_DEBUG
	memset(top, 0xFFFFFFFF, (size_t)(s->top - top));
#endif
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

const vm_byte* vmi_stack_peek(vmi_stack* s, vm_int32 size)
{
	if (s->top - size < s->blocks) {
		return NULL;
	}
	return s->top - size;
}

BOOL vmi_stack_verify(vmi_stack* s, const vm_byte* pos)
{
	return s->top >= pos;
}
