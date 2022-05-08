#ifndef _VMI_PROCESS_H_
#define _VMI_PROCESS_H_

#include "../vm_config.h"

// Header that describes where the bytecode can be found for each
struct vmi_process_header
{
	// "VM0"
	char header[3];

	// Compatibility version
	char version;

	// Offset where data starts
	vm_uint32 data_offset;

	// Offset where the code starts
	vm_uint32 code_offset;
};
typedef struct vmi_process_header vmi_process_header;

// The actual virtual machine
struct vmi_process
{
	// Bytecode in raw memory
	const vm_byte* bytecode;

	// Header
	vmi_process_header header;

	// The first thread managed by this process
	struct vmi_thread* first_thread;

	// Current thread that we are executing
	struct vmi_thread* current_thread;
};
typedef struct vmi_process vmi_process;

// Create a new process
extern vmi_process* vmi_process_new();

// Delete the supplied process
extern void vmi_process_destroy(vmi_process* p);

// Load process
extern vm_int32 vmi_process_load(vmi_process* p, const vm_byte* bytecode);

// Execute the supplied thread
extern vm_int32 vmi_process_exec(vmi_process* p, struct vmi_thread* t);

#endif
