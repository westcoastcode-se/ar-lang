#ifndef _VMC_LINKER_MARKER_H_
#define _VMC_LINKER_MARKER_H_

#include "../vm_config.h"
#include "../vm_string.h"

// Where marker value can be found
struct vmc_linker_marker_addr
{
	// Next item in the linked-list
	struct vmc_linker_marker_addr* next;

	// A unique signature
	vm_string signature;

	// Offset in the bytecode where the memory address is
	vm_uint32 offset;
};
typedef struct vmc_linker_marker_addr vmc_linker_marker_addr;

// Marker for where to inject a memory address. This marker exists for each injection
struct vmc_linker_marker_inject_addr
{
	// Next item in the linked-list
	struct vmc_linker_marker_inject_addr* next;

	// Offset in the bytecode where to inject a memory address into. 
	// 
	// Examples are where the "call" instruction is located in the bytecode
	vm_uint32 offset;

	// The address
	const vmc_linker_marker_addr* addr;
};
typedef struct vmc_linker_marker_inject_addr vmc_linker_marker_inject_addr;

// Allocate a new marker
extern vmc_linker_marker_addr* vmc_linker_marker_addr_alloc(const vm_string* signature);

// Destroy all markers
extern void vmc_linker_marker_addr_destroy(vmc_linker_marker_addr* first_marker);

// Search for a specific marker
extern vmc_linker_marker_addr* vmc_linker_marker_addr_search(vmc_linker_marker_addr* first_marker, const vm_string* name);

#endif
