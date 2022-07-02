#ifndef _VMC_PIPELINE_INSTR_H_
#define _VMC_PIPELINE_INSTR_H_

#include "../vm_config.h"
#include "vmp_types.h"
#include "../interpreter/vmi_ops.h"

// lda <index>
struct vmp_instr_def_lda
{
	VMC_PIPELINE_INSTR_HEADER;
	
	// Argument index to load
	vm_uint32 index;
};
typedef struct vmp_instr_def_lda vmp_instr_def_lda;

// str <index>
struct vmp_instr_def_str
{
	VMC_PIPELINE_INSTR_HEADER;

	// Return index to save
	vm_uint32 index;
};
typedef struct vmp_instr_def_str vmp_instr_def_str;

// Represents a constant value up to 8 bytes
struct vmp_constant
{
	// Represents an unknown value up to 8 bytes
	union {
		vm_int64 i8;
		vm_float64 f8;
		struct {
			vm_float32 f4;
			vm_float32 f4_1;
		};
		struct {
			vm_int32 i4;
			vm_int32 i4_1;
		};
		struct {
			vm_int16 i2;
			vm_int16 i2_[3];
		};
		struct {
			vm_int8 i1;
			vm_int8 i1_[7];
		};
#if defined(VM_64BIT)
		vm_byte* ptr;
#else
		struct {
			vm_byte* ptr;
			vm_byte* ptr_;
		};
#endif
	};
	vm_int32 type;
};
typedef struct vmp_constant vmp_constant;

extern vmp_constant vmp_const_i1(vm_int8 value);
extern vmp_constant vmp_const_i2(vm_int16 value);
extern vmp_constant vmp_const_i4(vm_int32 value);
extern vmp_constant vmp_const_i8(vm_int64 value);
extern vmp_constant vmp_const_f4(vm_float32 value);
extern vmp_constant vmp_const_f8(vm_float64 value);
extern vmp_constant vmp_const_ptr(vm_byte* value);

// ldc <type> <value>
struct vmp_instr_def_ldc
{
	VMC_PIPELINE_INSTR_HEADER;

	// The constant type
	const vmp_type* type;

	// The constant value
	vmp_constant constant;
};
typedef struct vmp_instr_def_ldc vmp_instr_def_ldc;

// ldc_s <type> <value>
struct vmp_instr_def_ldc_s
{
	VMC_PIPELINE_INSTR_HEADER;

	// The constant type
	const vmp_type* type;

	// The constant value
	vmp_constant constant;
};
typedef struct vmp_instr_def_ldc_s vmp_instr_def_ldc_s;

// add <type>
struct vmp_instr_def_add
{
	VMC_PIPELINE_INSTR_HEADER;

	// The types available are defined by the interpreter
	vm_int8 type;
};
typedef struct vmp_instr_def_add vmp_instr_def_add;

// Represents a basic instruction with no arguments
struct vmp_instr_def_basic
{
	VMC_PIPELINE_INSTR_HEADER;
};
typedef struct vmp_instr_def_basic vmp_instr_def_basic;
typedef struct vmp_instr_def_basic vmp_instr_def_ret;
typedef struct vmp_instr_def_basic vmp_instr_def_eoe;

// Create a new add instruction and return it
extern vmp_instr* vmp_instr_lda(vm_uint32 index);

// Create a new str instruction and return it
extern vmp_instr* vmp_instr_str(vm_uint32 index);

// Create a constant value instruction
extern vmp_instr* vmp_instr_ldc(const vmp_type* type, vmp_constant constant);

// Create a constant value instruction put into a large container
extern vmp_instr* vmp_instr_ldc_i8(const vmp_type* type, vmp_constant constant);

// Create a constant value instruction for smaller values, such as 16- and 8 bit constants
extern vmp_instr* vmp_instr_ldc_s(const vmp_type* type, vmp_constant constant);

// Create a new add instruction and return it
extern vmp_instr* vmp_instr_add(vm_int8 type);

// Create a new ret instruction
extern vmp_instr* vmp_instr_basic(vmp_instr_type type, vm_int32 size);

// Create a new ret instruction
inline static vmp_instr* vmp_instr_ret() {
	return vmp_instr_basic(VMP_INSTR_RET, sizeof(vmi_instr_ret));
}

// Create a new ret instruction
inline static vmp_instr* vmp_instr_eoe() {
	return vmp_instr_basic(VMP_INSTR_EOE, sizeof(vmi_instr_eoe));
}

// Build the bytecode for the supplied instruction and then 
extern const vmp_instr* vmp_instr_build(const vmp_instr* h, struct vmp_builder* builder);

#endif

