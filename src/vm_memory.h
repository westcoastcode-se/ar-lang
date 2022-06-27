#ifndef _VM_MEMORY_H_
#define _VM_MEMORY_H_

#include "vm_config.h"

// Unresizable memory
struct vm_unresizable_memory
{
	vm_byte* memory;
	const vm_byte* end;
};
typedef struct vm_unresizable_memory vm_unresizable_memory;

// Initialize the internal memory for the supplied memory block
extern BOOL vm_unresizable_memory_init(vm_unresizable_memory* m, vm_int32 size);

// Release the internal memory
extern void vm_unresizable_memory_release(vm_unresizable_memory* m, vm_int32 size);

// Get the memory inside the unresizable memory block
static inline vm_byte* vm_unresizable_memory_get(vm_unresizable_memory* m, vm_int32 pos)
{
	return m->memory + pos;
}

#endif

