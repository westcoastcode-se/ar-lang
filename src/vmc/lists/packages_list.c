#include "packages_list.h"
#include "../vmc_debug.h"
#include "../vmc_compiler_types.h"

BOOL vmc_packages_list_init(vmc_packages_list* l)
{
	l->count = 0;
	l->capacity = VMC_PACKAGES_LIST_CAPACITY;
	l->memory = (vmc_package**)vmc_malloc(sizeof(vmc_package*) * l->capacity);
	return l->memory != NULL;
}

void vmc_packages_list_destroy(vmc_packages_list* l)
{
	for (int i = 0; i < l->count; ++i) {
		vmc_package* const p = l->memory[i];
		vmc_package_free(p);
	}

	vmc_free(l->memory);
	l->memory = NULL;
	l->capacity = 0;
	l->count = 0;
}

vm_int32 vmc_packages_list_add(vmc_packages_list* l, vmc_package* ptr)
{
	if (l->count >= l->capacity) {
		l->capacity += VMC_TYPES_LIST_RESIZE;
		l->memory = (vmc_package**)vmc_realloc(l->memory, sizeof(vmc_package*) * l->capacity);
		if (l->memory == NULL)
			return -1;
	}
	ptr->id = l->count++;
	l->memory[ptr->id] = ptr;
	return l->count;
}

vmc_package* vmc_packages_list_find(vmc_packages_list* l, const vm_string* name)
{
	struct vmc_package** item = &l->memory[0];
	struct vmc_package** const end = &l->memory[l->count];
	for (; item != end; ++item) {
		struct vmc_package* header = *item;
		if (vm_string_cmp(&header->name, name)) {
			return header;
		}
	}
	return NULL;
}

