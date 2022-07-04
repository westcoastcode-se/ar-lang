#ifndef _VMC_PIPELINE_PACKAGE_H_
#define _VMC_PIPELINE_PACKAGE_H_

#include "../vm_string.h"
#include "vmp_list_args.h"
#include "vmp_list_packages.h"
#include "vmp_list_types.h"
#include "vmp_list_funcs.h"
#include "vmp_list_returns.h"
#include "vmp_list_locals.h"
#include "vmp_list_markers.h"
#include "vmp_list_inherits_from.h"
#include "vmp_list_inherited_by.h"

struct vmp_package
{
	struct vmp_pipeline* pipeline;

	// The name of the package
	vm_string name;

	// All types in this package
	vmp_list_types types;

	// All functions in this package
	vmp_list_funcs funcs;
};
typedef struct vmp_package vmp_package;

// A type such as an int32. It can also be a more complex struct type
struct vmp_type
{
	// The package this type is part of
	vmp_package* package;

	// The name for this type
	vm_string name;

	// Size, in bytes, that this type takes. If the size is 0 then this type has an unknown size
	vm_uint32 size;

	// Flags
	vm_uint32 flags;

	// Information on the interpreters data type
	vm_uint8 data_type;

	// If this type is of another type. Normally used for pointer- and array types
	struct vmp_type* of_type;

	// What this type is inherited from
	vmp_list_inherits_from inherits_from;

	// What types this type is inherited by
	vmp_list_inherited_by inherited_by;
};
typedef struct vmp_type vmp_type;

struct vmp_func
{
	// The package this function is part of
	vmp_package* package;

	// The name for this function
	vm_string name;

	// Offset where the bytecode for this function is found. Will be -1 if the offset
	// is now known yet. The compiler will try to resolve the offset during the
	// link step
	vm_uint32 offset;

	// The size, in bytes, for this function
	vm_uint32 body_size;

	// Arguments
	vmp_list_args args;

	// Return values
	vmp_list_returns returns;

	// Locals
	vmp_list_locals locals;

	// Memory markers
	vmp_list_markers markers;

	// The stack size required for this function to work
	vm_uint32 args_stack_size;
	vm_uint32 returns_stack_size;
	vm_uint32 locals_stack_size;

	// The first instruction in this function
	struct vmp_instr_header* first_instr;
	struct vmp_instr_header* last_instr;
};
typedef struct vmp_func vmp_func;

struct vmp_arg
{
	// Function this argument is part of
	const vmp_func* func;

	// The type this argument is
	const vmp_type* type;

	// The name of this argument, if any
	vm_string name;

	// Offset, in bytes, where this argument is located on the stack (from EPB's point of view)
	vm_uint32 offset;
};
typedef struct vmp_arg vmp_arg;

// Represents a return value from a function
struct vmp_return
{
	// Function this return property is part of
	const vmp_func* func;

	// The type this return is
	const vmp_type* type;

	// Offset, in bytes, where this return block is located on the stack (from EPB's point of view)
	vm_uint32 offset;
};
typedef struct vmp_return vmp_return;

struct vmp_local
{
	// Function this argument is part of
	const vmp_func* func;

	// The type this argument is
	const vmp_type* type;

	// The name of this argument, if any
	vm_string name;

	// Offset, in bytes, where this argument is located on the stack (from EPB's point of view)
	vm_uint32 offset;
};
typedef struct vmp_local vmp_local;

enum vmp_instr_type
{
	VMP_INSTR_LDA = 1,
	VMP_INSTR_LDC,
	VMP_INSTR_LDC_S,
	VMP_INSTR_LDC_I8,
	VMP_INSTR_RET,
	VMP_INSTR_STR,
	VMP_INSTR_STL,
	VMP_INSTR_LDL,
	VMP_INSTR_LOCALS,
	VMP_INSTR_ALLOCS,
	VMP_INSTR_CALL,
	VMP_INSTR_ADD,
	VMP_INSTR_CMP,
	VMP_INSTR_JMP,
	VMP_INSTR_CONV,
	VMP_INSTR_EOE,
};
typedef enum vmp_instr_type vmp_instr_type;

struct vmp_instr_header
{
	// The function this instruction is part of
	const vmp_func* func;

	// Type of instruction
	vmp_instr_type instr_type;

	// Size of the instruction this type represents
	vm_int32 instr_size;

	// Offset, in the function, where this instruction starts
	vm_int32 instr_offset;

	// The next instruction
	struct vmp_instr_header* next;
};
typedef struct vmp_instr_header vmp_instr_header;
typedef struct vmp_instr_header vmp_instr;

// Default header for all instructions
#define VMC_PIPELINE_INSTR_HEADER \
	union {\
		vmp_instr_header header;\
		struct {\
			const vmp_func* func;\
			vmp_instr_type instr_type; \
			vm_int32 instr_size;\
			vm_int32 instr_offset;\
			struct vmp_instr_header* next;\
		}; \
	}

// Initialize object header
#define VMC_PIPELINE_INIT_HEADER(PTR, TYPE, SIZE) PTR->func = NULL; PTR->instr_type = TYPE; PTR->instr_size = SIZE; vm_int32 instr_offset = 0; PTR->next = NULL
#define VMC_PIPELINE_INSTR_BASE(PTR) (&PTR->header)

struct vmp_marker
{
	const vmp_func* func;
	vm_int32 instr_offset;
};
typedef struct vmp_marker vmp_marker;

// New package
extern vmp_package* vmp_package_new(const vm_string* name);

// New package
extern vmp_package* vmp_package_newsz(const char* name, vm_int32 len);

// Destroy package
extern void vmp_package_destroy(vmp_package* p);

// Add the supplied function to this package
extern BOOL vmp_package_add_func(vmp_package* p, vmp_func* func);

// Add the supplied type to this package
extern BOOL vmp_package_add_type(vmp_package* p, vmp_type* type);

// Add the supplied type to this package
extern vmp_type* vmp_package_new_type(vmp_package* p, const vm_string* name, vm_uint32 size);

// Add the supplied type to this package
extern vmp_type* vmp_package_new_typesz(vmp_package* p, const char* name, int len, vm_uint32 size);

// Search for a type 
extern vmp_type* vmp_package_find_type(vmp_package* p, const vm_string* name);

// New type
extern vmp_type* vmp_type_new(const vm_string* name);

// Destroy type
extern void vmp_type_destroy(vmp_type* p);

// Check to see if the supplied type inherits from the test
extern BOOL vmp_type_test_inherits_from(const vmp_type* type, const vmp_type* inherits_from);

// Tell the pipeline that the supplied inherits from another type. Returns FALSE if the inheritence is not allowed
extern BOOL vmp_type_inherit_from(vmp_type* type, vmp_type* inherits_from);

// Check if you can convert one type into another using convertion
extern BOOL vmp_type_can_convert(const vmp_type* from, const vmp_type* to);

// New arg
extern vmp_arg* vmp_arg_new();

// Destroy arg
extern void vmp_arg_free(vmp_arg* a);

// Set the name of the argument
extern void vmp_arg_set_name(vmp_arg* a, const vm_string* name);

// Set the name of the argument
extern void vmp_arg_set_namesz(vmp_arg* a, const char* name, vm_int32 len);

// New return value
extern vmp_return* vmp_return_new();

// Destroy return
extern void vmp_return_free(vmp_return* a);

// New arg
extern vmp_local* vmp_local_new();

// Destroy arg
extern void vmp_local_free(vmp_local* l);

// Set the name of the argument
extern void vmp_local_set_name(vmp_local* l, const vm_string* name);

// Set the name of the argument
extern void vmp_local_set_namesz(vmp_local* l, const char* name, vm_int32 len);

// New type
extern vmp_func* vmp_func_new(const vm_string* name);

// New type
extern vmp_func* vmp_func_newsz(const char* name, vm_int32 name_len);

// Destroy type
extern void vmp_func_destroy(vmp_func* p);

// Add an argument to this function
extern BOOL vmp_func_add_arg(vmp_func* f, vmp_arg* arg);

// Add an argument to this function
extern vmp_arg* vmp_func_new_arg(vmp_func* f, vmp_type* type);

// Add an return value to this function
extern BOOL vmp_func_add_return(vmp_func* f, vmp_return* ret);

// Add a return value from this function
extern vmp_return* vmp_func_new_return(vmp_func* f, vmp_type* type);

// Add a local variable to this function
extern BOOL vmp_func_add_local(vmp_func* f, vmp_local* local);

// Add a local variable for this function of a specific type
extern vmp_local* vmp_func_new_local(vmp_func* f, vmp_type* type);

// Add a memory marker
extern vmp_marker* vmp_func_new_marker(vmp_func* f);

// Set the first instruction to be called when jumping to the supplied marker
extern void vmp_marker_set_instr(vmp_marker* m, vmp_instr* instr);

// Remove memory marker
extern void vmp_marker_free(vmp_marker* m);

// Add a new instruction
extern vmp_instr* vmp_func_add_instr(vmp_func* f, vmp_instr* instr);

extern void vmp_func_begin_body(vmp_func* f);

extern void vmp_func_begin_end(vmp_func* f);

#endif
