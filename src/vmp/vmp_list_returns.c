#include "vmp_list_returns.h"
#include "vmp_types.h"
#include "../vmc/vmc_debug.h"

#define CAPACITY (2)
#define RESIZE (4)

BOOL vmp_list_returns_init(vmp_list_returns* l)
{
	l->count = 0;
	l->capacity = CAPACITY;
	l->memory = (struct vmp_return**)vmc_malloc(sizeof(vmp_return*) * l->capacity);
	return l->memory != NULL;
}

void vmp_list_returns_release(vmp_list_returns* l)
{
	for (int i = 0; i < l->count; ++i) {
		vmp_return* const a = l->memory[i];
		vmp_return_free(a);
	}

	vmc_free(l->memory);
	l->memory = NULL;
	l->capacity = 0;
	l->count = 0;
}

vm_int32 vmp_list_returns_add(vmp_list_returns* l, vmp_return* ptr)
{
	if (l->count >= l->capacity) {
		l->capacity += RESIZE;
		l->memory = (vmp_return**)vmc_realloc(l->memory, sizeof(vmp_return*) * l->capacity);
		if (l->memory == NULL)
			return -1;
	}
	l->memory[l->count] = ptr;
	return l->count++;
}

vmp_return* vmp_list_returns_get(const vmp_list_returns* l, vm_int32 index)
{
	if (l->count <= index)
		return NULL;
	return l->memory[index];
}
