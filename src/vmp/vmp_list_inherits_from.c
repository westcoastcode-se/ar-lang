#include "vmp_list_inherits_from.h"
#include "vmp_types.h"
#include "../vmc/vmc_debug.h"

#define CAPACITY (2)
#define RESIZE (2)

BOOL vmp_list_inherits_from_init(vmp_list_inherits_from* l)
{
	l->count = 0;
	l->capacity = CAPACITY;
	l->memory = (vmp_type**)vmc_malloc(sizeof(vmp_type*) * l->capacity);
	return l->memory != NULL;
}

void vmp_list_inherits_from_release(vmp_list_inherits_from* l)
{
	vmc_free(l->memory);
	l->memory = NULL;
	l->capacity = 0;
	l->count = 0;
}

vm_int32 vmp_list_inherits_from_add(vmp_list_inherits_from* l, vmp_type* ptr)
{
	if (l->count >= l->capacity) {
		l->capacity += RESIZE;
		l->memory = (vmp_type**)vmc_realloc(l->memory, sizeof(vmp_type*) * l->capacity);
		if (l->memory == NULL)
			return -1;
	}
	l->memory[l->count] = ptr;
	return l->count++;
}

vmp_type* vmp_list_inherits_from_find(const vmp_list_inherits_from* l, const vm_string* name)
{
	struct vmp_type** item = &l->memory[0];
	struct vmp_type** const end = &l->memory[l->count];
	for (; item != end; ++item) {
		struct vmp_type* p = *item;
		if (vm_string_cmp(&p->name, name)) {
			return p;
		}
	}
	return NULL;
}

vmp_type* vmp_list_inherits_from_get(const vmp_list_inherits_from* l, vm_int32 index)
{
	if (l->count <= index)
		return NULL;
	return l->memory[index];
}
