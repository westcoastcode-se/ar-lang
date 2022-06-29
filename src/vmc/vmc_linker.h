#ifndef _VMC_LINKER_H_
#define _VMC_LINKER_H_

#include "../vm_string.h"
#include "../vm_message.h"
#include "vm_bytestream.h"
#include "vmc_linker_marker.h"

// Linker that's allowed to manipulare the bytecode after the compiler is done, such as
// when resolving jumping destination 
struct vmc_linker
{
	// Messages raised by the linker
	vm_messages messages;

	// Inject address
	vmc_linker_marker_inject_addr* inject_addr_first;
	vmc_linker_marker_inject_addr* inject_addr_last;
};
typedef struct vmc_linker vmc_linker;

// Initialize the supplied linker
extern void vmc_linker_init(vmc_linker* l);

// Perform the actual linking. Returns TRUE if the process was successfull.
extern BOOL vmc_linker_process(vmc_linker* l, vm_bytestream* stream);

// Release the internal properties for the linker
extern void vmc_linker_release(vmc_linker* l);

// Check to see if the linker has compiled successfully
inline static BOOL vmc_linker_success(vmc_linker* l)
{
	return vm_messages_has_messages(&l->messages) == TRUE;
}

// Add a linker marker for where to inject a memory address to. This is normally done for instructions, such as jmp, call, etc.
//
// The offset is normally "OFFSETOF(vmi_instr_<type>, destination)", where the destination is a vm_byte*
extern vmc_linker_marker_inject_addr* vmc_linker_marker_add_inject_addr(vmc_linker* l, const vmc_linker_marker_addr* addr, vm_uint32 offset);


#endif
