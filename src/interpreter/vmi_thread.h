#ifndef _VMI_THREAD_H_
#define _VMI_THREAD_H_

#include "vmi_process.h"
#include "vmi_stack.h"

// Flags that can happen during the execution of the virtual machine
enum vmi_thread_flags
{
	VMI_THREAD_FLAG_UNKNOWN_INSTRUCTION = 1,
	VMI_THREAD_FLAG_STACK_OUT_OF_MEMORY = 2,
	VMI_THREAD_FLAG_INVALID_LOCAL = 4,
	VMI_THREAD_FLAG_STACK_MISMANAGED = 8,
	VMI_THREAD_FLAG_NOT_IMPLEMENTED = 16,
	VMI_THREAD_FLAG_MANUAL_HALT = 32
};

// A block of memory stored on the stack. Every block is guaranteed to allow 8 bytes of memory
struct vmi_thread_vars_block
{
	union
	{
		vm_int64 i64;
		vm_float64 f64;
		vm_float32 f32;
		void* value;
		struct
		{
			vm_int8 i8;
			vm_int8 i8_1;
			vm_int8 i8_2;
			vm_int8 i8_3;
			vm_int8 i8_4;
			vm_int8 i8_5;
			vm_int8 i8_6;
			vm_int8 i8_7;
		};

		struct
		{
			vm_int16 i16;
			vm_int16 i16_1;
			vm_int16 i16_2;
			vm_int16 i16_3;
		};

		struct
		{
			vm_int32 i32;
			vm_int32 i32_1;
		};
	};
};
typedef struct vmi_thread_vars_block vmi_thread_vars_block;

// Call frame
struct vmi_thread_call_frame
{
	// Where argument memory start
	vm_byte* stack_arguments;

	// Stack position where return values are located
	vm_byte* stack_returns;

	// Blocks that can be used to access local memory. The actual location where the memory can be found
	// is on the stack. But can be, using shorthand access, used here
	vmi_thread_vars_block* locals;
};
typedef struct vmi_thread_call_frame vmi_thread_call_frame;

//
// A thread is the actual type which runs the actual bytecode
struct vmi_thread
{
	// The process running this thread
	vmi_process* process;

	// Bytecode
	const vm_byte* bytecode;

	// The current position where bytecode is being processed (Instruction pointer)
	vmi_ip ip;

	// Flags used to keep track of how this thread feels
	vm_int32 flags;

	// Pointer which can be used to refer to the position on the stack when the current function was called
	vm_byte* ebp;

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

// Reserve the supplied amount of memory on the stack
extern vm_int32 vmi_thread_reserve_stack(vmi_thread* t, vm_int32 size);

// Push a 32 bit integer to the stack of the supplied thread. Returns non-zero if an error has occurred
extern vm_int32 vmi_thread_push_i16(vmi_thread* t, vm_int16 value);

// Push a 32 bit integer to the stack of the supplied thread. Returns non-zero if an error has occurred
extern vm_int32 vmi_thread_push_i32(vmi_thread* t, vm_int32 value);

// Push a memory address to the stack
extern vm_int32 vmi_thread_push_ptr(vmi_thread* t, void* value);

// Start executing at the start of the supplied bytecode segment. It is assumed that any arguments are already pushed
// to the stack. If the execution is successful then the return code should be 0
extern vm_int32 vmi_thread_exec(vmi_thread* t, vmi_ip ip);

// Destroy the supplied thread
extern void vmi_thread_destroy(vmi_thread* t);

// Manually halt the supplied thread
extern void vmi_thread_halt(vmi_thread* t, const char* message);

// Manually halt the supplied thread with a flag
extern void vmi_thread_halti(vmi_thread* t, vm_int32 flags, const char* message);

// Manually halt the supplied thread with a flag
extern void vmi_thread_shalti(vmi_thread* t, vm_int32 flags, const char* format, ...);

#endif
