#ifndef _VMC_PIPELINE_INSTR_H_
#define _VMC_PIPELINE_INSTR_H_

#include "../vm_config.h"
#include "vmp_types.h"
#include "../interpreter/vmi_ops.h"

// lda <index>
struct vmp_instr_def_lda
{
	VMC_PIPELINE_INSTR_HEADER;
	const vmp_arg* arg;
};
typedef struct vmp_instr_def_lda vmp_instr_def_lda;

// lda_a <index>
struct vmp_instr_def_lda_a
{
	VMC_PIPELINE_INSTR_HEADER;
	const vmp_arg* arg;
};
typedef struct vmp_instr_def_lda_a vmp_instr_def_lda_a;

// ldr_a <index>
struct vmp_instr_def_ldr_a
{
	VMC_PIPELINE_INSTR_HEADER;

	// Return index to load
	vm_uint32 index;
};
typedef struct vmp_instr_def_ldr_a vmp_instr_def_ldr_a;

// stl <index>
struct vmp_instr_def_stl
{
	VMC_PIPELINE_INSTR_HEADER;
	const vmp_local* local;
};
typedef struct vmp_instr_def_stl vmp_instr_def_stl;

// locals
struct vmp_instr_def_locals
{
	VMC_PIPELINE_INSTR_HEADER;
};
typedef struct vmp_instr_def_locals vmp_instr_def_locals;

// ldl <index>
struct vmp_instr_def_ldl
{
	VMC_PIPELINE_INSTR_HEADER;
	const vmp_local* local;
};
typedef struct vmp_instr_def_ldl vmp_instr_def_ldl;

// ldf <func>
struct vmp_instr_def_ldf
{
	VMC_PIPELINE_INSTR_HEADER;

	// The function we want to push to the stack
	const vmp_func* target_func;
};
typedef struct vmp_instr_def_ldf vmp_instr_def_ldf;

// ldl_a <index>
struct vmp_instr_def_ldl_a
{
	VMC_PIPELINE_INSTR_HEADER;
	const vmp_local* local;
};
typedef struct vmp_instr_def_ldl_a vmp_instr_def_ldl_a;

// Represents a constant value up to 8 bytes
struct vmp_constant
{
	// Represents an unknown value up to 8 bytes
	union {
		vm_int64 i8;
		vm_uint64 ui8;
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
			vm_uint32 ui4;
			vm_uint32 ui4_1;
		};
		struct {
			vm_int16 i2;
			vm_int16 i2_[3];
		};
		struct {
			vm_uint16 ui2;
			vm_uint16 ui2_[3];
		};
		struct {
			vm_int8 i1;
			vm_int8 i1_[7];
		};
		struct {
			vm_uint8 ui1;
			vm_int8 ui1_[7];
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
extern vmp_constant vmp_const_ui1(vm_uint8 value);
extern vmp_constant vmp_const_i2(vm_int16 value);
extern vmp_constant vmp_const_ui2(vm_uint16 value);
extern vmp_constant vmp_const_i4(vm_int32 value);
extern vmp_constant vmp_const_ui4(vm_uint32 value);
extern vmp_constant vmp_const_i8(vm_int64 value);
extern vmp_constant vmp_const_ui8(vm_uint64 value);
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
typedef struct vmp_instr_def_ldc vmp_instr_def_ldc_s;

// ldg <global>
struct vmp_instr_def_ldg
{
	VMC_PIPELINE_INSTR_HEADER;

	// Global variable
	vmp_global* global;
};
typedef struct vmp_instr_def_ldg vmp_instr_def_ldg;
typedef struct vmp_instr_def_ldg vmp_instr_def_ldg_a;
typedef struct vmp_instr_def_ldg vmp_instr_def_stg;

// allocs <value|type>
struct vmp_instr_def_allocs
{
	VMC_PIPELINE_INSTR_HEADER;

	// The amount, in bytes, that's allowed to be pushed on the stack
	vm_int16 amount;

	// Or alternatively a type
	const vmp_type* type;
};
typedef struct vmp_instr_def_allocs vmp_instr_def_allocs;
typedef struct vmp_instr_def_allocs vmp_instr_def_frees;
typedef struct vmp_instr_def_allocs vmp_instr_def_alloch;
typedef struct vmp_instr_def_allocs vmp_instr_def_freeh;
// sturef <type>
struct vmp_instr_def_sturef
{
	VMC_PIPELINE_INSTR_HEADER;

	// Type to unref and set
	const vmp_type* type;
};
typedef struct vmp_instr_def_sturef vmp_instr_def_sturef;
typedef struct vmp_instr_def_sturef vmp_instr_def_sturef_s;

// sturef_s <type>
struct vmp_instr_def_stelem
{
	VMC_PIPELINE_INSTR_HEADER;

	// Array type to set
	const vmp_type* array_type;
};
typedef struct vmp_instr_def_stelem vmp_instr_def_stelem;
typedef struct vmp_instr_def_stelem vmp_instr_def_stelem_s;
typedef struct vmp_instr_def_stelem vmp_instr_def_ldelem;
typedef struct vmp_instr_def_stelem vmp_instr_def_ldelem_s;

// call <func>
struct vmp_instr_def_call
{
	VMC_PIPELINE_INSTR_HEADER;

	// The function we are calling
	const vmp_func* target_func;
};
typedef struct vmp_instr_def_call vmp_instr_def_call;

// callnative <func>
struct vmp_instr_def_callnative
{
	VMC_PIPELINE_INSTR_HEADER;

	// Definition
	const vmp_func* func_def;
};
typedef struct vmp_instr_def_callnative vmp_instr_def_callnative;

// calluref <func>
struct vmp_instr_def_calluref
{
	VMC_PIPELINE_INSTR_HEADER;

	// The function definition that we are calling.
	// TODO: Change this into a "type"
	const vmp_func* func_def;
};
typedef struct vmp_instr_def_calluref vmp_instr_def_calluref;

// add <type>
struct vmp_instr_def_add
{
	VMC_PIPELINE_INSTR_HEADER;

	// The types available are defined by the interpreter
	const vmp_type* type;
};
typedef struct vmp_instr_def_add vmp_instr_def_add;
typedef struct vmp_instr_def_add vmp_instr_def_sub;
typedef struct vmp_instr_def_add vmp_instr_def_mul;
typedef struct vmp_instr_def_add vmp_instr_def_div;

// add <type>
struct vmp_instr_def_cmp
{
	VMC_PIPELINE_INSTR_HEADER;

	// The type of comparator used by this compare function
	vm_int8 comparator;

	// What type is compared
	const vmp_type* compare_type;
};
typedef struct vmp_instr_def_cmp vmp_instr_def_cmp;

// jmp
struct vmp_instr_def_jmp
{
	VMC_PIPELINE_INSTR_HEADER;

	// The type of operator used by this jump function is using
	vm_int8 op;

	// Marker we want to jump to
	const vmp_marker* marker;
};
typedef struct vmp_instr_def_jmp vmp_instr_def_jmp;

// conv
struct vmp_instr_def_conv
{
	VMC_PIPELINE_INSTR_HEADER;

	// The type we are converting from
	const vmp_type* from_type;

	// The type we are converting to
	const vmp_type* to_type;
};
typedef struct vmp_instr_def_conv vmp_instr_def_conv;

// neg
typedef struct vmp_instr_def_neg
{
	VMC_PIPELINE_INSTR_HEADER;
	const vmp_type* type;
} vmp_instr_def_neg;

// Represents a basic instruction with no arguments
struct vmp_instr_def_basic
{
	VMC_PIPELINE_INSTR_HEADER;
};
typedef struct vmp_instr_def_basic vmp_instr_def_basic;
typedef struct vmp_instr_def_basic vmp_instr_def_ret;
typedef struct vmp_instr_def_basic vmp_instr_def_eoe;

// Create a new lda instruction and return it
extern vmp_instr* vmp_instr_lda(const vmp_arg* arg);

// Create a new lda_a instruction and return it
extern vmp_instr* vmp_instr_lda_a(const vmp_arg* arg);

// Create a new stl instruction and return it
extern vmp_instr* vmp_instr_stl(const vmp_local* local);

// Create a new ldl instruction and return it
extern vmp_instr* vmp_instr_ldl(const vmp_local* local);

// Create a new ldl_a instruction and return it
extern vmp_instr* vmp_instr_ldl_a(const vmp_local* local);

// Create a new ldf instruction and return it
extern vmp_instr* vmp_instr_ldf(const vmp_func* func);

// Add an instruction to allocate local variable memory for the supplied function
extern vmp_instr* vmp_instr_locals(const vmp_func* func);

// Create a constant value instruction
extern vmp_instr* vmp_instr_ldc(const vmp_type* type, vmp_constant constant);

// Create a constant value instruction put into a large container
extern vmp_instr* vmp_instr_ldc_i8(const vmp_type* type, vmp_constant constant);

// Create a constant value instruction for smaller values, such as 16- and 8 bit constants
extern vmp_instr* vmp_instr_ldc_s(const vmp_type* type, vmp_constant constant);

// Create a new ldg instruction and return it
extern vmp_instr* vmp_instr_ldg(vmp_global* g);

// Create a new ldg instruction and return it
extern vmp_instr* vmp_instr_ldg_a(vmp_global* g);

// Create a new ldg instruction and return it
extern vmp_instr* vmp_instr_stg(vmp_global* g);

// Create a allocation instruction on the stack based on a type
extern vmp_instr* vmp_instr_allocs(const vmp_type* type);

// Create a allocation instruction on the stack based on a constant amount
extern vmp_instr* vmp_instr_allocs_const(vm_int16 amount);

// Create a free instruction on the stack based on a type
extern vmp_instr* vmp_instr_frees(const vmp_type* type);

// Create a free instruction on the stack based on a constant amount
extern vmp_instr* vmp_instr_frees_const(vm_int16 amount);

// Create a allocation instruction on the heap based on a value on the stack
extern vmp_instr* vmp_instr_alloch();

// Create a allocation instruction on the heap based on a type
extern vmp_instr* vmp_instr_alloch_type(const vmp_type* type);

// Create a allocation instruction on the heap based on a constant amount
extern vmp_instr* vmp_instr_alloch_const(vm_int16 amount);

// Create a free instruction on the heap based on a type
extern vmp_instr* vmp_instr_freeh();

// Create a stunref instruction
extern vmp_instr* vmp_instr_sturef(const vmp_type* type);

// Create a stelem instruction for a specific array type
extern vmp_instr* vmp_instr_stelem(const vmp_type* type);

// Create a ldelem instruction for a specific array type
extern vmp_instr* vmp_instr_ldelem(const vmp_type* type);

// Call the supplied function
extern vmp_instr* vmp_instr_call(const vmp_func* func);

// Call the supplied function
extern vmp_instr* vmp_instr_callnative(const vmp_func* func);

// Call the function on the top of the stack
extern vmp_instr* vmp_instr_calluref(const vmp_func* func);

// Create a new add instruction and return it
extern vmp_instr* vmp_instr_add(const vmp_type* type);

// Create a new sub instruction and return it
extern vmp_instr* vmp_instr_sub(const vmp_type* type);

// Create a new mul instruction and return it
extern vmp_instr* vmp_instr_mul(const vmp_type* type);

// Create a new div instruction and return it
extern vmp_instr* vmp_instr_div(const vmp_type* type);

// Create a new cmp instruction and return it
extern vmp_instr* vmp_instr_clt(const vmp_type* type);

// Create a new cmp instruction and return it
extern vmp_instr* vmp_instr_cgt(const vmp_type* type);

// Create a new jmp instruction and return it
extern vmp_instr* vmp_instr_jmpt(const vmp_marker* marker);

// Create a new jmp instruction and return it
extern vmp_instr* vmp_instr_jmpf(const vmp_marker* marker);

// Create a new conv instruction
extern vmp_instr* vmp_instr_conv(const vmp_type* from, const vmp_type* to);

// Create a new neg instruction
extern vmp_instr* vmp_instr_neg(const vmp_type* type);

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

// Test how many previous instructions there are and if they are at least count
extern BOOL vmp_instr_test_prev_count(const vmp_instr* instr, vm_int32 count);

// Verify if the instruction is a constant and that it doesn't exceed the supplied value
extern BOOL vmp_instr_ldc_i8_leq(const vmp_instr* instr, BOOL ret_if_not_constant, vm_int64 value);

// Check if two instructions are of the same type
extern BOOL vmp_instr_test_type_eq(const vmp_instr* instr1, const vmp_instr* instr2);

// Get the i8 value from the supplied instruction
extern vm_int64 vmp_instr_ldc_i8_get(const vmp_instr* instr);

// Link two instructions together. If instr is NULL then next will be returned
extern vmp_instr* vmp_instr_link(vmp_instr* instr, vmp_instr* next);

// Get the last instruction
extern vmp_instr* vmp_instr_get_last(vmp_instr* instr);

// Build the bytecode for the supplied instruction and then 
extern const vmp_instr* vmp_instr_build(const vmp_instr* h, struct vmp_builder* builder);

#endif

