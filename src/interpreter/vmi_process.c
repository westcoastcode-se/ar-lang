#include "vmi_process.h"
#include "vmi_thread.h"

//
// PRIVATE ///////////////////////////////////////////////////////////////////////////
// 

BOOL _vmi_process_load_package_info(vmi_process* p, const vm_byte* bytecode)
{
	vm_uint32 i;
	const vm_byte* first_package_bytes = (p->bytecode + p->header.first_package_offset);
	const vmi_package_bytecode_header* package_header = (const vmi_package_bytecode_header*)(first_package_bytes);

	// Create an array containing information on all packages
	p->packages = (vmi_package*)malloc(sizeof(vmi_package) * p->header.packages_count);
	if (p->packages == NULL) {
		return FALSE;
	}

	// Prepare each package
	for (i = 0; i < p->header.packages_count; ++i) {
		const char* const name_start = first_package_bytes + sizeof(vmi_package_bytecode_header);
		vmi_package* const current_package = &p->packages[i];

		current_package->id = i;
		current_package->name.start = name_start;
		current_package->name.end = name_start + package_header->name_length;

		first_package_bytes += sizeof(vmi_package_bytecode_header) + package_header->name_length;
		package_header = (const vmi_package_bytecode_header*)(first_package_bytes);
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

vm_int32 vmi_process_exec(vmi_process* p, struct vmi_thread* t)
{
	vm_int32 result;
	// TODO: Add support for multiple threads
	// TODO: Add support for routines/light-weight threads
	p->current_thread = t;
	result = vmi_thread_exec(t, p->bytecode + p->header.code_offset);
	p->current_thread = NULL;
	return result;
}

const vmi_package* vmi_process_find_package_by_name(vmi_process* p, const char* name, int len)
{
	vm_uint32 i = 0;
	for (i = 0; i < p->header.packages_count; ++i) {
		if (vm_string_cmpsz(&p->packages[i].name, name, len)) {
			return &p->packages[i];
		}
	}
	return NULL;
}

extern const vmi_package* vmi_process_find_package_by_id(vmi_process* p, vm_uint32 id)
{
	if (p->header.packages_count >= id)
		return NULL;
	return &p->packages[id];
}
