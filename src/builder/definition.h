#ifndef _ARB_INSTR_H_
#define _ARB_INSTR_H_

#include "../arPrimitiveValue.h"
#include "../arInstructions.h"
#include "types.h"

// Represents an instruction that's executed when running a function
typedef struct arB_definition
{
	// The function this instruction is part of
	const arB_func* func;

	// Type of instruction
	arIncode instr_type;

	// Size of the instruction this type represents
	arInt32 instr_size;

	// Offset, in the function, where this instruction starts
	arInt32 instr_offset;

	// The previous instruction
	struct arB_definition* prev;

	// The next instruction
	struct arB_definition* next;
} arB_definition;

// Convert a definition into it's base-type
#define asB_definition(x) (&x->header)

// An instruction without any specials
typedef struct arB_def_empty
{
	arB_definition header;
} arB_def_empty, vmp_instr_def_basic, vmp_instr_def_ret, vmp_instr_def_eoe, vmp_instr_def_locals;

// An instruction that makes use of an argument
typedef struct arB_def_with_arg
{
	arB_definition header;
	const arB_arg* arg;
} arB_def_with_arg, vmp_instr_def_lda, vmp_instr_def_lda_a;

// An instruction that makes use of a function
typedef struct arB_def_with_func
{
	arB_definition header;
	// The function we want to push to the stack
	const arB_func* target_func;
} arB_def_with_func, vmp_instr_def_ldf, vmp_instr_def_call, vmp_instr_def_callnative;

// Instruction that makes use of a local variable
typedef struct arB_def_with_local
{
	arB_definition header;
	// Local reference
	const arB_local* local;
} arB_def_with_local, vmp_instr_def_ldl, vmp_instr_def_ldl_a, vmp_instr_def_stl;

// Instruction for global variables
typedef struct arB_def_with_global
{
	arB_definition header;
	// Global variable
	arB_global* global;
} arB_def_with_global, vmp_instr_def_ldg, vmp_instr_def_ldg_a, vmp_instr_def_stg;

// An instruction that makes use of a type and nothing else
typedef struct arB_def_with_type
{
	arB_definition header;
	// The types available are defined by the interpreter
	const arB_type* type;
} arB_def_with_type, vmp_instr_def_add, vmp_instr_def_sub, vmp_instr_def_mul, vmp_instr_def_div,
vmp_instr_def_neg, vmp_instr_def_sturef, vmp_instr_def_stelem, vmp_instr_def_ldelem,
vmp_instr_def_stelem_s, vmp_instr_def_sturef_s, vmp_instr_def_ldelem_s;

// Instruction that makes use of an constant and type
typedef struct arB_def_const
{
	arB_definition header;
	// The constant type
	const arB_type* type;
	// The constant value
	arPrimitiveValue constant;
} arB_def_const, vmp_instr_def_ldc, vmp_instr_def_ldc_s;

// Instruction that has to do with memory manipulation, such as allocating memory from the heap
typedef struct arB_def_memory
{
	arB_definition header;
	// The amount, in bytes, that's allowed to be pushed on the stack
	arInt16 amount;
	// Or alternatively a type
	const arB_type* type;
} arB_def_memory, vmp_instr_def_allocs, vmp_instr_def_frees, vmp_instr_def_alloch, vmp_instr_def_freeh;

// calluref <func>
typedef struct arB_def_with_func_sign
{
	arB_definition header;
	// The function definition that we are calling.
	// TODO: Change this into a "type" that represents the signature of the function
	const arB_func* func_def;
} arB_def_with_func_sign, vmp_instr_def_calluref;

// Instruction that has to do with an operator and a typ, such as comparing two values
typedef struct arB_def_with_op_type
{
	arB_definition header;
	// The bit operation type
	arInt32 op;
	// The types available are defined by the interpreter
	const arB_type* type;
} arB_def_with_op_type, vmp_instr_def_bit, vmp_instr_def_cmp;

// Instruction that makes use of a marker, normally done when jumping
typedef struct arB_def_with_marker
{
	arB_definition header;
	// The type of operator used by this jump function is using
	arInt8 op;
	// Marker we want to jump to
	const arB_marker* marker;
} arB_def_with_marker, vmp_instr_def_jmp;

// Instruction for converting one value to another
typedef struct arB_def_conv
{
	arB_definition header;
	// The type we are converting from
	const arB_type* from_type;
	// The type we are converting to
	const arB_type* to_type;
} arB_def_conv, vmp_instr_def_conv;

// Create a new lda instruction and return it
ARLANG_API arB_definition* arB_instr_lda(const arB_arg* arg);

// Create a new lda_a instruction and return it
ARLANG_API arB_definition* arB_instr_lda_a(const arB_arg* arg);

// Create a new stl instruction and return it
ARLANG_API arB_definition* arB_instr_stl(const arB_local* local);

// Create a new ldl instruction and return it
ARLANG_API arB_definition* arB_instr_ldl(const arB_local* local);

// Create a new ldl_a instruction and return it
ARLANG_API arB_definition* arB_instr_ldl_a(const arB_local* local);

// Create a new ldf instruction and return it
ARLANG_API arB_definition* arB_instr_ldf(const arB_func* func);

// Add an instruction to allocate local variable memory for the supplied function
ARLANG_API arB_definition* arB_instr_locals(const arB_func* func);

// Create a constant value instruction
ARLANG_API arB_definition* arB_instr_ldc(const arB_type * type, arPrimitiveValue constant);

// Create a constant value instruction put into a large container
ARLANG_API arB_definition* arB_instr_ldc_i64(const arB_type* type, arPrimitiveValue constant);

// Create a constant value instruction for smaller values, such as 16- and 8 bit constants
ARLANG_API arB_definition* vmp_instr_ldc_i16(const arB_type* type, arPrimitiveValue constant);

// Create a new ldg instruction and return it
ARLANG_API arB_definition* arB_instr_ldg(arB_global* g);

// Create a new ldg instruction and return it
ARLANG_API arB_definition* arB_instr_ldg_a(arB_global* g);

// Create a new ldg instruction and return it
ARLANG_API arB_definition* arB_instr_stg(arB_global* g);

// Create a allocation instruction on the stack based on a type
ARLANG_API arB_definition* arB_instr_allocs(const arB_type* type);

// Create a allocation instruction on the stack based on a constant amount
ARLANG_API arB_definition* arB_instr_allocs_const(arInt16 amount);

// Create a free instruction on the stack based on a type
ARLANG_API arB_definition* arB_instr_frees(const arB_type* type);

// Create a free instruction on the stack based on a constant amount
ARLANG_API arB_definition* arB_instr_frees_const(arInt16 amount);

// Create a allocation instruction on the heap based on a value on the stack
ARLANG_API arB_definition* arB_instr_alloch(const arB_type* type);

// Create a allocation instruction on the heap based on a type
ARLANG_API arB_definition* arB_instr_alloch_type(const arB_type* type);

// Create a allocation instruction on the heap based on a constant amount
ARLANG_API arB_definition* arB_instr_alloch_const(arInt16 amount);

// Create a free instruction on the heap based on a type
ARLANG_API arB_definition* arB_instr_freeh();

// Create a stunref instruction
ARLANG_API arB_definition* arB_instr_sturef(const arB_type* type);

// Create a stelem instruction for a specific array type
ARLANG_API arB_definition* arB_instr_stelem(const arB_type* type);

// Create a ldelem instruction for a specific array type
ARLANG_API arB_definition* arB_instr_ldelem(const arB_type* type);

// Call the supplied function
ARLANG_API arB_definition* arB_instr_call(const arB_func* func);

// Call the supplied function
ARLANG_API arB_definition* arB_instr_callnative(const arB_func* func);

// Call the function on the top of the stack
ARLANG_API arB_definition* arB_instr_calluref(const arB_func* func);

// Create a new add instruction and return it
ARLANG_API arB_definition* arB_instr_add(const arB_type* type);

// Create a new sub instruction and return it
ARLANG_API arB_definition* arB_instr_sub(const arB_type* type);

// Create a new mul instruction and return it
ARLANG_API arB_definition* arB_instr_mul(const arB_type* type);

// Create a new div instruction and return it
ARLANG_API arB_definition* arB_instr_div(const arB_type* type);

// Create a new cmp instruction and return it
ARLANG_API arB_definition* arB_instr_clt(const arB_type* type);

// Create a new cmp instruction and return it
ARLANG_API arB_definition* arB_instr_cgt(const arB_type* type);

// Create a new jmp instruction and return it
ARLANG_API arB_definition* arB_instr_jmpt(const arB_marker* marker);

// Create a new jmp instruction and return it
ARLANG_API arB_definition* arB_instr_jmpf(const arB_marker* marker);

// Create a new conv instruction
ARLANG_API arB_definition* arB_instr_conv(const arB_type* from, const arB_type* to);

// Create a new neg instruction
ARLANG_API arB_definition* arB_instr_neg(const arB_type* type);

// Create a new bit instruction
ARLANG_API arB_definition* arB_instr_bit(arInt32 op, const arB_type* type);

// Create a new bitnot instruction
ARLANG_API arB_definition* arB_instr_bit_not(const arB_type* type);

// Create a new ret instruction
ARLANG_API arB_definition* arB_instr_empty(arOpcode type, arInt32 size);

// Create a new ret instruction
ARLANG_API arB_definition* arB_instr_ret();

// Create a new ret instruction
ARLANG_API arB_definition* arB_instr_eoe();

// Test how many previous instructions there are and if they are at least count
ARLANG_API BOOL arB_instr_test_prev_count(const arB_definition* instr, arInt32 count);

// Verify if the instruction is a constant and that it doesn't exceed the supplied value
ARLANG_API BOOL arB_instr_ldc_i8_leq(const arB_definition* instr, BOOL ret_if_not_constant, arInt64 value);

// Check if two instructions are of the same type
ARLANG_API BOOL arB_instr_test_type_eq(const arB_definition* instr1, const arB_definition* instr2);

// Get the i64 value from the supplied instruction
ARLANG_API arInt64 arB_instr_ldc_i8_get(const arB_definition* instr);

// Link two instructions together. If instr is NULL then next will be returned
ARLANG_API arB_definition* arB_instr_link(arB_definition* instr, arB_definition* next);

// Get the last instruction
ARLANG_API arB_definition* arB_instr_get_last(arB_definition* instr);

// Build the bytecode for the supplied instruction and then 
ARLANG_API const arB_definition* arB_instr_build(const arB_definition* h, struct arBuilder* builder);

#endif

