#ifndef _VMI_THREAD_H_
#define _VMI_THREAD_H_

#include "vmi_process.h"
#include "vmi_stack.h"

// Flags that can happen during the execution of the virtual machine
enum vmi_thread_flags
{
	VMI_THREAD_FLAG_UNKNOWN_INSTRUCTION = 1,
	VMI_THREAD_FLAG_STACK_OUT_OF_MEMORY = 2,
	VMI_THREAD_FLAG_INVALID_LOCAL = 4
};

//
// A thread is the actual type which runs the actual bytecode
struct vmi_thread
{
	// The process running this thread
	vmi_process* process;

	// Bytecode
	const vm_byte* bytecode;

	// The current position where bytecode is being processed (Instruction pointer)
	const vm_byte* ip;

	// Flags used to keep track of how this thread feels
	vm_int32 flags;

	// Blocks that can be used to access local memory. The actual location where the memory can be found
	// is on the stack. But can be, using shorthand access, used here
	vmi_stack_block* locals;

	// Stack
	vmi_stack stack;

	// Reason set when the process stops
	char exit_reason[4096];

	// Position which resulted in the thread being stopped or halted
	const vm_byte* halt_pos;

	// Next thread
	struct vmi_thread* next_thread;
};
typedef struct vmi_thread vmi_thread;

// Create a new thread
extern vmi_thread* vmi_thread_new(vmi_process* process);

// Start executing at the start of the bytecode segment
extern vm_int32 vmi_thread_exec(vmi_thread* t, const vm_byte* ip);

// Destroy the supplied thread
extern void vmi_thread_delete(vmi_thread* t);

#endif
