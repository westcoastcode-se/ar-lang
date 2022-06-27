#include "vm_memory.h"
#include <stdlib.h>

BOOL vm_unresizable_memory_init(vm_unresizable_memory* m, vm_int32 size)
{
	m->memory = malloc(size);
	m->end = m->memory + size;
	return m->memory != NULL;
}

void vm_unresizable_memory_release(vm_unresizable_memory* m, vm_int32 size)
{
	free(m->memory);
	m->memory = NULL;
	m->end = NULL;
}

