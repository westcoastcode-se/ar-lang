#include "vmp_list_packages.h"
#include "vmp_types.h"
#include "../vmc/vmc_debug.h"

#define CAPACITY (8)
#define RESIZE (8)

BOOL vmp_list_packages_init(vmp_list_packages* l)
{
	l->count = 0;
	l->capacity = CAPACITY;
	l->memory = (vmp_package**)vmc_malloc(sizeof(vmp_package*) * l->capacity);
	return l->memory != NULL;
}

void vmp_list_packages_release(vmp_list_packages* l)
{
	for (int i = 0; i < l->count; ++i) {
		vmp_package* const p = l->memory[i];
		vmp_package_destroy(p);
	}

	vmc_free(l->memory);
	l->memory = NULL;
	l->capacity = 0;
	l->count = 0;
}

vm_int32 vmp_list_packages_add(vmp_list_packages* l, vmp_package* ptr)
{
	if (l->count >= l->capacity) {
		l->capacity += RESIZE;
		l->memory = (vmp_package**)vmc_realloc(l->memory, sizeof(vmp_package*) * l->capacity);
		if (l->memory == NULL)
			return -1;
	}
	l->memory[l->count] = ptr;
	return l->count++;
}

vmp_package* vmp_list_packages_find(const vmp_list_packages* l, const vm_string* name)
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

vmp_package* vmp_list_packages_get(const vmp_list_packages* l, vm_int32 index)
{
	if (l->count <= index)
		return NULL;
	return l->memory[index];
}

vmp_package* vmp_list_packages_first(const vmp_list_packages* l)
{
	if (l->count == 0)
		return NULL;
	return l->memory[0];
}
