#ifndef _VMI_OPS_H_
#define _VMI_OPS_H_

#include "vmi_config.h"

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

//
// Default type defines
//

#define VMI_INSTR_PROP_BOOL (0)
#define VMI_INSTR_PROP_INT8 (1)
#define VMI_INSTR_PROP_UINT8 (2)
#define VMI_INSTR_PROP_INT16 (3)
#define VMI_INSTR_PROP_UINT16 (4)
#define VMI_INSTR_PROP_INT32 (5)
#define VMI_INSTR_PROP_UINT32 (6)
#define VMI_INSTR_PROP_INT64 (7)
#define VMI_INSTR_PROP_UINT64 (8)
#define VMI_INSTR_PROP_FLOAT32 (9)
#define VMI_INSTR_PROP_FLOAT64 (10)
#define VMI_INSTR_PROP_PTR (11)

enum vmi_icodes
{
	// Load the value of an argument to the stack
	VMI_LDA = 1,

	// Load the address one of the arguments
	VMI_LDA_A,
	
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

	// Return the the caller instruction pointer
	VMI_RET,

	// Locals marker
	VMI_LOCALS,

	// Push a constant onto the stack using it's shorthand instruction
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

	// Convert a value on the stack and push the converted value to the stack
	VMI_CONV,

	// Compare the two top-most values on the stack and push an vm_int32 bit value
	// that represents the result
	VMI_CMP,

	// Jump to a new destination
	VMI_JMP,

	// Copy stack value at the top and push the copied value to the top of the stack
	VMI_COPY_S,

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
typedef struct vmi_instr_single_instruction vmi_instr_add;

// A lda(rgument) instruction
struct vmi_instr_lda
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
typedef struct vmi_instr_lda vmi_instr_lda;
typedef struct vmi_instr_lda vmi_instr_lda_a;

// A ldl instruction
struct vmi_instr_ldl
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
typedef struct vmi_instr_ldl vmi_instr_ldl;

// A stl instruction
struct vmi_instr_stl
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
typedef struct vmi_instr_stl vmi_instr_stl;

// A ldl_a(address) instruction
struct vmi_instr_ldl_a
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
typedef struct vmi_instr_ldl_a vmi_instr_ldl_a;

// A sturef instruction
struct vmi_instr_sturef
{
	OPCODE_HEADER;
	vm_int32 size;
};
typedef struct vmi_instr_sturef vmi_instr_sturef;

// A sturef_s(short) instruction
struct vmi_instr_sturef_s
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
typedef struct vmi_instr_sturef_s vmi_instr_sturef_s;

// A stelem_s(hort) instruction
struct vmi_instr_stelem_s
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
typedef struct vmi_instr_stelem_s vmi_instr_stelem_s;

// A stelem(ent) instruction
struct vmi_instr_stelem
{
	OPCODE_HEADER;
	vm_int32 size_per_element;
};
typedef struct vmi_instr_stelem vmi_instr_stelem;

typedef struct vmi_instr_stelem vmi_instr_ldelem;
typedef struct vmi_instr_stelem_s vmi_instr_ldelem_s;

// A ldc_s(hort) instruction
struct vmi_instr_ldc_s
{
	union
	{
		vmi_opcode_header header;
		vmi_opcode opcode;
		struct
		{
			vm_uint8 icode;
			vm_uint8 props1;
			vm_uint8 i8;
			vm_uint8 i8_1;
		};
		struct
		{
			vm_uint16 i16_0;
			vm_uint16 i16;
		};
	};
};
typedef struct vmi_instr_ldc_s vmi_instr_ldc_s;

// A ldc(onstant) 32 bit instruction
struct vmi_instr_ldc_i32
{
	OPCODE_HEADER;
	union {
		vm_int32 i32;
		struct {
			vm_int16 i16;
			vm_int16 i16_1;
		};
		struct {
			vm_int8 i8;
			vm_int8 i8_1;
			vm_int8 i8_2;
			vm_int8 i8_3;
		};
		vm_float32 f32;
#ifdef VM_32BIT
		vm_byte* ptr;
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
		vm_int64 i64;
		vm_float64 f64;
#ifdef VM_64BIT
		vm_byte* ptr;
#endif
	};
};
typedef struct vmi_instr_ldc_i64 vmi_instr_ldc_i64;
#ifdef VM_64BIT
typedef struct vmi_instr_ldc_i64 vmi_instr_ldc_ptr;
#endif

#define VMI_INSTR_CONST_PROP1_BOOL VMI_INSTR_PROP_BOOL
#define VMI_INSTR_CONST_PROP1_INT8 VMI_INSTR_PROP_INT8
#define VMI_INSTR_CONST_PROP1_UINT8 VMI_INSTR_PROP_UINT8
#define VMI_INSTR_CONST_PROP1_INT16 VMI_INSTR_PROP_INT16
#define VMI_INSTR_CONST_PROP1_UINT16 VMI_INSTR_PROP_UINT16
#define VMI_INSTR_CONST_PROP1_INT32 VMI_INSTR_PROP_INT32
#define VMI_INSTR_CONST_PROP1_UINT32 VMI_INSTR_PROP_UINT32
#define VMI_INSTR_CONST_PROP1_INT64 VMI_INSTR_PROP_INT64
#define VMI_INSTR_CONST_PROP1_UINT64 VMI_INSTR_PROP_UINT64
#define VMI_INSTR_CONST_PROP1_FLOAT32 VMI_INSTR_PROP_FLOAT32
#define VMI_INSTR_CONST_PROP1_FLOAT64 VMI_INSTR_PROP_FLOAT64

// An instruction that has a size that allows for 2 bytes of capacity
struct vmi_instr_mem_size2
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
		vmi_opcode_header header;
		vmi_opcode opcode;
		struct
		{
			vm_uint8 icode;
			vm_uint8 props1;

			// The expected (incoming) stack size that's required by the function - such as the arguments and the 
			// return values.
			vm_uint16 expected_stack_size;
		};
	};
	vmi_ip addr;
};
typedef struct vmi_instr_call vmi_instr_call;

// A callnative instruction
struct vmi_instr_callnative
{
	union
	{
		vmi_opcode_header header;
		vmi_opcode opcode;
		struct
		{
			vm_uint8 icode;
			vm_uint8 props1;

			// The expected (incoming) stack size that's required by the function - such as the arguments and the 
			// return values.
			vm_uint16 expected_stack_size;
		};
	};
	// Pointer to the C-function
	vm_nativefunc func_ptr;
};
typedef struct vmi_instr_callnative vmi_instr_callnative;

struct vmi_instr_ret
{
	union
	{
		vmi_opcode_header header;
		vmi_opcode opcode;
		struct
		{
			vm_uint8 icode;
			vm_uint8 props1;

			// Local memory to give back when returning from a function
			vm_uint16 pop_locals_size;
		};
	};
#if defined(VM_STACK_DEBUG)
	vm_uint32 expected_ebp_offset;
#endif
};
typedef struct vmi_instr_ret vmi_instr_ret;

// A locals instruction
struct vmi_instr_locals
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
	vmi_ip destination;
};
typedef struct vmi_instr_jmp vmi_instr_jmp;
#define VMI_INSTR_JMP_PROP1_TRUE (0)
#define VMI_INSTR_JMP_PROP1_FALSE (1)

// End-of-execution instruction. The execution will be halted when when this operation is reached
typedef vmi_instr_single_instruction vmi_instr_eoe;

enum vmi_ocodes
{
	VMI_OP_LDC_I1 = (VMI_LDC | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_INT8)),
	VMI_OP_LDC_UI1 = (VMI_LDC | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_UINT8)),
	VMI_OP_LDC_I2 = (VMI_LDC | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_INT16)),
	VMI_OP_LDC_UI2 = (VMI_LDC | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_UINT16)),
	VMI_OP_LDC_I4 = (VMI_LDC | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_INT32)),
	VMI_OP_LDC_UI4 = (VMI_LDC | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_UINT32)),
	VMI_OP_LDC_I8 = (VMI_LDC_I8 | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_INT64)),
	VMI_OP_LDC_UI8 = (VMI_LDC_I8 | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_UINT64)),
	VMI_OP_LDC_F4 = (VMI_LDC | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_FLOAT32)),
	VMI_OP_LDC_F8 = (VMI_LDC_I8 | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_FLOAT64)),

	VMI_OP_LDC_S_I1_0 = (VMI_LDC_S | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_INT8) | VMI_PROPS2_OPCODE(0)),
	VMI_OP_LDC_S_I1_1 = (VMI_LDC_S | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_INT8) | VMI_PROPS2_OPCODE(1)),
	VMI_OP_LDC_S_I1_N1 = (VMI_LDC_S | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_INT8) | VMI_PROPS2_OPCODE(-1)),

	VMI_OP_LDC_S_I2_0 = (VMI_LDC_S | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_INT16) | VMI_PROPS2_OPCODE(0)),
	VMI_OP_LDC_S_I2_1 = (VMI_LDC_S | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_INT16) | VMI_PROPS2_OPCODE(1)),
	VMI_OP_LDC_S_I2_N1 = (VMI_LDC_S | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_INT16) | VMI_PROPS2_OPCODE(-1)),

	VMI_OP_LDC_S_I4_0 = (VMI_LDC_S | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_INT32) | VMI_PROPS2_OPCODE(0)),
	VMI_OP_LDC_S_I4_1 = (VMI_LDC_S | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_INT32) | VMI_PROPS2_OPCODE(1)),
	VMI_OP_LDC_S_I4_N1 = (VMI_LDC_S | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_INT32) | VMI_PROPS2_OPCODE(-1)),

	VMI_OP_LDC_S_I8_0 = (VMI_LDC_S | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_INT64) | VMI_PROPS2_OPCODE(0)),
	VMI_OP_LDC_S_I8_1 = (VMI_LDC_S | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_INT64) | VMI_PROPS2_OPCODE(1)),
	VMI_OP_LDC_S_I8_N1 = (VMI_LDC_S | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_INT64) | VMI_PROPS2_OPCODE(-1)),

	VMI_OP_LDC_S_F4_0 = (VMI_LDC_S | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_FLOAT32) | VMI_PROPS2_OPCODE(0)),
	VMI_OP_LDC_S_F4_1 = (VMI_LDC_S | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_FLOAT32) | VMI_PROPS2_OPCODE(1)),
	VMI_OP_LDC_S_F4_N1 = (VMI_LDC_S | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_FLOAT32) | VMI_PROPS2_OPCODE(-1)),

	VMI_OP_LDC_S_F8_0 = (VMI_LDC_S | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_FLOAT64) | VMI_PROPS2_OPCODE(0)),
	VMI_OP_LDC_S_F8_1 = (VMI_LDC_S | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_FLOAT64) | VMI_PROPS2_OPCODE(1)),
	VMI_OP_LDC_S_F8_N1 = (VMI_LDC_S | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_FLOAT64) | VMI_PROPS2_OPCODE(-1)),

	VMI_OP_ADD_I1 = (VMI_ADD | VMI_PROPS1_OPCODE(VMI_INSTR_PROP_INT8)),
	VMI_OP_ADD_UI1 = (VMI_ADD | VMI_PROPS1_OPCODE(VMI_INSTR_PROP_UINT8)),
	VMI_OP_ADD_I2 = (VMI_ADD | VMI_PROPS1_OPCODE(VMI_INSTR_PROP_INT16)),
	VMI_OP_ADD_UI2 = (VMI_ADD | VMI_PROPS1_OPCODE(VMI_INSTR_PROP_UINT16)),
	VMI_OP_ADD_I4 = (VMI_ADD | VMI_PROPS1_OPCODE(VMI_INSTR_PROP_INT32)),
	VMI_OP_ADD_UI4 = (VMI_ADD | VMI_PROPS1_OPCODE(VMI_INSTR_PROP_UINT32)),
	VMI_OP_ADD_I8 = (VMI_ADD | VMI_PROPS1_OPCODE(VMI_INSTR_PROP_INT64)),
	VMI_OP_ADD_UI8 = (VMI_ADD | VMI_PROPS1_OPCODE(VMI_INSTR_PROP_UINT64)),
	VMI_OP_ADD_F4 = (VMI_ADD | VMI_PROPS1_OPCODE(VMI_INSTR_PROP_FLOAT32)),
	VMI_OP_ADD_F8 = (VMI_ADD | VMI_PROPS1_OPCODE(VMI_INSTR_PROP_FLOAT64)),

	VMI_OP_CONV_I2_I4 = (VMI_CONV | VMI_PROPS1_OPCODE(VMI_INSTR_PROP_INT16) | VMI_PROPS2_OPCODE(VMI_INSTR_PROP_INT32)),
	VMI_OP_CONV_I4_I2 = (VMI_CONV | VMI_PROPS1_OPCODE(VMI_INSTR_PROP_INT32) | VMI_PROPS2_OPCODE(VMI_INSTR_PROP_INT16)),

	VMI_OP_CMP_LT = (VMI_CMP | VMI_PROPS1_OPCODE(VMI_INSTR_CMP_PROP1_LT) | VMI_PROPS2_OPCODE(VMI_INSTR_CMP_PROP2_SIGNED)),
	VMI_OP_CMP_GT = (VMI_CMP | VMI_PROPS1_OPCODE(VMI_INSTR_CMP_PROP1_GT) | VMI_PROPS2_OPCODE(VMI_INSTR_CMP_PROP2_SIGNED)),

	VMI_OP_JMPT = (VMI_JMP | VMI_PROPS1_OPCODE(VMI_INSTR_JMP_PROP1_TRUE)),
	VMI_OP_JMPF = (VMI_JMP | VMI_PROPS1_OPCODE(VMI_INSTR_JMP_PROP1_FALSE)),

	VMI_OP_COPY_S_INT32 = (VMI_COPY_S | VMI_PROPS1_OPCODE(VMI_INSTR_PROP_INT32)),
	
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
};
#endif
