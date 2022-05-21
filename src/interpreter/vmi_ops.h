#ifndef _VMI_OPS_H_
#define _VMI_OPS_H_

#include "vmi_config.h"

// High INT8 (the four last bits)
#define VMI_PROPS_HIGH(P) ((P) << 4)

// Low INT8 (the four first bits)
#define VMI_PROPS_LOW(P) ((P))

#define VMI_PROPS1_OPCODE(P) ((P) << 8)
#define VMI_PROPS2_OPCODE(P) ((P) << 16)
#define VMI_PROPS3_OPCODE(P) ((P) << 24)

enum vmi_icodes
{
	// Begin a function invocation (with no local variables)
	VMI_BEGIN = 1,

	// Load argument to the stack
	VMI_LOAD_A,
	
	// Save stack value to return target
	VMI_SAVE_R,

	// Load constant value to the stack
	VMI_CONST,

	// Allocate memory on the stack
	VMI_ALLOC_S,

	// Free memory from the stack
	VMI_FREE_S,

	// Call a function
	VMI_CALL,

	// Return the the caller instruction pointer
	VMI_RET,

	// Add two values on the stack
	VMI_ADD,

	// Convert a value on the stack and push the converted value to the stack
	VMI_CONV,

	// End-of-execution. This will forcefully stop the execution of the virtual machine. Normally used
	// when calling a function from an unmanaged source, such as when used as a scripting language
	VMI_EOE = 255
};

// A code processed by the process
typedef vm_int32 vmi_opcode;

// Header for all instructions
struct vmi_opcode_header
{
	union
	{
		vmi_opcode opcode;
		struct
		{
			vm_uint8 icode;
			vm_uint8 props1;
			vm_uint8 props2;
			vm_uint8 props3;
		};
	};
};
typedef struct vmi_opcode_header vmi_opcode_header;

// Header for all instructions (must be identical with vmi_opcode_header)
#define OPCODE_HEADER union { \
	vmi_opcode_header header; \
	vmi_opcode opcode; \
	struct { \
		vm_uint8 icode; \
		vm_uint8 props1; \
		vm_uint8 props2; \
		vm_uint8 props3; \
	};  \
}

// A single instruction block. This represents an instruction that has no extra data
struct vmi_instr_single_instruction
{
	OPCODE_HEADER;
};
typedef struct vmi_instr_single_instruction vmi_instr_single_instruction;

#define VMI_INSTR_ADD_PROP1_INT16 (0)
#define VMI_INSTR_ADD_PROP1_INT32 (1)
typedef struct vmi_instr_single_instruction vmi_instr_add;

// A begin instruction
struct vmi_instr_begin
{
	// Header (must be identical with vmi_opcode_header)
	union
	{
		vmi_opcode_header header;
		vmi_opcode opcode;
		struct
		{
			vm_uint8 icode;
			vm_uint8 props1;
			vm_uint16 expected_stack_size;
		};
	};
};
typedef struct vmi_instr_begin vmi_instr_begin;

// A load_a(rgument) instruction
struct vmi_instr_load_a
{
	union
	{
		vmi_opcode_header header;
		vmi_opcode opcode;
		struct
		{
			vm_uint8 icode;
			vm_uint8 size;
			vm_uint8 offset;
			vm_uint8 props3;
		};
	};
};
typedef struct vmi_instr_load_a vmi_instr_load_a;

// A save_r(eturn) instruction
struct vmi_instr_save_r
{
	union
	{
		vmi_opcode_header header;
		vmi_opcode opcode;
		struct
		{
			vm_uint8 icode;
			vm_uint8 size;
			vm_uint8 offset;
			vm_uint8 props3;
		};
	};
};
typedef struct vmi_instr_save_r vmi_instr_save_r;

// A const(ant) int32 instruction
struct vmi_instr_const_int32
{
	OPCODE_HEADER;
	struct {
		vm_int32 value;
		struct {
			vm_int16 i16;
			vm_int16 i161;
		};
		struct {
			vm_int8 i8;
			vm_int8 i81;
			vm_int8 i82;
			vm_int8 i83;
		};
		vm_float32 f32;
#ifdef VM_32BIT
		vm_byte* ptr;
#endif
	};
};
typedef struct vmi_instr_const_int32 vmi_instr_const_int32;
#ifdef VM_32BIT
typedef struct vmi_instr_const_int32 vmi_instr_const_ptr;
#endif

#define VMI_INSTR_CONST_PROP1_BOOL (0)
#define VMI_INSTR_CONST_PROP1_INT8 (1)
#define VMI_INSTR_CONST_PROP1_UINT8 (2)
#define VMI_INSTR_CONST_PROP1_INT16 (3)
#define VMI_INSTR_CONST_PROP1_UINT16 (4)
#define VMI_INSTR_CONST_PROP1_INT32 (5)
#define VMI_INSTR_CONST_PROP1_UINT32 (6)
#define VMI_INSTR_CONST_PROP1_INT64 (7)
#define VMI_INSTR_CONST_PROP1_UINT64 (8)
#define VMI_INSTR_CONST_PROP1_FLOAT32 (9)
#define VMI_INSTR_CONST_PROP1_FLOAT64 (10)

// A alloc_s(tack) instruction
struct vmi_instr_alloc_s
{
	union
	{
		vmi_opcode_header header;
		vmi_opcode opcode;
		struct
		{
			vm_uint8 icode;
			vm_uint8 props1;
			vm_uint16 size;
		};
	};
};
typedef struct vmi_instr_alloc_s vmi_instr_alloc_s;

// A free_s(tack) instruction
struct vmi_instr_free_s
{
	union
	{
		vmi_opcode_header header;
		vmi_opcode opcode;
		struct
		{
			vm_uint8 icode;
			vm_uint8 props1;
			vm_uint16 size;
		};
	};
};
typedef struct vmi_instr_free_s vmi_instr_free_s;

// A call instruction
struct vmi_instr_call
{
	OPCODE_HEADER;
	vmi_ip addr;
};
typedef struct vmi_instr_call vmi_instr_call;

struct vmi_instr_ret
{
	// Header (must be identical with vmi_opcode_header)
	union
	{
		vmi_opcode_header header;
		vmi_opcode opcode;
		struct
		{
			vm_uint8 icode;
			vm_uint8 props1;
			vm_uint16 pop_stack_size;
		};
	};
#if defined(VM_STACK_DEBUG)
	vm_uint32 expected_ebp_offset;
#endif
};
typedef struct vmi_instr_ret vmi_instr_ret;

// A conv(ert) instruction
struct vmi_instr_conv
{
	OPCODE_HEADER;
};
typedef struct vmi_instr_conv vmi_instr_conv;

#define VMI_INSTR_CONV_PROP1_INT16 (0)
#define VMI_INSTR_CONV_PROP2_INT32 (0)

// End-of-execution instruction. The execution will be halted when when this operation is reached
typedef vmi_instr_single_instruction vmi_instr_eoe;

enum vmi_ocodes
{
	VMI_OP_CONST_INT8 = (VMI_CONST | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_INT8)),
	VMI_OP_CONST_INT16 = (VMI_CONST | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_INT16)),
	VMI_OP_CONST_INT32 = (VMI_CONST | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_INT32)),

	VMI_OP_ADD_I16 = (VMI_ADD | VMI_PROPS1_OPCODE(VMI_INSTR_ADD_PROP1_INT16)),
	VMI_OP_ADD_I32 = (VMI_ADD | VMI_PROPS1_OPCODE(VMI_INSTR_ADD_PROP1_INT32)),

	VMI_OP_CONV_I16_I32 = (VMI_CONV | VMI_PROPS1_OPCODE(VMI_INSTR_CONV_PROP1_INT16) | VMI_PROPS2_OPCODE(VMI_INSTR_CONV_PROP2_INT32)),
};

#endif
