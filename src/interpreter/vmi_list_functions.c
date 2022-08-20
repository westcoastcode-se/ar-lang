#include "vmi_list_functions.h"
#include "process.h"

#define CAPACITY (8)
#define RESIZE (8)

BOOL vmi_list_functions_init(vmi_list_functions* l)
{
	l->count = 0;
	l->capacity = CAPACITY;
	l->memory = (vmi_function**)malloc(sizeof(vmi_function*) * l->capacity);
	return l->memory != NULL;
}

void vmi_list_functions_release(vmi_list_functions* l)
{
	for (int i = 0; i < l->count; ++i) {
		vmi_function* const p = l->memory[i];
		vmi_function_destroy(p);
	}

	free(l->memory);
	l->memory = NULL;
	l->capacity = 0;
	l->count = 0;
}

vm_int32 vmi_list_functions_add(vmi_list_functions* l, vmi_function* ptr)
{
	if (l->count >= l->capacity) {
		l->capacity += RESIZE;
		l->memory = (vmi_function**)realloc(l->memory, sizeof(vmi_function*) * l->capacity);
		if (l->memory == NULL)
			return -1;
	}
	l->memory[l->count] = ptr;
	return l->count++;
}

vmi_function* vmi_list_functions_find(const vmi_list_functions* l, const vm_string* name)
{
	struct vmi_function** item = &l->memory[0];
	struct vmi_function** const end = &l->memory[l->count];
	for (; item != end; ++item) {
		struct vmi_function* p = *item;
		if (vm_string_cmp(&p->name, name)) {
			return p;
		}
	}
	return NULL;
}

vmi_function* vmi_list_functions_get(const vmi_list_functions* l, vm_int32 index)
{
	if (l->count <= index)
		return NULL;
	return l->memory[index];
}

vmi_function* vmi_list_functions_first(const vmi_list_functions* l)
{
	if (l->count == 0)
		return NULL;
	return l->memory[0];
}
