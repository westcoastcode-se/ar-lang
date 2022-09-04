#include "arStack.h"

void arStack_init(arStack* s)
{
	s->top = s->blocks;
	s->end = s->blocks + VM_STACK_DEFAULT_SIZE;
#ifdef VM_STACK_DEBUG
	memset(s->blocks, 0xFFFFFFFF, VM_STACK_DEFAULT_SIZE);
#endif
}

void arStack_release(arStack* s)
{
	// Do nothing for now
}

char* arStack_push(arStack* s, arInt32 size)
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

char* arStack_pop(arStack* s, arInt32 size)
{
	s->top -= size;
	// Did we pop to much memory?
	if (s->top < s->blocks) {
		s->top += size;
		return NULL;
	}
	return s->top;
}

const arByte* arStack_peek(arStack* s, arInt32 size)
{
	if (s->top - size < s->blocks) {
		return NULL;
	}
	return s->top - size;
}

BOOL arStack_verify(arStack* s, const arByte* pos)
{
	return s->top >= pos;
}

arInt32 arStack_capacity(const arStack* s)
{
	return (arInt32)(s->end - s->blocks);
}

int arStack_count(const arStack* s)
{
	return (arInt32)(s->top - s->blocks);
}
