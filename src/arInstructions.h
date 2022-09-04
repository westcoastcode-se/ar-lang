#ifndef _ARLANG_INSTRUCTIONS_H_
#define _ARLANG_INSTRUCTIONS_H_

#include "arconf.h"
#include "arPrimitive.h"

// High INT8 (the four last bits)
#define VMI_PROPS_HIGH(P) ((P) << 4)

// Low INT8 (the four first bits)
#define VMI_PROPS_LOW(P) ((P))

//
// Helper structs to put opcodes into their appropriate position in memory
//

#define VMI_PROPS1_OPCODE(P) ((P) << 8)
#define VMI_PROPS2_OPCODE(P) ((P) << 16)
#define VMI_PROPS3_OPCODE(P) ((P) << 24)

enum vmi_icodes
{
	// Load the value of an argument to the stack
	VMI_LDA = 1,

	// Load the address one of the arguments
	VMI_LDA_A,

	// Load a global value to the stack
	VMI_LDG,

	// Load the address where the global value is located
	VMI_LDG_A,

	// Store a value to a global value
	VMI_STG,

	// Allocate memory on the stack
	VMI_ALLOCS,

	// Free memory from the stack
	VMI_FREES,

	// Allocate memory on the heap
	VMI_ALLOCH,

	// Allocate memory on the heap based on a constant value
	VMI_ALLOCH_C,

	// Free memory from the heap based on a constant value
	VMI_FREEH,

	// Call a function
	VMI_CALL,

	// Call a native (C) function
	VMI_CALLNATIVE,

	// Call a function where the address of the function can be found on the stack
	VMI_CALLUREF,

	// Return the the caller instruction pointer
	VMI_RET,

	// Locals marker
	VMI_LOCALS,

	// Push a constant onto the stack using it's shorthand instruction. Works for 16-bit values and smaller
	VMI_LDC_S,

	// Push a constant onto the stack
	VMI_LDC,

	// Push a 8 byte constant onto the stack
	VMI_LDC_I8,

	// Load local variable and put the result on the stack
	VMI_LDL,

	// Load the memory address to where a local variable is located
	VMI_LDL_A,

	// Store a value from the stack into a local variable
	VMI_STL,

	// Push a function reference onto the stack
	VMI_LDF,

	// Store the value on the stack into the memory address located on the stack
	VMI_STUREF,

	// Store the value on the stack into the memory address located on the stack using a specialized instruction for types with a size smaller than 256
	VMI_STUREF_S,

	// Store an element to the memory address on the stack
	VMI_STELEM,

	// Store an element to the memory address on the stack using a specialized instruction for types with a size smaller than 256
	VMI_STELEM_S,

	// Load an element from the memory address on the stack
	VMI_LDELEM,

	// Load an element from the memory address on the stack using a specialized instruction for types with a size smaller than 256
	VMI_LDELEM_S,

	// Add two values on the stack
	VMI_ADD,

	// Subtract two types
	VMI_SUB,

	// Multiply two values
	VMI_MUL,

	// Divide two values
	VMI_DIV,

	// Convert a value on the stack and push the converted value to the stack
	VMI_CONV,

	// Compare the two top-most values on the stack and push an arInt32 bit value
	// that represents the result
	VMI_CMP,

	// Jump to a new destination
	VMI_JMP,

	// Copy stack value at the top and push the copied value to the top of the stack
	VMI_COPY_S,

	// Negate the value on the stack
	VMI_NEG,

	// A bit operation
	VMI_BIT,

	// End-of-execution. This will forcefully stop the execution of the virtual machine. Normally used
	// when calling a function from an unmanaged source, such as when used as a scripting language
	VMI_EOE = 255
};

// Header for all operations executed by the virtual machine
typedef struct arOperationHeader
{
	union
	{
		arOpcode opcode;
		struct
		{
			arIncode icode;
			arUint8 props1;
			arUint8 props2;
			arUint8 props3;
		};
	};
} arOperationHeader;

// Header for all instructions (must be identical with arOperationHeader)
#define OPCODE_HEADER union { \
	arOperationHeader header; \
	arOpcode opcode; \
	struct { \
		arIncode icode; \
		arUint8 props1; \
		arUint8 props2; \
		arUint8 props3; \
	};  \
}

// A single instruction block. This represents an instruction that has no extra data
struct vmi_instr_single_instruction
{
	OPCODE_HEADER;
};
typedef struct vmi_instr_single_instruction vmi_instr_single_instruction;
typedef struct vmi_instr_single_instruction vmi_instr_add;
typedef struct vmi_instr_single_instruction vmi_instr_sub;
typedef struct vmi_instr_single_instruction vmi_instr_mul;
typedef struct vmi_instr_single_instruction vmi_instr_div;

// A simple instruction that has an 8-bit size and offset
typedef struct arInstruction_size_offset
{
	union
	{
		arOperationHeader header;
		arOpcode opcode;
		struct
		{
			arIncode icode;
			arUint8 size;
			arUint8 offset;
			arUint8 props3;
		};
	};
} arInstruction_size_offset, arInstruction_lda, arInstruction_lda_a, arInstruction_ldl, arInstruction_ldl_a, 
arInstruction_stl;

// A ldg instruction
typedef struct arInstruction_ldg
{
	union
	{
		arOperationHeader header;
		arOpcode opcode;
		struct
		{
			arIncode icode;
			arUint8 size;
			arUint8 offset;
			arUint8 props3;
		};
	};
	arByte* addr;
} arInstruction_ldg, arInstruction_ldg_a, arInstruction_stg;

// A "load function to the stack" instruction
struct vmi_instr_ldf
{
	OPCODE_HEADER;
	arIP addr;
};
typedef struct vmi_instr_ldf vmi_instr_ldf;

// A sturef instruction
struct vmi_instr_sturef
{
	OPCODE_HEADER;
	arInt32 size;
};
typedef struct vmi_instr_sturef vmi_instr_sturef;

// A sturef_s(short) instruction
struct vmi_instr_sturef_s
{
	union
	{
		arOperationHeader header;
		arOpcode opcode;
		struct
		{
			arIncode icode;
			arUint8 size;
			arUint8 offset;
			arUint8 props3;
		};
	};
};
typedef struct vmi_instr_sturef_s vmi_instr_sturef_s;

// A stelem_s(hort) instruction
struct vmi_instr_stelem_s
{
	union
	{
		arOperationHeader header;
		arOpcode opcode;
		struct
		{
			arIncode icode;
			arUint8 size;
			arUint8 offset;
			arUint8 props3;
		};
	};
};
typedef struct vmi_instr_stelem_s vmi_instr_stelem_s;

// A stelem(ent) instruction
struct vmi_instr_stelem
{
	OPCODE_HEADER;
	arInt32 size_per_element;
};
typedef struct vmi_instr_stelem vmi_instr_stelem;

typedef struct vmi_instr_stelem vmi_instr_ldelem;
typedef struct vmi_instr_stelem_s vmi_instr_ldelem_s;

// A ldc_s(hort) instruction
struct vmi_instr_ldc_s
{
	union
	{
		arOperationHeader header;
		arOpcode opcode;
		struct
		{
			arIncode icode;
			arUint8 props1;
			arUint8 i8;
			arUint8 i8_1;
		};
		struct
		{
			arUint16 i16_0;
			arUint16 i16;
		};
	};
};
typedef struct vmi_instr_ldc_s vmi_instr_ldc_s;

// A ldc(onstant) 32 bit instruction
struct vmi_instr_ldc_i32
{
	OPCODE_HEADER;
	union {
		arInt32 i32;
		struct {
			arInt16 i16;
			arInt16 i16_1;
		};
		struct {
			arInt8 i8;
			arInt8 i8_1;
			arInt8 i8_2;
			arInt8 i8_3;
		};
		arFloat32 f32;
#ifdef VM_32BIT
		arByte* ptr;
#endif
	};
};
typedef struct vmi_instr_ldc_i32 vmi_instr_ldc_i32;
#ifdef VM_32BIT
typedef struct vmi_instr_ldc_i32 vmi_instr_const_ptr;
#endif

// A ldc(onstant) 64 bit instruction
struct vmi_instr_ldc_i64
{
	OPCODE_HEADER;
	union {
		arInt64 i64;
		arFloat64 f64;
#ifdef ARLANG_64BIT
		arByte* ptr;
#endif
	};
};
typedef struct vmi_instr_ldc_i64 vmi_instr_ldc_i64;
#ifdef ARLANG_64BIT
typedef struct vmi_instr_ldc_i64 vmi_instr_ldc_ptr;
#endif

// An instruction that has a size that allows for 2 bytes of capacity
struct vmi_instr_mem_size2
{
	union
	{
		arOperationHeader header;
		arOpcode opcode;
		struct
		{
			arIncode icode;
			arUint8 props1;
			arUint16 size;
		};
	};
};

// A allocs(tack) instruction
typedef struct vmi_instr_mem_size2 vmi_instr_allocs;
// A frees(tack) instruction
typedef struct vmi_instr_mem_size2 vmi_instr_frees;
// A alloch(eap)_c(onst) instruction
typedef struct vmi_instr_mem_size2 vmi_instr_alloch_c;
// A alloch(eap) instruction
typedef struct vmi_instr_single_instruction vmi_instr_alloch;
// A freeh(eap) instruction
typedef struct vmi_instr_single_instruction vmi_instr_freeh;

// A call instruction
struct vmi_instr_call
{
	union
	{
		arOperationHeader header;
		arOpcode opcode;
		struct
		{
			arIncode icode;
			arUint8 props1;

			// The expected (incoming) stack size that's required by the function - such as the arguments and the 
			// return values.
			arUint16 expected_stack_size;
		};
	};
	arIP addr;
};
typedef struct vmi_instr_call vmi_instr_call;

// A callnative instruction
struct vmi_instr_callnative
{
	union
	{
		arOperationHeader header;
		arOpcode opcode;
		struct
		{
			arIncode icode;
			arUint8 props1;

			// The expected (incoming) stack size that's required by the function - such as the arguments and the 
			// return values.
			arUint16 expected_stack_size;
		};
	};
	// Pointer to the C-function
	arB_native_func func_ptr;
};
typedef struct vmi_instr_callnative vmi_instr_callnative;

// Call a function based on an address on the top of stack
struct vmi_instr_calluref
{
	union
	{
		arOperationHeader header;
		arOpcode opcode;
		struct
		{
			arIncode icode;
			arUint8 props1;

			// The expected (incoming) stack size that's required by the function - such as the arguments and the 
			// return values.
			arUint16 expected_stack_size;
		};
	};
	// Just to ensure that calluref and callnative and call are the same size (for now)
	// TODO: Add support for different sizes here
	arIP _;
};
typedef struct vmi_instr_calluref vmi_instr_calluref;

struct vmi_instr_ret
{
	union
	{
		arOperationHeader header;
		arOpcode opcode;
		struct
		{
			arIncode icode;
			arUint8 props1;

			// Local memory to give back when returning from a function
			arUint16 pop_locals_size;
		};
	};
#if defined(VM_STACK_DEBUG)
	arUint32 expected_ebp_offset;
#endif
};
typedef struct vmi_instr_ret vmi_instr_ret;

// A locals instruction
struct vmi_instr_locals
{
	union
	{
		arOperationHeader header;
		arOpcode opcode;
		struct
		{
			arIncode icode;
			arUint8 props1;
			arUint16 size;
		};
	};
};
typedef struct vmi_instr_locals vmi_instr_locals;

typedef struct vmi_instr_single_instruction vmi_instr_conv;

// A cmp(are) instruction
struct vmi_instr_cmp
{
	OPCODE_HEADER;
};
typedef struct vmi_instr_cmp vmi_instr_cmp;
#define VMI_INSTR_CMP_PROP1_LT (0)
#define VMI_INSTR_CMP_PROP1_LE (1)
#define VMI_INSTR_CMP_PROP1_GT (2)
#define VMI_INSTR_CMP_PROP1_GE (3)
#define VMI_INSTR_CMP_PROP2_SIGNED (0)
#define VMI_INSTR_CMP_PROP2_UNSIGNED (1)

// A jmp instruction
struct vmi_instr_jmp
{
	OPCODE_HEADER;
	arIP destination;
};
typedef struct vmi_instr_jmp vmi_instr_jmp;
#define VMI_INSTR_JMP_PROP1_TRUE (0)
#define VMI_INSTR_JMP_PROP1_FALSE (1)

// Negate the current value on the stack
typedef struct vmi_instr_single_instruction vmi_instr_neg;

// Bit operations
typedef struct vmi_instr_single_instruction vmi_instr_bit;
#define VMI_INSTR_BIT_PROPS2_NOT (1)

// End-of-execution instruction. The execution will be halted when when this operation is reached
typedef vmi_instr_single_instruction vmi_instr_eoe;

enum vmi_ocodes
{
	VMI_OP_LDC_I1 = (VMI_LDC | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_I8)),
	VMI_OP_LDC_UI1 = (VMI_LDC | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_UI8)),
	VMI_OP_LDC_I2 = (VMI_LDC | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_I16)),
	VMI_OP_LDC_UI2 = (VMI_LDC | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_UI16)),
	VMI_OP_LDC_I4 = (VMI_LDC | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_I32)),
	VMI_OP_LDC_UI4 = (VMI_LDC | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_UI32)),
	VMI_OP_LDC_I8 = (VMI_LDC_I8 | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_I64)),
	VMI_OP_LDC_UI8 = (VMI_LDC_I8 | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_UI64)),
	VMI_OP_LDC_F4 = (VMI_LDC | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_F32)),
	VMI_OP_LDC_F8 = (VMI_LDC_I8 | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_F64)),

	VMI_OP_LDC_S_I1_0 = (VMI_LDC_S | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_I8) | VMI_PROPS2_OPCODE(0)),
	VMI_OP_LDC_S_I1_1 = (VMI_LDC_S | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_I8) | VMI_PROPS2_OPCODE(1)),
	VMI_OP_LDC_S_I1_N1 = (VMI_LDC_S | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_I8) | VMI_PROPS2_OPCODE(-1)),

	VMI_OP_LDC_S_I2_0 = (VMI_LDC_S | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_I16) | VMI_PROPS2_OPCODE(0)),
	VMI_OP_LDC_S_I2_1 = (VMI_LDC_S | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_I16) | VMI_PROPS2_OPCODE(1)),
	VMI_OP_LDC_S_I2_N1 = (VMI_LDC_S | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_I16) | VMI_PROPS2_OPCODE(-1)),

	VMI_OP_LDC_S_I4_0 = (VMI_LDC_S | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_I32) | VMI_PROPS2_OPCODE(0)),
	VMI_OP_LDC_S_I4_1 = (VMI_LDC_S | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_I32) | VMI_PROPS2_OPCODE(1)),
	VMI_OP_LDC_S_I4_N1 = (VMI_LDC_S | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_I32) | VMI_PROPS2_OPCODE(-1)),

	VMI_OP_LDC_S_I8_0 = (VMI_LDC_S | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_I64) | VMI_PROPS2_OPCODE(0)),
	VMI_OP_LDC_S_I8_1 = (VMI_LDC_S | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_I64) | VMI_PROPS2_OPCODE(1)),
	VMI_OP_LDC_S_I8_N1 = (VMI_LDC_S | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_I64) | VMI_PROPS2_OPCODE(-1)),

	VMI_OP_LDC_S_F4_0 = (VMI_LDC_S | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_F32) | VMI_PROPS2_OPCODE(0)),
	VMI_OP_LDC_S_F4_1 = (VMI_LDC_S | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_F32) | VMI_PROPS2_OPCODE(1)),
	VMI_OP_LDC_S_F4_N1 = (VMI_LDC_S | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_F32) | VMI_PROPS2_OPCODE(-1)),

	VMI_OP_LDC_S_F8_0 = (VMI_LDC_S | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_F64) | VMI_PROPS2_OPCODE(0)),
	VMI_OP_LDC_S_F8_1 = (VMI_LDC_S | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_F64) | VMI_PROPS2_OPCODE(1)),
	VMI_OP_LDC_S_F8_N1 = (VMI_LDC_S | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_F64) | VMI_PROPS2_OPCODE(-1)),

	VMI_OP_ADD_I1 = (VMI_ADD | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_I8)),
	VMI_OP_ADD_UI1 = (VMI_ADD | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_UI8)),
	VMI_OP_ADD_I2 = (VMI_ADD | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_I16)),
	VMI_OP_ADD_UI2 = (VMI_ADD | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_UI16)),
	VMI_OP_ADD_I4 = (VMI_ADD | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_I32)),
	VMI_OP_ADD_UI4 = (VMI_ADD | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_UI32)),
	VMI_OP_ADD_I8 = (VMI_ADD | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_I64)),
	VMI_OP_ADD_UI8 = (VMI_ADD | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_UI64)),
	VMI_OP_ADD_F4 = (VMI_ADD | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_F32)),
	VMI_OP_ADD_F8 = (VMI_ADD | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_F64)),

	VMI_OP_SUB_I1 = (VMI_SUB | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_I8)),
	VMI_OP_SUB_UI1 = (VMI_SUB | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_UI8)),
	VMI_OP_SUB_I2 = (VMI_SUB | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_I16)),
	VMI_OP_SUB_UI2 = (VMI_SUB | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_UI16)),
	VMI_OP_SUB_I4 = (VMI_SUB | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_I32)),
	VMI_OP_SUB_UI4 = (VMI_SUB | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_UI32)),
	VMI_OP_SUB_I8 = (VMI_SUB | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_I64)),
	VMI_OP_SUB_UI8 = (VMI_SUB | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_UI64)),
	VMI_OP_SUB_F4 = (VMI_SUB | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_F32)),
	VMI_OP_SUB_F8 = (VMI_SUB | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_F64)),

	VMI_OP_MUL_I1 = (VMI_MUL | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_I8)),
	VMI_OP_MUL_UI1 = (VMI_MUL | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_UI8)),
	VMI_OP_MUL_I2 = (VMI_MUL | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_I16)),
	VMI_OP_MUL_UI2 = (VMI_MUL | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_UI16)),
	VMI_OP_MUL_I4 = (VMI_MUL | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_I32)),
	VMI_OP_MUL_UI4 = (VMI_MUL | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_UI32)),
	VMI_OP_MUL_I8 = (VMI_MUL | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_I64)),
	VMI_OP_MUL_UI8 = (VMI_MUL | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_UI64)),
	VMI_OP_MUL_F4 = (VMI_MUL | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_F32)),
	VMI_OP_MUL_F8 = (VMI_MUL | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_F64)),

	VMI_OP_DIV_I1 = (VMI_DIV | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_I8)),
	VMI_OP_DIV_UI1 = (VMI_DIV | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_UI8)),
	VMI_OP_DIV_I2 = (VMI_DIV | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_I16)),
	VMI_OP_DIV_UI2 = (VMI_DIV | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_UI16)),
	VMI_OP_DIV_I4 = (VMI_DIV | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_I32)),
	VMI_OP_DIV_UI4 = (VMI_DIV | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_UI32)),
	VMI_OP_DIV_I8 = (VMI_DIV | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_I64)),
	VMI_OP_DIV_UI8 = (VMI_DIV | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_UI64)),
	VMI_OP_DIV_F4 = (VMI_DIV | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_F32)),
	VMI_OP_DIV_F8 = (VMI_DIV | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_F64)),

	VMI_OP_CONV_I2_I4 = (VMI_CONV | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_I16) | VMI_PROPS2_OPCODE(ARLANG_PRIMITIVE_I32)),
	VMI_OP_CONV_I4_I2 = (VMI_CONV | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_I32) | VMI_PROPS2_OPCODE(ARLANG_PRIMITIVE_I16)),

	VMI_OP_CMP_LT = (VMI_CMP | VMI_PROPS1_OPCODE(VMI_INSTR_CMP_PROP1_LT) | VMI_PROPS2_OPCODE(VMI_INSTR_CMP_PROP2_SIGNED)),
	VMI_OP_CMP_GT = (VMI_CMP | VMI_PROPS1_OPCODE(VMI_INSTR_CMP_PROP1_GT) | VMI_PROPS2_OPCODE(VMI_INSTR_CMP_PROP2_SIGNED)),

	VMI_OP_JMPT = (VMI_JMP | VMI_PROPS1_OPCODE(VMI_INSTR_JMP_PROP1_TRUE)),
	VMI_OP_JMPF = (VMI_JMP | VMI_PROPS1_OPCODE(VMI_INSTR_JMP_PROP1_FALSE)),

	VMI_OP_COPY_S_INT32 = (VMI_COPY_S | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_I32)),

	VMI_OP_STUREF_S_I1 = (VMI_STUREF_S | VMI_PROPS1_OPCODE(1)),
	VMI_OP_STUREF_S_I2 = (VMI_STUREF_S | VMI_PROPS1_OPCODE(2)),
	VMI_OP_STUREF_S_I4 = (VMI_STUREF_S | VMI_PROPS1_OPCODE(4)),
	VMI_OP_STUREF_S_I8 = (VMI_STUREF_S | VMI_PROPS1_OPCODE(8)),

	VMI_OP_STELEM_S_I1 = (VMI_STELEM_S | VMI_PROPS1_OPCODE(1)),
	VMI_OP_STELEM_S_I2 = (VMI_STELEM_S | VMI_PROPS1_OPCODE(2)),
	VMI_OP_STELEM_S_I4 = (VMI_STELEM_S | VMI_PROPS1_OPCODE(4)),
	VMI_OP_STELEM_S_I8 = (VMI_STELEM_S | VMI_PROPS1_OPCODE(8)),

	VMI_OP_LDELEM_S_I1 = (VMI_LDELEM_S | VMI_PROPS1_OPCODE(1)),
	VMI_OP_LDELEM_S_I2 = (VMI_LDELEM_S | VMI_PROPS1_OPCODE(2)),
	VMI_OP_LDELEM_S_I4 = (VMI_LDELEM_S | VMI_PROPS1_OPCODE(4)),
	VMI_OP_LDELEM_S_I8 = (VMI_LDELEM_S | VMI_PROPS1_OPCODE(8)),

	VMI_OP_NEG_I1 = (VMI_NEG | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_I8)),
	VMI_OP_NEG_UI1 = (VMI_NEG | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_UI8)),
	VMI_OP_NEG_I2 = (VMI_NEG | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_I16)),
	VMI_OP_NEG_UI2 = (VMI_NEG | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_UI16)),
	VMI_OP_NEG_I4 = (VMI_NEG | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_I32)),
	VMI_OP_NEG_UI4 = (VMI_NEG | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_UI32)),
	VMI_OP_NEG_I8 = (VMI_NEG | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_I64)),
	VMI_OP_NEG_UI8 = (VMI_NEG | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_UI64)),
	VMI_OP_NEG_F4 = (VMI_NEG | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_F32)),
	VMI_OP_NEG_F8 = (VMI_NEG | VMI_PROPS1_OPCODE(ARLANG_PRIMITIVE_F64)),

	VMI_OP_BIT_NOT_I1 = (VMI_BIT | VMI_PROPS2_OPCODE(ARLANG_PRIMITIVE_I8) | VMI_PROPS1_OPCODE(VMI_INSTR_BIT_PROPS2_NOT)),
	VMI_OP_BIT_NOT_UI1 = (VMI_BIT | VMI_PROPS2_OPCODE(ARLANG_PRIMITIVE_UI8) | VMI_PROPS1_OPCODE(VMI_INSTR_BIT_PROPS2_NOT)),
	VMI_OP_BIT_NOT_I2 = (VMI_BIT | VMI_PROPS2_OPCODE(ARLANG_PRIMITIVE_I16) | VMI_PROPS1_OPCODE(VMI_INSTR_BIT_PROPS2_NOT)),
	VMI_OP_BIT_NOT_UI2 = (VMI_BIT | VMI_PROPS2_OPCODE(ARLANG_PRIMITIVE_UI16) | VMI_PROPS1_OPCODE(VMI_INSTR_BIT_PROPS2_NOT)),
	VMI_OP_BIT_NOT_I4 = (VMI_BIT | VMI_PROPS2_OPCODE(ARLANG_PRIMITIVE_I32) | VMI_PROPS1_OPCODE(VMI_INSTR_BIT_PROPS2_NOT)),
	VMI_OP_BIT_NOT_UI4 = (VMI_BIT | VMI_PROPS2_OPCODE(ARLANG_PRIMITIVE_UI32) | VMI_PROPS1_OPCODE(VMI_INSTR_BIT_PROPS2_NOT)),
	VMI_OP_BIT_NOT_I8 = (VMI_BIT | VMI_PROPS2_OPCODE(ARLANG_PRIMITIVE_I64) | VMI_PROPS1_OPCODE(VMI_INSTR_BIT_PROPS2_NOT)),
	VMI_OP_BIT_NOT_UI8 = (VMI_BIT | VMI_PROPS2_OPCODE(ARLANG_PRIMITIVE_UI64) | VMI_PROPS1_OPCODE(VMI_INSTR_BIT_PROPS2_NOT)),
};

#endif
