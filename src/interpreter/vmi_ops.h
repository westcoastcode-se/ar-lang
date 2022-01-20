#ifndef _VMI_OPS_H_
#define _VMI_OPS_H_

#include "../vm_config.h"

// High INT8 (the four last bits)
#define VMI_PROPS_HIGH(P) ((P) << 4)

// Low INT8 (the four first bits)
#define VMI_PROPS_LOW(P) ((P))

#define VMI_PROPS1_OPCODE(P) ((P) << 8)
#define VMI_PROPS2_OPCODE(P) ((P) << 16)
#define VMI_PROPS3_OPCODE(P) ((P) << 24)

enum vmi_icodes
{
	// Reserve memory for local variables
	VMI_VARS = 1,

	// Load a local variable to the stack
	VMI_LOAD,

	// Load a local long value to the stack
	VMI_LOADL,

	// Store a value from the top of the stack into a variable
	VMI_STORE,

	// Store a long value from the top of the stack into a variable
	VMI_STOREL,

	// End-of-execution. This will forcefully stop the execution of the virtual machine. Normally used
	// when calling a function from an unmanaged source, such as when used as a scripting language
	VMI_EOE = 255
};

enum vmi_ocodes
{
	VMI_OP_LOAD0 = (VMI_LOAD | VMI_PROPS1_OPCODE(0)),
	VMI_OP_LOAD1 = (VMI_LOAD | VMI_PROPS1_OPCODE(1)),
	VMI_OP_LOAD2 = (VMI_LOAD | VMI_PROPS1_OPCODE(2)),
	VMI_OP_LOAD3 = (VMI_LOAD | VMI_PROPS1_OPCODE(3)),
	VMI_OP_LOAD4 = (VMI_LOAD | VMI_PROPS1_OPCODE(4)),
	VMI_OP_LOAD5 = (VMI_LOAD | VMI_PROPS1_OPCODE(5)),
	VMI_OP_LOAD6 = (VMI_LOAD | VMI_PROPS1_OPCODE(6)),
	VMI_OP_LOADX = (VMI_LOAD | VMI_PROPS1_OPCODE(0xFF)),

	VMI_OP_LOADL0 = (VMI_LOADL | VMI_PROPS1_OPCODE(0)),
	VMI_OP_LOADL1 = (VMI_LOADL | VMI_PROPS1_OPCODE(1)),
	VMI_OP_LOADL2 = (VMI_LOADL | VMI_PROPS1_OPCODE(2)),
	VMI_OP_LOADL3 = (VMI_LOADL | VMI_PROPS1_OPCODE(3)),
	VMI_OP_LOADL4 = (VMI_LOADL | VMI_PROPS1_OPCODE(4)),
	VMI_OP_LOADL5 = (VMI_LOADL | VMI_PROPS1_OPCODE(5)),
	VMI_OP_LOADL6 = (VMI_LOADL | VMI_PROPS1_OPCODE(6)),
	VMI_OP_LOADLX = (VMI_LOADL | VMI_PROPS1_OPCODE(0xFF)),

	VMI_OP_STORE0 = (VMI_STORE | VMI_PROPS1_OPCODE(0)),
	VMI_OP_STORE1 = (VMI_STORE | VMI_PROPS1_OPCODE(1)),
	VMI_OP_STORE2 = (VMI_STORE | VMI_PROPS1_OPCODE(2)),
	VMI_OP_STORE3 = (VMI_STORE | VMI_PROPS1_OPCODE(3)),
	VMI_OP_STORE4 = (VMI_STORE | VMI_PROPS1_OPCODE(4)),
	VMI_OP_STORE5 = (VMI_STORE | VMI_PROPS1_OPCODE(5)),
	VMI_OP_STORE6 = (VMI_STORE | VMI_PROPS1_OPCODE(6)),
	VMI_OP_STOREX = (VMI_STORE | VMI_PROPS1_OPCODE(0xFF)),

	VMI_OP_STOREL0 = (VMI_STOREL | VMI_PROPS1_OPCODE(0)),
	VMI_OP_STOREL1 = (VMI_STOREL | VMI_PROPS1_OPCODE(1)),
	VMI_OP_STOREL2 = (VMI_STOREL | VMI_PROPS1_OPCODE(2)),
	VMI_OP_STOREL3 = (VMI_STOREL | VMI_PROPS1_OPCODE(3)),
	VMI_OP_STOREL4 = (VMI_STOREL | VMI_PROPS1_OPCODE(4)),
	VMI_OP_STOREL5 = (VMI_STOREL | VMI_PROPS1_OPCODE(5)),
	VMI_OP_STOREL6 = (VMI_STOREL | VMI_PROPS1_OPCODE(6)),
	VMI_OP_STORELX = (VMI_STOREL | VMI_PROPS1_OPCODE(0xFF)),
};

// A code processed by the process
typedef vm_int32 vmi_opcode;

// Header for app operations
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

// Header for all instructions
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

// Reserves memory for local variables
struct vmi_instr_vars
{
	OPCODE_HEADER;

	// The number of blocks to be reserved
	vm_int32 blocks;
};
typedef struct vmi_instr_vars vmi_instr_vars;

// Load a local variable and push it to the stack
struct vmi_instr_load
{
	OPCODE_HEADER;
};
typedef struct vmi_instr_load vmi_instr_load;

// Load a generic 1 block item to the stack
struct vmi_instr_loadx
{
	OPCODE_HEADER;
	vm_int16 block_index;
};
typedef struct vmi_instr_loadx vmi_instr_loadx;

// Load a 2-block value from a local variable and put it onto the stack
struct vmi_instr_load2b
{
	OPCODE_HEADER;
};
typedef struct vmi_instr_load2b vmi_instr_load2b;

// Pop a value from the stack and put the result into a local variable
struct vmi_instr_store
{
	OPCODE_HEADER;
};
typedef struct vmi_instr_store vmi_instr_store;

// Pop a value from the stack and put the result into a generic local variable block
struct vmi_instr_storex
{
	OPCODE_HEADER;
	vm_int16 block_index;
};
typedef struct vmi_instr_storex vmi_instr_storex;

// End-of-execution instruction. The execution will be halted when when this operation is reached
struct vmi_instr_eoe
{
	OPCODE_HEADER;
};
typedef struct vmi_instr_eoe vmi_instr_eoe;


#endif
