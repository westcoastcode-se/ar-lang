#ifndef _VMI_PROCESS_H_
#define _VMI_PROCESS_H_

#include "../vm_config.h"
#include "../vm_string.h"

// Metadata of the package saved in the bytecode
struct vmi_package_bytecode_header
{
	vm_int32 name_length;
	vm_int32 num_functions;
	vm_int32 num_types;
	vm_int32 type_offset;
	vm_int32 function_offset;
	// char* name
};
typedef struct vmi_package_bytecode_header vmi_package_bytecode_header;

// Package information
struct vmi_package
{
	// Unique id for this package
	vm_uint32 id;

	// Name of the package
	vm_string name;
};
typedef struct vmi_package vmi_package;

// Header that describes where the bytecode can be found for each
struct vmi_process_header
{
	// "VM0"
	char header[3];

	// Compatibility version
	char version;

	// Offset where the code starts
	vm_uint32 code_offset;
	
	// Offset where data starts, such as constant data, package- and function information
	vm_uint32 data_offset;

	// Number of packages compiled and linked with the bytecode
	vm_uint32 packages_count;

	// Offset where the first package is found
	vm_uint32 first_package_offset;
};
typedef struct vmi_process_header vmi_process_header;

// The actual virtual machine
struct vmi_process
{
	// Bytecode in raw memory
	const vm_byte* bytecode;

	// Header
	vmi_process_header header;

	// All packages (number of packages can be found in the header)
	vmi_package* packages;

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

// Search for a package with the supplied name
extern const vmi_package* vmi_process_find_package_by_name(vmi_process* p, const char* name, int len);

// Search for a package with the supplied name
extern const vmi_package* vmi_process_find_package_by_id(vmi_process* p, vm_uint32 id);

#endif
