#include "types_list.h"
#include "../vmc_compiler_types.h"

BOOL vmc_types_list_init(vmc_types_list* l)
{
	l->count = 0;
	l->capacity = VMC_TYPES_LIST_CAPACITY;
	l->memory = (struct vmc_type_header**)malloc(sizeof(struct vmc_type_header*) * l->capacity);
	return l->memory != NULL;
}

void vmc_types_list_destroy(vmc_types_list* l)
{
	for (int i = 0; i < l->count; ++i) {
		vmc_type_header* const header = l->memory[i];
		switch (header->type) {
		case VMC_TYPE_HEADER_IMPORT_ALIAS:
			free((vmc_import_alias*)header);
			continue;
		case VMC_TYPE_HEADER_TYPE:
			free((vmc_type_definition*)header);
			continue;
		case VMC_TYPE_HEADER_FUNC:
			free((vmc_func*)header);
			continue;
		default:
			// Unknown header type
			assert(FALSE);
		}
	}

	free(l->memory);
	l->memory = NULL;
	l->capacity = 0;
	l->count = 0;
}

vm_int32 vmc_types_list_add(vmc_types_list* l, struct vmc_type_header* ptr)
{
	if (l->count >= l->capacity) {
		l->capacity += VMC_TYPES_LIST_RESIZE;
		l->memory = (struct vmc_type_header**)realloc(l->memory, sizeof(struct vmc_type_header*) * l->capacity);
		if (l->memory == NULL)
			return -1;
	}
	ptr->id = l->count++;
	l->memory[ptr->id] = ptr;
	return l->count;
}

struct vmc_type_header* vmc_types_list_find(vmc_types_list* l, const vm_string* name)
{
	struct vmc_type_header** item = &l->memory[0];
	struct vmc_type_header** const end = &l->memory[l->count];
	for (; item != end; ++item) {
		struct vmc_type_header* header = *item;
		if (vm_string_cmp(&header->name, name)) {
			return header;
		}
	}
	return NULL;
}

struct vmc_type_header* vmc_types_list_find_recursive(vmc_types_list* l, const vm_string* name)
{
	struct vmc_type_header** item = &l->memory[0];
	struct vmc_type_header** const end = &l->memory[l->count];
	for (; item != end; ++item) {
		struct vmc_type_header* header = *item;
		if (vm_string_cmp(&header->name, name)) {
			return header;
		}
		else if (vm_string_length(&header->name) == 0 && header->type == VMC_TYPE_HEADER_IMPORT_ALIAS) {
			vmc_import_alias* alias = (vmc_import_alias*)header;
			header = vmc_types_list_find(&alias->package->types, name);
			if (header != NULL)
				return header;
		}
	}
	return NULL;
}

struct vmc_func* vmc_types_list_find_func(vmc_types_list* l, const vm_string* signature)
{
	struct vmc_type_header** item = &l->memory[0];
	struct vmc_type_header** const end = &l->memory[l->count];
	for (; item != end; ++item) {
		struct vmc_type_header* header = *item;
		if (header->type == VMC_TYPE_HEADER_FUNC) {
			struct vmc_func* func = (struct vmc_func*)header;
			if (vm_string_cmp(&func->signature, signature)) {
				return func;
			}
		}
	}
	return NULL;
}

vm_int32 vmc_types_list_count_type(vmc_types_list* l, vm_int32 type)
{
	vm_int32 count = 0;
	const struct vmc_type_header** item = &l->memory[0];
	const struct vmc_type_header** const end = &l->memory[l->count];
	for (; item != end; ++item) {
		const struct vmc_type_header* header = *item;
		if (header->type == type)
			count++;
	}
	return count;
}
