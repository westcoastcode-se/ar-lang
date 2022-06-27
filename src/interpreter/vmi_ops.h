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

enum vmi_icodes
{
	// Begin a function invocation (with no local variables)
	VMI_BEGIN = 1,

	// Load the value of an argument to the stack
	VMI_LDA,

	// Load the address one of the arguments
	VMI_LDA_A,
	
	// Save stack value to return target
	VMI_SAVE_R,

	// Allocate memory on the stack
	VMI_ALLOC_S,

	// Free memory from the stack
	VMI_FREE_S,

	// Call a function
	VMI_CALL,

	// Return the the caller instruction pointer
	VMI_RET,

	// Locals marker
	VMI_LOCALS,

	// Push a constant onto the stack using it's shorthand instruction
	VMI_LDC_S,

	// Push a constant onto the stack
	VMI_LDC,

	// Push a 64 bit constant onto the stack
	VMI_LDC_I64,

	// Load local variable and put the result on the stack
	VMI_LOAD_L,

	// Save value on the stack to a local variable
	VMI_SAVE_L,

	// Load the memory address to where a local variable is located
	VMI_LDL_A,

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

			// The expected (incoming) stack size that's required by the function - such as the arguments and the 
			// return values
			vm_uint16 expected_stack_size;
		};
	};
};
typedef struct vmi_instr_begin vmi_instr_begin;

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

// A load_l(ocal) instruction
struct vmi_instr_load_l
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
typedef struct vmi_instr_load_l vmi_instr_load_l;

// A save_l(ocal) instruction
struct vmi_instr_save_l
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
typedef struct vmi_instr_save_l vmi_instr_save_l;

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
	OPCODE_HEADER;
	union
	{
		vm_uint32 _pop_memory;
		struct
		{
			vm_uint16 pop_locals_size;
			vm_uint16 pop_stack_size;
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

// A conv(ert) instruction
struct vmi_instr_conv
{
	OPCODE_HEADER;
};
typedef struct vmi_instr_conv vmi_instr_conv;

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
	VMI_OP_LDC_I8 = (VMI_LDC | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_INT8)),
	VMI_OP_LDC_I16 = (VMI_LDC | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_INT16)),
	VMI_OP_LDC_I32 = (VMI_LDC | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_INT32)),
	VMI_OP_LDC_I64 = (VMI_LDC_I64 | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_INT64)),
	VMI_OP_LDC_F32 = (VMI_LDC | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_FLOAT32)),
	VMI_OP_LDC_F64 = (VMI_LDC_I64 | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_FLOAT64)),

	VMI_OP_LDC_S_I8_0 = (VMI_LDC_S | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_INT8) | VMI_PROPS2_OPCODE(0)),
	VMI_OP_LDC_S_I8_1 = (VMI_LDC_S | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_INT8) | VMI_PROPS2_OPCODE(1)),
	VMI_OP_LDC_S_I8_N1 = (VMI_LDC_S | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_INT8) | VMI_PROPS2_OPCODE(-1)),
	VMI_OP_LDC_S_I16_0 = (VMI_LDC_S | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_INT16) | VMI_PROPS2_OPCODE(0)),
	VMI_OP_LDC_S_I16_1 = (VMI_LDC_S | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_INT16) | VMI_PROPS2_OPCODE(1)),
	VMI_OP_LDC_S_I16_N1 = (VMI_LDC_S | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_INT16) | VMI_PROPS2_OPCODE(-1)),
	VMI_OP_LDC_S_I32_0 = (VMI_LDC_S | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_INT32) | VMI_PROPS2_OPCODE(0)),
	VMI_OP_LDC_S_I32_1 = (VMI_LDC_S | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_INT32) | VMI_PROPS2_OPCODE(1)),
	VMI_OP_LDC_S_I32_N1 = (VMI_LDC_S | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_INT32) | VMI_PROPS2_OPCODE(-1)),
	VMI_OP_LDC_S_I64_0 = (VMI_LDC_S | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_INT64) | VMI_PROPS2_OPCODE(0)),
	VMI_OP_LDC_S_I64_1 = (VMI_LDC_S | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_INT64) | VMI_PROPS2_OPCODE(1)),
	VMI_OP_LDC_S_I64_N1 = (VMI_LDC_S | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_INT64) | VMI_PROPS2_OPCODE(-1)),
	VMI_OP_LDC_S_F32_0 = (VMI_LDC_S | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_FLOAT32) | VMI_PROPS2_OPCODE(0)),
	VMI_OP_LDC_S_F32_1 = (VMI_LDC_S | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_FLOAT32) | VMI_PROPS2_OPCODE(1)),
	VMI_OP_LDC_S_F32_N1 = (VMI_LDC_S | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_FLOAT32) | VMI_PROPS2_OPCODE(-1)),
	VMI_OP_LDC_S_F64_0 = (VMI_LDC_S | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_FLOAT64) | VMI_PROPS2_OPCODE(0)),
	VMI_OP_LDC_S_F64_1 = (VMI_LDC_S | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_FLOAT64) | VMI_PROPS2_OPCODE(1)),
	VMI_OP_LDC_S_F64_N1 = (VMI_LDC_S | VMI_PROPS1_OPCODE(VMI_INSTR_CONST_PROP1_FLOAT64) | VMI_PROPS2_OPCODE(-1)),

	VMI_OP_ADD_I16 = (VMI_ADD | VMI_PROPS1_OPCODE(VMI_INSTR_ADD_PROP1_INT16)),
	VMI_OP_ADD_I32 = (VMI_ADD | VMI_PROPS1_OPCODE(VMI_INSTR_ADD_PROP1_INT32)),

	VMI_OP_CONV_I16_I32 = (VMI_CONV | VMI_PROPS1_OPCODE(VMI_INSTR_PROP_INT16) | VMI_PROPS2_OPCODE(VMI_INSTR_PROP_INT32)),
	VMI_OP_CONV_I32_I16 = (VMI_CONV | VMI_PROPS1_OPCODE(VMI_INSTR_PROP_INT32) | VMI_PROPS2_OPCODE(VMI_INSTR_PROP_INT16)),

	VMI_OP_CMP_LT = (VMI_CMP | VMI_PROPS1_OPCODE(VMI_INSTR_CMP_PROP1_LT) | VMI_PROPS2_OPCODE(VMI_INSTR_CMP_PROP2_SIGNED)),
	VMI_OP_CMP_GT = (VMI_CMP | VMI_PROPS1_OPCODE(VMI_INSTR_CMP_PROP1_GT) | VMI_PROPS2_OPCODE(VMI_INSTR_CMP_PROP2_SIGNED)),

	VMI_OP_JMPT = (VMI_JMP | VMI_PROPS1_OPCODE(VMI_INSTR_JMP_PROP1_TRUE)),
	VMI_OP_JMPF = (VMI_JMP | VMI_PROPS1_OPCODE(VMI_INSTR_JMP_PROP1_FALSE)),

	VMI_OP_COPY_S_INT32 = (VMI_COPY_S | VMI_PROPS1_OPCODE(VMI_INSTR_PROP_INT32)),
	
	VMI_OP_STUREF_S_I8 = (VMI_STUREF_S | VMI_PROPS1_OPCODE(1)),
	VMI_OP_STUREF_S_I16 = (VMI_STUREF_S | VMI_PROPS1_OPCODE(2)),
	VMI_OP_STUREF_S_I32 = (VMI_STUREF_S | VMI_PROPS1_OPCODE(4)),
	VMI_OP_STUREF_S_I64 = (VMI_STUREF_S | VMI_PROPS1_OPCODE(8)),

	VMI_OP_STELEM_S_I8 = (VMI_STELEM_S | VMI_PROPS1_OPCODE(1)),
	VMI_OP_STELEM_S_I16 = (VMI_STELEM_S | VMI_PROPS1_OPCODE(2)),
	VMI_OP_STELEM_S_I32 = (VMI_STELEM_S | VMI_PROPS1_OPCODE(4)),
	VMI_OP_STELEM_S_I64 = (VMI_STELEM_S | VMI_PROPS1_OPCODE(8)),

	VMI_OP_LDELEM_S_I8 = (VMI_LDELEM_S | VMI_PROPS1_OPCODE(1)),
	VMI_OP_LDELEM_S_I16 = (VMI_LDELEM_S | VMI_PROPS1_OPCODE(2)),
	VMI_OP_LDELEM_S_I32 = (VMI_LDELEM_S | VMI_PROPS1_OPCODE(4)),
	VMI_OP_LDELEM_S_I64 = (VMI_LDELEM_S | VMI_PROPS1_OPCODE(8)),
};
#endif
