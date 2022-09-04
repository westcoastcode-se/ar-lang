#ifndef _VMI_STACK_H_
#define _VMI_STACK_H_

#include "arconf.h"

// Manages the memory on the stack
typedef struct arStack
{
	arByte blocks[VM_STACK_DEFAULT_SIZE];
	arByte* top;
	const arByte* end;
} arStack;

// Initialize the supplied stack
ARLANG_API void arStack_init(arStack* s);

// Release the supplied stack
ARLANG_API void arStack_release(arStack* s);

// Move the stack pointer forward and return a pointer to where we can store the number of blocks required
// Returns NULL if there is no more free memory on the stack
ARLANG_API arByte* arStack_push(arStack* s, arInt32 size);

// Move the stack backwards. This will return a pointer to where you can read
// the poped value from. Will return NULL if you've popped to much memory
ARLANG_API arByte* arStack_pop(arStack* s, arInt32 size);

// Peek on the top most value on the stack with the supplied size
ARLANG_API const arByte* arStack_peek(arStack* s, arInt32 size);

// Verify that the stack is at the supplied position or after. Useful for when running
// the virtual machine in debug mode
ARLANG_API BOOL arStack_verify(arStack* s, const arByte* pos);

// Get the capacity of the stack, in bytes
ARLANG_API arInt32 arStack_capacity(const arStack* s);

// Count how much bytes are reserved on the stack
ARLANG_API int arStack_count(const arStack* s);

#endif
