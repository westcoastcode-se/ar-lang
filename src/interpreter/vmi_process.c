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

void vmi_process_delete(vmi_process* p)
{
	ASSERT_NOT_NULL(p);
	free(p);
}

vm_int32 vmi_process_load(vmi_process* p, const vm_byte* bytecode)
{
	// Validate the bytecode
	const vmi_process_header* header = (const vmi_process_header*)bytecode;
	if (header->header[0] != 'S' || header->header[1] != 'C' || header->header[2] != 'R')
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
	p->current_thread = t;
	result = vmi_thread_exec(t, p->bytecode + p->header.code_offset);
	p->current_thread = NULL;
	return result;
}