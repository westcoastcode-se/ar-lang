#include "vmc_compiler_types.h"

vmc_type_definition* vmc_type_definition_new(const vm_string* name, vm_int32 size)
{
	vmc_type_definition* type = (vmc_type_definition*)malloc(sizeof(vmc_type_definition));
	if (type == NULL)
		return type;
	VMC_INIT_TYPE_HEADER(type, VMC_TYPE_HEADER_UNKNOWN, size);
	type->name = *name;
	type->mask = VMC_TYPE_DEF_MASK_NONE;
	type->of_type = NULL;
	type->package = NULL;
	type->next = NULL;
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
	func->memory_marker_first = func->memory_marker_last = NULL;
}

vmc_func* vmc_func_malloc()
{
	vmc_func* func = (vmc_func*)malloc(sizeof(vmc_func));
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
	vmc_linker_memory_marker_destroy(func->memory_marker_first);
	func->memory_marker_first = func->memory_marker_last = NULL;
	free(func);
}

vmc_package* vmc_package_malloc(const char* name, int length)
{
	vmc_package* p = (vmc_package*)malloc(sizeof(vmc_package));
	if (p == NULL)
		return NULL;
	VMC_INIT_TYPE_HEADER(p, VMC_TYPE_HEADER_PACKAGE, sizeof(void*));
	vm_string_setsz(&p->name, name, length);
	p->full_name = p->name;
	p->func_first = p->func_last = NULL;
	p->func_count = 0;
	p->type_first = p->type_last = NULL;
	p->type_count = 0;
	p->data_offset = 0;
	p->memory_marker_first = p->memory_marker_last = NULL;
	p->root_package = NULL;
	return p;
}

void vmc_package_free(vmc_package* p)
{
	vmc_func* f;
	vmc_type_definition* t;

	// Cleanup functions
	f = p->func_first;
	while (f != NULL) {
		vmc_func* next = f->next;
		vmc_func_free(f);
		f = next;
	}

	// Cleanup types
	t = p->type_first;
	while (t != NULL) {
		vmc_type_definition* next = t->next;
		free(t);
		t = next;
	}

	// Cleanup memory markers
	vmc_linker_memory_marker_destroy(p->memory_marker_first);
	p->memory_marker_first = p->memory_marker_last = NULL;

	free(p);
}

void vmc_package_add_func(vmc_package* p, vmc_func* f)
{
	if (p->func_last == NULL) {
		p->func_first = p->func_last = f;
	}
	else {
		p->func_last->next = f;
		p->func_last = f;
	}
	p->func_count++;
}

void vmc_package_add_type(vmc_package* p, vmc_type_definition* type)
{
	type->package = p;
	if (p->type_last == NULL) {
		p->type_first = p->type_last = type;
	}
	else {
		p->type_last->next = type;
		p->type_last = type;
	}
	p->type_count++;
}

vmc_type_definition* vmc_package_find_type(vmc_package* p, const vm_string* name)
{
	vmc_type_definition* type = p->type_first;
	while (type != NULL) {
		if (vm_string_cmp(&type->name, name))
			return type;
		type = type->next;
	}
	if (p->root_package) {
		return vmc_package_find_type(p->root_package, name);
	}
	return NULL;
}

vmc_type_definition* vmc_package_find_type_with_parent(vmc_package* p, const vm_string* name, vmc_type_definition* parent)
{
	if (parent == NULL)
		return vmc_package_find_type(p, name);

	vmc_type_definition* type = p->type_first;
	while (type != NULL) {
		if (type->of_type == parent && vm_string_cmp(&type->name, name))
			return type;
		type = type->next;
	}
	if (p->root_package) {
		return vmc_package_find_type_with_parent(p->root_package, name, parent);
	}
	return NULL;
}

vmc_func* vmc_func_find(vmc_package* p, const vm_string* signature)
{
	vmc_func* func = p->func_first;
	while (func != NULL) {
		if (vm_string_cmp(&func->signature, signature)) {
			return func;
		}
		func = func->next;
	}
	return NULL;
}
