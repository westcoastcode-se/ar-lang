#include "vmp_list_args.h"
#include "vmp_types.h"
#include "../vm_debug.h"

#define CAPACITY (2)
#define RESIZE (4)

BOOL vmp_list_args_init(vmp_list_args* l)
{
	l->count = 0;
	l->capacity = CAPACITY;
	l->memory = (struct vmp_arg**)vm_malloc(sizeof(vmp_arg*) * l->capacity);
	return l->memory != NULL;
}

void vmp_list_args_release(vmp_list_args* l)
{
	for (int i = 0; i < l->count; ++i) {
		vmp_arg* const a = l->memory[i];
		vmp_arg_free(a);
	}

	vm_free(l->memory);
	l->memory = NULL;
	l->capacity = 0;
	l->count = 0;
}

vm_int32 vmp_list_args_add(vmp_list_args* l, vmp_arg* ptr)
{
	if (l->count >= l->capacity) {
		l->capacity += RESIZE;
		l->memory = (vmp_arg**)vm_realloc(l->memory, sizeof(vmp_arg*) * l->capacity);
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

vmp_arg* vmp_list_args_find(const vmp_list_args* l, const vm_string* name)
{
	struct vmp_arg** item = &l->memory[0];
	struct vmp_arg** const end = &l->memory[l->count];
	for (; item != end; ++item) {
		struct vmp_arg* p = *item;
		if (vm_string_cmp(&p->name, name)) {
			return p;
		}
	}
	return NULL;
}
