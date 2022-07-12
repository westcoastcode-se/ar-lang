#include "vmp_list_types.h"
#include "vmp_types.h"
#include "../vm_debug.h"

#define CAPACITY (8)
#define RESIZE (8)

BOOL vmp_list_types_init(vmp_list_types* l)
{
	l->count = 0;
	l->capacity = CAPACITY;
	l->memory = (vmp_type**)vm_malloc(sizeof(vmp_type*) * l->capacity);
	return l->memory != NULL;
}

void vmp_list_types_release(vmp_list_types* l)
{
	for (int i = 0; i < l->count; ++i) {
		vmp_type* const p = l->memory[i];
		vmp_type_destroy(p);
	}

	vm_free(l->memory);
	l->memory = NULL;
	l->capacity = 0;
	l->count = 0;
}

vm_int32 vmp_list_types_add(vmp_list_types* l, vmp_type* ptr)
{
	if (l->count >= l->capacity) {
		l->capacity += RESIZE;
		l->memory = (vmp_type**)vm_realloc(l->memory, sizeof(vmp_type*) * l->capacity);
		if (l->memory == NULL)
			return -1;
	}
	l->memory[l->count] = ptr;
	return l->count++;
}

vmp_type* vmp_list_types_find(const vmp_list_types* l, const vm_string* name)
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

vmp_type* vmp_list_types_get(const vmp_list_types* l, vm_int32 index)
{
	if (l->count <= index)
		return NULL;
	return l->memory[index];
}
