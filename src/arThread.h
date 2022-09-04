#ifndef _ARLANG_THREAD_H_
#define _ARLANG_THREAD_H_

#include "arProcess.h"
#include "arStack.h"
#include "arLocals.h"

// Flags that can happen during the execution of the virtual machine
typedef enum arThread_flags
{
	arThread_FLAG_UNKNOWN_INSTRUCTION = 1,
	arThread_FLAG_OUT_OF_MEMORY = 2,
	arThread_FLAG_INVALID_LOCAL = 4,
	arThread_FLAG_STACK_MISMANAGED = 8,
	arThread_FLAG_LOCALS_MISMANAGED = 16,
	arThread_FLAG_NOT_IMPLEMENTED = 32,
	arThread_FLAG_MANUAL_HALT = 64
} arThread_flags;

// Call frame
typedef struct arThreadCallFrame
{
	// Where arguments can be found
	arByte* ebp;

	// Where local variables can be found
	arByte* locals_ptr;

	// The next instruction to be called when returning
	arIP ret;
} arThreadCallFrame;

//
// A thread is the actual type which runs the actual bytecode
typedef struct arThread
{
	// The process running this thread
	arProcess* process;

	// Bytecode
	arIP bytecode;

	// The current position where bytecode is being processed (Instruction pointer)
	arIP ip;

	// Flags used to keep track of how this thread feels
	arInt32 flags;

	// Available callframes
	arThreadCallFrame call_frames[VM_RECURSIVE_FN_CALLFRAMES];

	// The current call frame
	arThreadCallFrame* cf_pos;

	// Represents the current call frame in an easier manner
	union {
		arThreadCallFrame cf;
		struct {
			arByte* ebp;
			arByte* locals_ptr;
			arIP ret;
		};
	};

	// Stack
	arStack stack;

	// Container for local variables
	arLocals locals;

	// Reason set when the process stops
	char exit_reason[4096];

	// Position which resulted in the thread being stopped or halted
	const arByte* halt_pos;

	// Next thread
	struct arThread* next_thread;
} arThread;

// Create a new thread
ARLANG_API arThread* arThread_new(arProcess* process);

// Get an argument with the supplied offset
ARLANG_API arByte* arThread_getArg(arThread* t, arInt32 offset);

// Reserve the supplied amount of memory on the stack and return a pointer to where you're
// allowed to put the data into. Will return NULL if the stack is out of memory
ARLANG_API arByte* arThread_pushStack(arThread* t, arInt32 size);

// Pop a value from the stack and return a pointer to where you can read the data from.
ARLANG_API arByte* arThread_popStack(arThread* t, arInt32 size);

// Start executing at the start of the entrypoint of the supplied function. It is assumed that any arguments are already pushed
// to the stack. 
// 
// If the execution is successful then the return code should be 0
ARLANG_API arInt32 arThread_entrypoint(arThread* t, const arFunction* func);

// Start executing a function. This function is normally called if you call a script function from a c function when the
// thread is already running. It is assumed that any arguments are already pushed to the stack. 
//
// If the execution is successful then the return code should be 0
ARLANG_API arInt32 arThread_exec(arThread* t, const arFunction* func);

// Prepare a function call
ARLANG_API arIP arThread_beginCall(arThread* t, arIP current_ip, arIP next_ip, arInt32 expected_stack_size);

// Destroy the supplied thread
ARLANG_API void arThread_destroy(arThread* t);

// Manually halt the supplied thread
ARLANG_API void arThread_halt(arThread* t, arInt32 flags, const char* message);

// Manually halt the supplied thread with a flag
ARLANG_API void arThread_haltf(arThread* t, arInt32 flags, const char* format, ...);

#endif
