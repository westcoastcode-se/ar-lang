#include "vmp_list_globals.h"
#include "vmp_types.h"
#include "../vm_debug.h"

#define CAPACITY (4)
#define RESIZE (4)

BOOL vmp_list_globals_init(vmp_list_globals* l)
{
	l->count = 0;
	l->capacity = CAPACITY;
	l->memory = (struct vmp_global**)vm_malloc(sizeof(vmp_global*) * l->capacity);
	return l->memory != NULL;
}

void vmp_list_globals_release(vmp_list_globals* l)
{
	for (int i = 0; i < l->count; ++i) {
		vmp_global* const a = l->memory[i];
		vmp_global_free(a);
	}

	vm_free(l->memory);
	l->memory = NULL;
	l->capacity = 0;
	l->count = 0;
}

vm_int32 vmp_list_globals_add(vmp_list_globals* l, vmp_global* ptr)
{
	if (l->count >= l->capacity) {
		l->capacity += RESIZE;
		l->memory = (vmp_global**)vm_realloc(l->memory, sizeof(vmp_global*) * l->capacity);
		if (l->memory == NULL)
			return -1;
	}
	l->memory[l->count] = ptr;
	return l->count++;
}

vmp_global* vmp_list_globals_get(const vmp_list_globals* l, vm_int32 index)
{
	if (l->count <= index)
		return NULL;
	return l->memory[index];
}

vmp_global* vmp_list_globals_find(const vmp_list_globals* l, const vm_string* name)
{
	struct vmp_global** item = &l->memory[0];
	struct vmp_global** const end = &l->memory[l->count];
	for (; item != end; ++item) {
		struct vmp_global* p = *item;
		if (vm_string_cmp(&p->name, name)) {
			return p;
		}
	}
	return NULL;
}
