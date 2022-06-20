#include "vmc_compiler_types.h"

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
	p->next = NULL;
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
