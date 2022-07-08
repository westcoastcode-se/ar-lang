#include "vmp_list_imports.h"
#include "vmp_types.h"
#include "vmp_debug.h"

#define CAPACITY (4)
#define RESIZE (4)

BOOL vmp_list_imports_init(vmp_list_imports* l)
{
	l->count = 0;
	l->capacity = CAPACITY;
	l->memory = (vmp_package**)vmp_malloc(sizeof(vmp_package*) * l->capacity);
	return l->memory != NULL;
}

void vmp_list_imports_release(vmp_list_imports* l)
{
	vmp_free(l->memory);
	l->memory = NULL;
	l->capacity = 0;
	l->count = 0;
}

vm_int32 vmp_list_imports_add(vmp_list_imports* l, vmp_package* ptr)
{
	if (l->count >= l->capacity) {
		l->capacity += RESIZE;
		l->memory = (vmp_package**)vmp_realloc(l->memory, sizeof(vmp_package*) * l->capacity);
		if (l->memory == NULL)
			return -1;
	}
	l->memory[l->count] = ptr;
	return l->count++;
}

vmp_package* vmp_list_imports_find(const vmp_list_imports* l, const vm_string* name)
{
	struct vmp_package** item = &l->memory[0];
	struct vmp_package** const end = &l->memory[l->count];
	for (; item != end; ++item) {
		struct vmp_package* p = *item;
		if (vm_string_cmp(&p->name, name)) {
			return p;
		}
	}
	return NULL;
}

vmp_package* vmp_list_imports_get(const vmp_list_imports* l, vm_int32 index)
{
	if (l->count <= index)
		return NULL;
	return l->memory[index];
}

vmp_package* vmp_list_imports_first(const vmp_list_imports* l)
{
	if (l->count == 0)
		return NULL;
	return l->memory[0];
}
