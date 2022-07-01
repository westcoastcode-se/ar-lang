#include "vmp_list_funcs.h"
#include "vmp_types.h"
#include "../vmc/vmc_debug.h"

#define CAPACITY (8)
#define RESIZE (8)

BOOL vmp_list_funcs_init(vmp_list_funcs* l)
{
	l->count = 0;
	l->capacity = CAPACITY;
	l->memory = (vmp_func**)vmc_malloc(sizeof(vmp_func*) * l->capacity);
	return l->memory != NULL;
}

void vmp_list_funcs_release(vmp_list_funcs* l)
{
	for (int i = 0; i < l->count; ++i) {
		vmp_func* const p = l->memory[i];
		vmp_func_destroy(p);
	}

	vmc_free(l->memory);
	l->memory = NULL;
	l->capacity = 0;
	l->count = 0;
}

vm_int32 vmp_list_funcs_add(vmp_list_funcs* l, vmp_func* ptr)
{
	if (l->count >= l->capacity) {
		l->capacity += RESIZE;
		l->memory = (vmp_func**)vmc_realloc(l->memory, sizeof(vmp_func*) * l->capacity);
		if (l->memory == NULL)
			return -1;
	}
	l->memory[l->count] = ptr;
	return l->count++;
}

vmp_func* vmp_list_funcs_find(const vmp_list_funcs* l, const vm_string* name)
{
	struct vmp_func** item = &l->memory[0];
	struct vmp_func** const end = &l->memory[l->count];
	for (; item != end; ++item) {
		struct vmp_func* p = *item;
		if (vm_string_cmp(&p->name, name)) {
			return p;
		}
	}
	return NULL;
}

vmp_func* vmp_list_funcs_get(const vmp_list_funcs* l, vm_int32 index)
{
	if (l->count <= index)
		return NULL;
	return l->memory[index];
}
