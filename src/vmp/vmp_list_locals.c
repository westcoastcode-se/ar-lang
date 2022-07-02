#include "vmp_list_locals.h"
#include "vmp_types.h"
#include "../vmc/vmc_debug.h"

#define CAPACITY (4)
#define RESIZE (4)

BOOL vmp_list_locals_init(vmp_list_locals* l)
{
	l->count = 0;
	l->capacity = CAPACITY;
	l->memory = (struct vmp_local**)vmc_malloc(sizeof(vmp_local*) * l->capacity);
	return l->memory != NULL;
}

void vmp_list_locals_release(vmp_list_locals* l)
{
	for (int i = 0; i < l->count; ++i) {
		vmp_local* const a = l->memory[i];
		vmp_local_free(a);
	}

	vmc_free(l->memory);
	l->memory = NULL;
	l->capacity = 0;
	l->count = 0;
}

vm_int32 vmp_list_locals_add(vmp_list_locals* l, vmp_local* ptr)
{
	if (l->count >= l->capacity) {
		l->capacity += RESIZE;
		l->memory = (vmp_local**)vmc_realloc(l->memory, sizeof(vmp_local*) * l->capacity);
		if (l->memory == NULL)
			return -1;
	}
	l->memory[l->count] = ptr;
	return l->count++;
}

vmp_local* vmp_list_locals_get(const vmp_list_locals* l, vm_int32 index)
{
	if (l->count <= index)
		return NULL;
	return l->memory[index];
}
