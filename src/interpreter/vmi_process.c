#include "vmi_process.h"
#include "vmi_thread.h"

//
// PRIVATE ///////////////////////////////////////////////////////////////////////////
// 

BOOL _vmi_process_load_package_info(vmi_process* p, const vm_byte* bytecode)
{
	vm_uint32 i;
	vm_uint32 functions_start_index = 0;
	const vm_byte* first_package_bytes = (p->bytecode + p->header.first_package_offset);

	// Create an array containing information on all packages
	p->packages = (vmi_package*)malloc(sizeof(vmi_package) * p->header.packages_count);
	if (p->packages == NULL) {
		return FALSE;
	}

	// Create memory for all functions
	p->functions = (vmi_package_func*)malloc(sizeof(vmi_package_func) * p->header.functions_count);
	if (p->functions == NULL) {
		return FALSE;
	}

	// Prepare each package
	for (i = 0; i < p->header.packages_count; ++i) {
		const vmi_package_bytecode_header* header = (const vmi_package_bytecode_header*)(first_package_bytes);
		const char* const name_start = first_package_bytes + sizeof(vmi_package_bytecode_header);
		vmi_package* const current_package = &p->packages[i];
		
		current_package->id = i;
		current_package->name.start = name_start;
		current_package->name.end = name_start + header->name_length;
		current_package->functions_start_index = functions_start_index;
		current_package->functions_count = header->functions_count;
		current_package->process = p;

		first_package_bytes += sizeof(vmi_package_bytecode_header) + header->name_length;
		functions_start_index += header->functions_count;
	}

	// Prepare each function
	for (i = 0; i < p->header.functions_count; ++i) {
		const vmi_package_func_bytecode_header* header = (const vmi_package_func_bytecode_header*)(first_package_bytes);
		const char* const name_start = first_package_bytes + sizeof(vmi_package_func_bytecode_header);
		vmi_package_func* const current_func = &p->functions[i];

		current_func->id = i;
		current_func->name.start = name_start;
		current_func->name.end = name_start + header->name_length;
		current_func->ptr = bytecode + header->ptr_start;
		current_func->expected_stack_size = header->expected_stack_size;

		first_package_bytes += sizeof(vmi_package_func_bytecode_header) + header->name_length;
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
	return p;
}

void vmi_process_destroy(vmi_process* p)
{
	ASSERT_NOT_NULL(p);
	if (p->packages != NULL) {
		free(p->packages);
		p->packages = NULL;
	}
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

vm_int32 vmi_process_exec(vmi_process* p, struct vmi_thread* t, const vmi_package_func* func)
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
	vm_uint32 i = 0;
	for (i = 0; i < p->header.packages_count; ++i) {
		if (vm_string_cmpsz(&p->packages[i].name, name, len)) {
			return &p->packages[i];
		}
	}
	return NULL;
}

const vmi_package_func* vmi_package_find_function_by_name(const vmi_package* p, const char* name, int len)
{
	vm_uint32 i;
	const vm_uint32 count = p->functions_count;
	if (count == 0)
		return NULL;
	for (i = p->functions_start_index; i < count; ++i) {
		if (vm_string_cmpsz(&p->process->functions[i].name, name, len)) {
			return &p->process->functions[i];
		}
	}
	return NULL;
}

const vmi_package* vmi_process_find_package_by_id(const vmi_process* p, vm_uint32 id)
{
	if (p->header.packages_count >= id)
		return NULL;
	return &p->packages[id];
}

const vmi_package_func* vmi_process_find_function_by_id(const vmi_process* p, vm_uint32 id)
{
	if (p->header.functions_count >= id)
		return NULL;
	return &p->functions[id];
}
