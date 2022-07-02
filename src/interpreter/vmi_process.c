#include "vmi_process.h"
#include "vmi_thread.h"

//
// PRIVATE ///////////////////////////////////////////////////////////////////////////
// 

BOOL _vmi_process_load_package_info(vmi_process* p, const vm_byte* bytecode)
{
	vm_uint32 i, j;
	const vm_byte* ptr = (p->bytecode + p->header.first_package_offset);
	
	// Prepare each package
	for (i = 0; i < p->header.packages_count; ++i) {
		const vmi_package_bytecode_header* header = (const vmi_package_bytecode_header*)(ptr);
		const char* name_start = ptr + sizeof(vmi_package_bytecode_header);
		vmi_package* const current_package = vmi_package_new();

		current_package->id = vmi_list_packages_add(&p->packages, current_package);
		current_package->name.start = name_start;
		current_package->name.end = name_start + header->name_length;
		current_package->process = p;

		ptr += sizeof(vmi_package_bytecode_header) + header->name_length;

		for(j = 0; j < header->functions_count; ++j) {
			const vmi_package_func_bytecode_header* fh = (const vmi_package_func_bytecode_header*)(ptr);
			name_start = ptr + sizeof(vmi_package_bytecode_header);
			vmi_function* const current_func = vmi_function_new();
			current_func->id = p->functions_count++;
			current_func->name.start = name_start;
			current_func->name.end = name_start + fh->name_length;
			current_func->ptr = bytecode + fh->ptr_start;
			current_func->expected_stack_size = fh->expected_stack_size;

			vmi_list_functions_add(&current_package->functions, current_func);

			ptr += sizeof(vmi_package_func_bytecode_header) + fh->name_length;
		}
	}

	// Set function in the global functions list
	p->functions = malloc(sizeof(vmi_function*) * p->functions_count);
	for (i = 0; i < p->packages.count; ++i) {
		const vmi_package* package = vmi_list_packages_get(&p->packages, i);
		for (j = 0; j < package->functions.count; ++j) {
			const vmi_function* func = vmi_list_functions_get(&package->functions, j);
			p->functions[func->id] = func;
		}
	}

	return TRUE;
}

//
// PUBLIC ///////////////////////////////////////////////////////////////////////////
// 

vmi_process* vmi_process_new()
{
	vmi_process* p = (vmi_process*)malloc(sizeof(vmi_process));
	if (p == NULL)
		return NULL;
	memset(p, 0, sizeof(vmi_process));
	vmi_list_packages_init(&p->packages);
	p->functions = NULL;
	p->functions_count = 0;
	return p;
}

void vmi_process_destroy(vmi_process* p)
{
	ASSERT_NOT_NULL(p);
	vmi_list_packages_release(&p->packages);
	free(p);
}

vm_int32 vmi_process_load(vmi_process* p, const vm_byte* bytecode)
{
	// Validate the bytecode
	const vmi_process_header* header = (const vmi_process_header*)bytecode;
	if (header->header[0] != 'V' || header->header[1] != 'M' || header->header[2] != '0')
		return -1;
	if (header->version != VM_VERSION)
		return -1;

	// Set values
	p->bytecode = bytecode;
	p->header = *header;

	// Load package information
	if (!_vmi_process_load_package_info(p, bytecode)) {
		return -1;
	}
	return 0;
}

vm_int32 vmi_process_exec(vmi_process* p, struct vmi_thread* t, const vmi_function* func)
{
	vm_int32 result;
	// TODO: Add support for multiple threads
	// TODO: Add support for routines/light-weight threads
	p->current_thread = t;
	result = vmi_thread_entrypoint(t, func);
	p->current_thread = NULL;
	return result;
}

const vmi_package* vmi_process_find_package_by_name(const vmi_process* p, const char* name, int len)
{
	const vm_uint32 num_packages = p->packages.count;
	vm_uint32 i = 0;
	for (i = 0; i < num_packages; ++i) {
		const vmi_package* const package = vmi_list_packages_get(&p->packages, i);
		if (vm_string_cmpsz(&package->name, name, len)) {
			return package;
		}
	}
	return NULL;
}

const vmi_function* vmi_package_find_function_by_name(const vmi_package* p, const char* name, int len)
{
	vm_uint32 i;
	const vm_uint32 count = p->functions.count;
	for (i = 0; i < count; ++i) {
		const vmi_function* const func = vmi_list_functions_get(&p->functions, i);
		if (vm_string_cmpsz(&func->name, name, len)) {
			return func;
		}
	}
	return NULL;
}

const vmi_package* vmi_process_find_package_by_id(const vmi_process* p, vm_uint32 id)
{
	return vmi_list_packages_get(&p->packages, id);
}

const vmi_function* vmi_process_find_function_by_id(const vmi_process* p, vm_uint32 id)
{
	if (p->functions_count >= id)
		return NULL;
	return p->functions[id];
}

extern vmi_package* vmi_package_new()
{
	vmi_package* p = malloc(sizeof(vmi_package));
	if (p == NULL)
		return NULL;
	vmi_list_functions_init(&p->functions);
	return p;
}

extern void vmi_package_destroy(vmi_package* p)
{
	vmi_list_functions_release(&p->functions);
	free(p);
}

extern vmi_function* vmi_function_new()
{
	vmi_function* f = malloc(sizeof(vmi_function));
	if (f == NULL)
		return NULL;
	return f;
}

void vmi_function_destroy(vmi_function* f)
{
	free(f);	
}
