#ifndef _VMI_STACK_H_
#define _VMI_STACK_H_

#include "../vm_config.h"

// A block of memory stored on the stack
struct vmi_stack_block
{
#if defined(VM_32BIT)
	union
	{
		vm_int32 i32;
		void* addr;
		struct
		{
			vm_int8 i8;
			vm_int8 i8_1;
			vm_int8 i8_2;
			vm_int8 i8_3;
		};
		struct
		{
			vm_int16 i16;
			vm_int16 i16_1;
		};
	};
#else
	union
	{
		vm_int64 i64;
		void* addr;
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
#endif
};
typedef struct vmi_stack_block vmi_stack_block;

// Default the initial stack size is 32kb
#if !defined(VMI_STACK_NUM_BLOCKS)
#	define VMI_STACK_NUM_BLOCKS (VM_STACK_DEFAULT_SIZE / sizeof(vmi_stack_block))
#endif

// Manages the memory on the stack
struct vmi_stack
{
	vmi_stack_block blocks[VMI_STACK_NUM_BLOCKS];
	vmi_stack_block* top;
	const vmi_stack_block* end;
};
typedef struct vmi_stack vmi_stack;

// Initialize the supplied stack
extern void vmi_stack_init(vmi_stack* s);

// Release the supplied stack
extern void vmi_stack_release(vmi_stack* s);

// Move the stack pointer forward and return a pointer to where we can store the number of blocks required
// Returns NULL if there is no more free memory on the stack
extern vmi_stack_block* vmi_stack_push(vmi_stack* s, vm_int32 blocks);

// Move the stack backwards. This will return a pointer to where you can read
// the poped value from. Will return NULL if you've popped to much memory
extern vmi_stack_block* vmi_stack_pop(vmi_stack* s, vm_int32 blocks);

// Get the capacity of the stack, in bytes
inline static vm_int32 vmi_stack_capacity(const vmi_stack* s)
{
	return (vm_int32)(s->end - s->blocks);
}

// Count how much bytes are reserved on the stack
inline static int vmi_stack_count(const vmi_stack* s)
{
	return (vm_int32)(s->top - s->blocks);
}

#ifndef VMI_STACK_BLOCK_TOLOCAL
#	if defined(VM_32BIT)
#		define VMI_STACK_BLOCK_TOLOCAL(DEST, SRC) DEST.i32 = SRC->i32
#	else
#		define VMI_STACK_BLOCK_TOLOCAL(DEST, SRC) DEST.i64 = SRC->i64
#	endif
#endif

#ifndef VMI_STACK_BLOCK_FROMLOCAL
#	if defined(VM_32BIT)
#		define VMI_STACK_BLOCK_FROMLOCAL(DEST, SRC) DEST->i32 = SRC.i32
#	else
#		define VMI_STACK_BLOCK_FROMLOCAL(DEST, SRC) DEST->i64 = SRC.i64
#	endif
#endif

#endif
