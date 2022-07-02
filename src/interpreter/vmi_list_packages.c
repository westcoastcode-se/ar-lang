#include "vmi_list_packages.h"
#include "vmi_process.h"

#define CAPACITY (8)
#define RESIZE (8)

BOOL vmi_list_packages_init(vmi_list_packages* l)
{
	l->count = 0;
	l->capacity = CAPACITY;
	l->memory = (vmi_package**)malloc(sizeof(vmi_package*) * l->capacity);
	return l->memory != NULL;
}

void vmi_list_packages_release(vmi_list_packages* l)
{
	for (int i = 0; i < l->count; ++i) {
		vmi_package* const p = l->memory[i];
		vmi_package_destroy(p);
	}

	free(l->memory);
	l->memory = NULL;
	l->capacity = 0;
	l->count = 0;
}

vm_int32 vmi_list_packages_add(vmi_list_packages* l, vmi_package* ptr)
{
	if (l->count >= l->capacity) {
		l->capacity += RESIZE;
		l->memory = (vmi_package**)realloc(l->memory, sizeof(vmi_package*) * l->capacity);
		if (l->memory == NULL)
			return -1;
	}
	l->memory[l->count] = ptr;
	return l->count++;
}

vmi_package* vmi_list_packages_find(const vmi_list_packages* l, const vm_string* name)
{
	struct vmi_package** item = &l->memory[0];
	struct vmi_package** const end = &l->memory[l->count];
	for (; item != end; ++item) {
		struct vmi_package* p = *item;
		if (vm_string_cmp(&p->name, name)) {
			return p;
		}
	}
	return NULL;
}

vmi_package* vmi_list_packages_get(const vmi_list_packages* l, vm_int32 index)
{
	if (l->count <= index)
		return NULL;
	return l->memory[index];
}

vmi_package* vmi_list_packages_first(const vmi_list_packages* l)
{
	if (l->count == 0)
		return NULL;
	return l->memory[0];
}
