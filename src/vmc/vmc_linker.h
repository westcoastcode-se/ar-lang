#ifndef _VMC_LINKER_H_
#define _VMC_LINKER_H_

#include "../vm_string.h"
#include "vm_bytestream.h"

// A memory marker, used to remember a position in the bytecode for when you perform jumps.
struct vmc_linker_memory_marker
{
	// Marker name
	vm_string name;

	// Offset where the marker points towards inside the function
	vm_int32 offset;

	// Next marker in the list
	struct vmc_linker_memory_marker* next;
};
typedef struct vmc_linker_memory_marker vmc_linker_memory_marker;

// A marker target
struct vmc_linker_memory_marker_target
{
	// Marker which a target get its offset from
	const vmc_linker_memory_marker* marker;

	// Offset where we should inject the offset into inside the bytestream
	vm_int32 bytestream_offset;

	// Next target to set
	struct vmc_linker_memory_marker_target* next;
};
typedef struct vmc_linker_memory_marker_target vmc_linker_memory_marker_target;

// Linker that's allowed to manipulare the bytecode after the compiler is done, such as
// when resolving jumping destination 
struct vmc_linker
{
	// Byte stream
	vm_bytestream* bytestream;

	// Targets to update during the linkage process
	vmc_linker_memory_marker_target* targets_first;
	vmc_linker_memory_marker_target* targets_last;
};
typedef struct vmc_linker vmc_linker;

// Allocate memory for a new memory marker
extern vmc_linker_memory_marker* vmc_linker_memory_marker_alloc();

// Destroy the supplied marker and all it's siblings
extern void vmc_linker_memory_marker_destroy(vmc_linker_memory_marker* m);

// Search for a memory marker with a specific name
extern vmc_linker_memory_marker* vmc_linker_memory_marker_find_sibling(vmc_linker_memory_marker* m, const vm_string* name);

// Initialize the supplied linker
extern void vmc_linker_init(vmc_linker* l, vm_bytestream* stream);

// Perform the actual linking
extern void vmc_linker_process(vmc_linker* l);

// Release the internal properties for the linker
extern void vmc_linker_release(vmc_linker* l);

// Add a memory marker to be processed during the linkage step. Will return NULL if the computer is out of memory.
//
// The offset is normally "OFFSETOF(vmi_instr_<type>, destination)", where the destination is a vm_byte*
extern vmc_linker_memory_marker_target* vmc_linker_add_memory_marker(vmc_linker* l, vmc_linker_memory_marker* marker,
	vm_int32 offset);

#endif
