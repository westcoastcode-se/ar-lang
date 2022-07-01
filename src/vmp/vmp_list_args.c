#include "vmp_list_args.h"
#include "vmp_types.h"
#include "../vmc/vmc_debug.h"

#define CAPACITY (2)
#define RESIZE (4)

BOOL vmp_list_args_init(vmp_list_args* l)
{
	l->count = 0;
	l->capacity = CAPACITY;
	l->memory = (struct vmp_arg**)vmc_malloc(sizeof(vmp_arg*) * l->capacity);
	return l->memory != NULL;
}

void vmp_list_args_release(vmp_list_args* l)
{
	for (int i = 0; i < l->count; ++i) {
		vmp_arg* const a = l->memory[i];
		vmp_arg_free(a);
	}

	vmc_free(l->memory);
	l->memory = NULL;
	l->capacity = 0;
	l->count = 0;
}

vm_int32 vmp_list_args_add(vmp_list_args* l, vmp_arg* ptr)
{
	if (l->count >= l->capacity) {
		l->capacity += RESIZE;
		l->memory = (vmp_arg**)vmc_realloc(l->memory, sizeof(vmp_arg*) * l->capacity);
		if (l->memory == NULL)
			return -1;
	}
	l->memory[l->count] = ptr;
	return l->count++;
}

vmp_arg* vmp_list_args_get(const vmp_list_args* l, vm_int32 index)
{
	if (l->count <= index)
		return NULL;
	return l->memory[index];
}
