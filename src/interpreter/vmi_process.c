#include "vmi_process.h"
#include "vmi_thread.h"

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

BOOL vmi_process_find_package(vmi_process* p, const char* name, int len, vmi_package* result)
{
	// Metadata starts directly after the header
	int i = 0;
	const vm_byte* first_package_bytes = (p->bytecode + p->header.first_package_offset);
	const vmi_package_bytecode_header* package_header = (const vmi_package_bytecode_header*)(first_package_bytes);
	for (int i = 0; i < p->header.packages_count; ++i) {

		first_package_bytes += sizeof(vmi_package_bytecode_header) + package_header->name_length;
		package_header = (const vmi_package_bytecode_header*)(first_package_bytes);
	}

	return FALSE;
}