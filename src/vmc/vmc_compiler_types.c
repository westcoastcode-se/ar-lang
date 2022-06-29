#include "vmc_compiler_types.h"
#include "vmc_debug.h"

vmc_type_definition* vmc_type_definition_new(const vm_string* name, vm_int32 size)
{
	vmc_type_definition* type = (vmc_type_definition*)vmc_malloc(sizeof(vmc_type_definition));
	if (type == NULL)
		return type;
	VMC_INIT_TYPE_HEADER(type, VMC_TYPE_HEADER_TYPE, size);
	type->name = *name;
	type->mask = VMC_TYPE_DEF_MASK_NONE;
	type->of_type = NULL;
	type->package = NULL;
	return type;
}

vmc_type_definition* vmc_type_definition_of_type(const vm_string* name, vmc_type_definition* parent)
{
	vmc_type_definition* def = vmc_type_definition_new(name, parent->size);
	if (def == NULL)
		return NULL;
	def->of_type = parent;
	return def;
}

BOOL vmc_type_definition_compare(const vmc_type_definition* from, const vmc_type_definition* to, vmc_type_definition_castinfo* result)
{
	if (from->of_type == NULL)
		return FALSE;
	if (from == to) {
		result->offset = 0;
		return TRUE;
	}
	while ((from = from->of_type) != NULL)
	{
		if (from == to) {
			// figure out the offset
			return TRUE;
		}
	}
	return FALSE;
}

void vmc_func_init(vmc_func* func)
{
	VMC_INIT_TYPE_HEADER(func, VMC_TYPE_HEADER_FUNC, sizeof(void*));
	func->package = NULL;
	vm_string_zero(&func->name);
	func->offset = -1;
	func->next = NULL;
	func->modifiers = 0;
	func->complexity = 0;
	func->complexity_components = 0;
	func->args_count = 0;
	func->args_total_size = 0;
	func->returns_count = 0;
	func->returns_total_size = 0;
	func->locals_count = 0;
	func->locals_total_size = 0;
	func->marker_local_addr_first = func->marker_local_addr_last = NULL;
}

vmc_func* vmc_func_malloc()
{
	vmc_func* func = (vmc_func*)vmc_malloc(sizeof(vmc_func));
	if (func == NULL)
		return NULL;
	vmc_func_init(func);
	return func;
}

void vmc_func_calculate_offsets(vmc_func* func)
{
	// The offset represents where a specific argument can be found. The order is backwards, because 
	// you push values from the right to the left (i.e. memory for the last return value is pushed first)
	vm_int32 offset = func->args_total_size + func->returns_total_size;
	vm_int32 i;
	if (func->args_count > 0) {
		for (i = 0; i < func->args_count; ++i) {
			vmc_var* info = &func->args[i];
			offset -= info->definition->size;
			info->offset = offset;
		}
	}
	if (func->returns_count > 0) {
		for (i = 0; i < func->returns_count; ++i) {
			vmc_var* info = &func->returns[i];
			offset -= info->definition->size;
			info->offset = offset;
		}
	}
}

// Destroy memory allocated for the supplied function
void vmc_func_free(vmc_func* func)
{
	vmc_linker_marker_addr_destroy(func->marker_local_addr_first);
	func->marker_local_addr_first = func->marker_local_addr_last = NULL;

	vmc_free(func);
}

vmc_package* vmc_package_malloc(const char* name, int length)
{
	vmc_package* p = (vmc_package*)vmc_malloc(sizeof(vmc_package));
	if (p == NULL)
		return NULL;
	VMC_INIT_TYPE_HEADER(p, VMC_TYPE_HEADER_PACKAGE, sizeof(void*));
	vm_string_setsz(&p->name, name, length);
	p->full_name = p->name;
	vmc_types_list_init(&p->types);
	p->data_offset = 0;
	p->marker_func_addr_first = p->marker_func_addr_last = NULL;
	p->global_package = NULL;
	return p;
}

void vmc_package_free(vmc_package* p)
{
	vmc_types_list_destroy(&p->types);

	// Cleanup memory markers
	vmc_linker_marker_addr_destroy(p->marker_func_addr_first);
	p->marker_func_addr_first = p->marker_func_addr_last = NULL;

	vmc_free(p);
}

void vmc_package_add_func(vmc_package* p, vmc_func* f)
{
	f->package = p;
	vmc_types_list_add(&p->types, TO_TYPE_HEADER(f));
}

void vmc_package_add_type(vmc_package* p, vmc_type_definition* type)
{
	type->package = p;
	vmc_types_list_add(&p->types, TO_TYPE_HEADER(type));
}

void vmc_package_add_import_alias(vmc_package* p, vmc_package* package, const vm_string* _alias)
{
	vmc_import_alias* alias = (vmc_import_alias*)vmc_malloc(sizeof(vmc_import_alias));
	VMC_INIT_TYPE_HEADER(alias, VMC_TYPE_HEADER_IMPORT_ALIAS, 0);
	alias->name = *_alias;
	alias->package = package;
	vmc_types_list_add(&p->types, TO_TYPE_HEADER(alias));
}

vmc_type_header* vmc_package_find(vmc_package* p, const vm_string* name)
{
	vmc_type_header* header = vmc_types_list_find(&p->types, name);
	if (header == NULL && p->global_package != NULL) {
		header = vmc_package_find(p->global_package, name);
	}
	return header;
}

vmc_func* vmc_func_find(vmc_package* p, const vm_string* signature)
{
	return vmc_types_list_find_func(&p->types, signature);
}
