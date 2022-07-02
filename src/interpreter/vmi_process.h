#ifndef _VMI_PROCESS_H_
#define _VMI_PROCESS_H_

#include "vmi_config.h"
#include "vmi_list_functions.h"
#include "vmi_list_packages.h"
#include "../vm_string.h"

struct vmi_package_func_bytecode_header
{
	// Length of the name
	vm_uint32 name_length;

	// Offset where the function starts (in relation to the start of the bytecode)
	vm_uint32 ptr_start;

	// How many bytes are expected to be pushed for this function to work
	vm_int32 expected_stack_size;

	// char name[]
};
typedef struct vmi_package_func_bytecode_header vmi_package_func_bytecode_header;

// Metadata of the package saved in the bytecode
struct vmi_package_bytecode_header
{
	vm_uint32 name_length;
	vm_uint32 functions_count;
	vm_uint32 types_count;
	// char name[]
	// functions[]
};
typedef struct vmi_package_bytecode_header vmi_package_bytecode_header;

// Information of a function inside a package
struct vmi_function
{
	// Unique id for this function
	vm_uint32 id;

	// Name of the function
	vm_string name;

	// Pointer to where the function is located
	vmi_ip ptr;

	// How many bytes are expected to be pushed for this function to work
	vm_int32 expected_stack_size;
};
typedef struct vmi_function vmi_function;

// Package information
struct vmi_package
{
	// Unique id for this package
	vm_uint32 id;

	// Name of the package
	vm_string name;

	// All functions part of this package
	vmi_list_functions functions;

	// Process this package is part of
	struct vmi_process* process;
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

	// All packages
	vmi_list_packages packages;

	// All functions part of this list
	const vmi_function** functions;
	vm_uint32 functions_count;

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
extern vm_int32 vmi_process_exec(vmi_process* p, struct vmi_thread* t, const vmi_function* func);

// Search for a package with the supplied name
extern const vmi_package* vmi_process_find_package_by_name(const vmi_process* p, const char* name, int len);

// Search for a function with the supplied name
extern const vmi_function* vmi_package_find_function_by_name(const vmi_package* p, const char* name, int len);

// Search for a package with the supplied unique id
extern const vmi_package* vmi_process_find_package_by_id(const vmi_process* p, vm_uint32 id);

// Search for a function with the supplied unique id
extern const vmi_function* vmi_process_find_function_by_id(const vmi_process* p, vm_uint32 id);

// Create a new package
extern vmi_package* vmi_package_new();

// Destroy the supplied package
extern void vmi_package_destroy(vmi_package* p);

extern vmi_function* vmi_function_new();

extern void vmi_function_destroy(vmi_function* f);

#endif
