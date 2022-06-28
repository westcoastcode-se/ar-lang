#ifndef _VMC_LINKER_MARKER_H_
#define _VMC_LINKER_MARKER_H_

#include "../vm_config.h"

// Define what type of marker a should be injected
enum vmc_linker_marker_type
{
	// No more markers found
	VMC_LINKER_MARKER_TYPE_END = 0,
	// Inject a memory location
	VMC_LINKER_MARKER_TYPE_INJECT_ADDR = 1
};
typedef enum vmc_linker_marker_type vmc_linker_marker_type;

// Header for a memory marker. This makes it possible for the linker to know what it should be doing with
// the marker itself
struct vmc_linker_marker_header
{
	// The type of marker this is
	vmc_linker_marker_type type;
	
	// The size, in bytes, of the marker
	vm_int32 size;
};
typedef struct vmc_linker_marker_header vmc_linker_marker_header;

// Header for all linker markers
#define VMC_LINKER_MARKER_HEADER union { \
	vmc_linker_marker_header header; \
	struct { \
		vmc_linker_marker_type type; \
		vm_int32 size; \
	};  \
}

// Marker for where to inject a memory address
struct vmc_linker_marker_inject_addr
{
	VMC_LINKER_MARKER_HEADER;

	// Offset in the bytecode where to inject a memory address into. 
	// 
	// Examples are where the "call" instruction is located in the bytecode
	vm_uint32 offset;
};

// Fetch the next marker in the array of markers. If the next marker is at the end then a NULL is returned
static inline vmc_linker_marker_header* vmc_linker_marker_next(vmc_linker_marker_header* header) {
	vmc_linker_marker_header* const next_header = ((vm_byte*)header + header->size);
	return next_header->type == VMC_LINKER_MARKER_TYPE_END ? NULL : next_header;
}

#endif
