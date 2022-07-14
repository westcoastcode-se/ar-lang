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
#include "vmp_list_imports.h"
#include "vmp_list_globals.h"

// Return value indicating that the item is already added once
#define VMP_LIST_ADDED (0)

// Return value indicating that the item is already added once
#define VMP_LIST_OUT_OF_MEMORY (1)

// Return value indicating that the item is already added once
#define VMP_LIST_ALREADY_ADDED (2)

// Return value indicating that the another item with the same identifier already exist
#define VMP_LIST_ALREADY_EXISTS (3)

// Different types of keywords
enum vmp_keyword_type
{
	VMP_KEYWORD_PACKAGE = (1 << 0),
	VMP_KEYWORD_TYPE = (1 << 1),
	VMP_KEYWORD_FUNC = (1 << 2),
	VMP_KEYWORD_ARG = (1 << 3),
	VMP_KEYWORD_RETURN = (1 << 4),
	VMP_KEYWORD_LOCAL = (1 << 5),
	VMP_KEYWORD_GLOBAL = (1 << 6),
	VMP_KEYWORD_CONST = (1 << 7)
};
typedef enum vmp_keyword_type vmp_keyword_type;

// Represents a generic keyword
struct vmp_keyword
{
	// Type
	vmp_keyword_type keyword_type;

	// The name of the keyword
	vm_string name;
};
typedef struct vmp_keyword vmp_keyword;

// Is the supplied keyword assignable
static inline BOOL vmp_keyword_is_assignable(vmp_keyword* k)
{
	switch (k->keyword_type) {
	case VMP_KEYWORD_ARG:
	case VMP_KEYWORD_RETURN:
	case VMP_KEYWORD_LOCAL:
		return TRUE;
	default:
		return FALSE;
	}
}

struct vmp_package
{
	// Header
	union
	{
		vmp_keyword header;
		struct
		{
			vmp_keyword_type keyword_type;
			vm_string name;
		};
	};

	struct vmp_pipeline* pipeline;

	// All types in this package
	vmp_list_types types;

	// All functions in this package
	vmp_list_funcs funcs;

	// Imports
	vmp_list_imports imports;

	// Global variables
	vmp_list_globals globals;
};
typedef struct vmp_package vmp_package;

// A type such as an int32. It can also be a more complex struct type
struct vmp_type
{
	// Header
	union
	{
		vmp_keyword header;
		struct
		{
			vmp_keyword_type keyword_type;
			vm_string name;
		};
	};

	// The package this type is part of
	vmp_package* package;

	// Size, in bytes, that this type takes. If the size is 0 then this type has an unknown size
	vm_uint32 size;

	// Flags
	vm_uint32 flags;

	// Information on the interpreters data type
	vm_uint8 data_type;

	// If this type is of another type. Normally used for pointer- and array types
	const struct vmp_type* of_type;

	// What this type is inherited from
	vmp_list_inherits_from inherits_from;

	// What types this type is inherited by
	vmp_list_inherited_by inherited_by;
};
typedef struct vmp_type vmp_type;

#define VMP_TYPE_FLAGS_PTR (1 << 0)
#define VMP_TYPE_FLAGS_ARRAY (1 << 1)

// Structure used to help creating a type
struct vmp_type_props
{
	vm_string name;
	vm_uint32 size;
	vm_uint32 flags;
	vm_uint8 data_type;
	const vmp_type* of_type;
};
typedef struct vmp_type_props vmp_type_props;

struct vmp_func
{
	// Header
	union
	{
		vmp_keyword header;
		struct
		{
			vmp_keyword_type keyword_type;
			vm_string name;
		};
	};

	// The package this function is part of
	vmp_package* package;

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

	// A reference to a native function if one exist
	vm_nativefunc native_func;

	// Flags used to define the function
	vm_int32 flags;

	// The stack size required for this function to work
	vm_uint32 args_stack_size;
	vm_uint32 returns_stack_size;
	vm_uint32 locals_stack_size;

	// The first instruction in this function
	struct vmp_instr_header* first_instr;
	struct vmp_instr_header* last_instr;
};
typedef struct vmp_func vmp_func;

// A function is public
#define VMP_FUNC_FLAGS_PUBLIC (1 << 0)
// A function is marked as an external function
#define VMP_FUNC_FLAGS_EXTERN (1 << 1)

struct vmp_arg
{
	// Header
	union
	{
		vmp_keyword header;
		struct
		{
			vmp_keyword_type keyword_type;
			vm_string name;
		};
	};

	// Function this argument is part of
	const vmp_func* func;

	// The type this argument is
	const vmp_type* type;

	// Offset, in bytes, where this argument is located on the stack (from EPB's point of view)
	vm_uint32 offset;

	// The index of the argument
	vm_uint32 index;
};
typedef struct vmp_arg vmp_arg;

// Represents a return value from a function
struct vmp_return
{
	// Header
	union
	{
		vmp_keyword header;
		struct
		{
			vmp_keyword_type keyword_type;
			vm_string name;
		};
	};

	// Function this return property is part of
	const vmp_func* func;

	// The type this return is
	const vmp_type* type;
};
typedef struct vmp_return vmp_return;

struct vmp_local
{
	// Header
	union
	{
		vmp_keyword header;
		struct
		{
			vmp_keyword_type keyword_type;
			vm_string name;
		};
	};

	// Function this argument is part of
	const vmp_func* func;

	// The type this argument is
	const vmp_type* type;

	// Offset, in bytes, where this argument is located on the stack (from EPB's point of view)
	vm_uint32 offset;

	// The index where the local variable exist
	vm_uint32 index;
};
typedef struct vmp_local vmp_local;

struct vmp_global
{
	// Header
	union
	{
		vmp_keyword header;
		struct
		{
			vmp_keyword_type keyword_type;
			vm_string name;
		};
	};

	// Package this global variable is part of
	const vmp_package* package;

	// The type this variable is
	const vmp_type* type;

	// Offset, in bytes, where the global varaible can be found
	vm_uint32 offset;
};
typedef struct vmp_global vmp_global;

enum vmp_instr_type
{
	VMP_INSTR_LDA = 1,
	VMP_INSTR_LDA_A,
	VMP_INSTR_LDC,
	VMP_INSTR_LDC_S,
	VMP_INSTR_LDC_I8,
	VMP_INSTR_LDF,
	VMP_INSTR_RET,
	VMP_INSTR_STL,
	VMP_INSTR_LDL,
	VMP_INSTR_LDL_A,
	VMP_INSTR_LOCALS,
	VMP_INSTR_LDG,
	VMP_INSTR_LDG_A,
	VMP_INSTR_STG,
	VMP_INSTR_ALLOCS,
	VMP_INSTR_FREES,
	VMP_INSTR_ALLOCH,
	VMP_INSTR_ALLOCH_C,
	VMP_INSTR_FREEH,
	VMP_INSTR_STUREF,
	VMP_INSTR_STUREF_S,
	VMP_INSTR_STELEM,
	VMP_INSTR_STELEM_S,
	VMP_INSTR_LDELEM,
	VMP_INSTR_LDELEM_S,
	VMP_INSTR_CALL,
	VMP_INSTR_CALLNATIVE,
	VMP_INSTR_CALLUREF,
	VMP_INSTR_ADD,
	VMP_INSTR_SUB,
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

	// The previous instruction
	struct vmp_instr_header* prev;

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
			struct vmp_instr_header* prev;\
			struct vmp_instr_header* next;\
		}; \
	}

// Initialize object header
#define VMC_PIPELINE_INIT_HEADER(PTR, TYPE, SIZE) PTR->func = NULL; PTR->instr_type = TYPE; PTR->instr_size = SIZE; vm_int32 instr_offset = 0; PTR->next = NULL; PTR->prev = NULL
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

// Add the supplied function to this package. This function will return 0 we added the type successfully
extern int vmp_package_add_func(vmp_package* p, vmp_func* func);

// Add the supplied type to this package. This function will return 0 we added the type successfully
extern int vmp_package_add_type(vmp_package* p, vmp_type* type);

// Add another package as an import. It's not allowed to have circular imports 
extern int vmp_package_add_import(vmp_package* p, vmp_package* imported);

// Add a global variable to a package
extern int vmp_package_add_global(vmp_package* p, vmp_global* g);

// Add the supplied type to this package
extern vmp_type* vmp_package_new_type(vmp_package* p, const vm_string* name, vm_uint32 size);

// Add the supplied type to this package
extern vmp_type* vmp_package_new_typesz(vmp_package* p, const char* name, int len, vm_uint32 size);

// Search for a type 
extern vmp_type* vmp_package_find_type(vmp_package* p, const vm_string* name);

// Search for a function
extern vmp_func* vmp_package_find_func(vmp_package* p, const vm_string* name);

// Search for an import
extern vmp_package* vmp_package_find_import(vmp_package* p, const vm_string* name);

// Search for a type in the supplied package or any of the imported packages
extern vmp_type* vmp_package_find_type_include_imports(vmp_package* p, const vm_string* name);

// Search for an global variable
extern vmp_global* vmp_package_find_global(vmp_package* p, const vm_string* name);

// Find a generic keyword in the supplied package
extern vmp_keyword* vmp_package_find_keyword(vmp_package* p, const vm_string* name);

// Add a global variable for the supplied package
extern vmp_global* vmp_package_new_global(vmp_package* p, vmp_type* type);

// New type
extern vmp_type* vmp_type_new(const vm_string* name);

// New type
extern vmp_type* vmp_type_new_from_props(const vmp_type_props* props);

// Get properties that can be used when creating a new type.
// Important! This function is not thread-safe
extern const vmp_type_props* vmp_type_props_get(const vm_string* name, vm_uint32 size, vm_uint32 flags, vm_uint8 data_type, const vmp_type* of_type);

// Mark the supplied type as an of type
extern BOOL vmp_type_of_type(vmp_type* t, const vmp_type* of_type);

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

// New local
extern vmp_local* vmp_local_new();

// Destroy arg
extern void vmp_local_free(vmp_local* l);

// Set the name of the argument
extern void vmp_local_set_name(vmp_local* l, const vm_string* name);

// Set the name of the argument
extern void vmp_local_set_namesz(vmp_local* l, const char* name, vm_int32 len);

// New global variable
extern vmp_global* vmp_global_new();

// Destroy the global variable
extern void vmp_global_free(vmp_global* g);

// Set the name of the variable
extern void vmp_global_set_name(vmp_global* g, const vm_string* name);

// Set the name of the variable
extern void vmp_global_set_namesz(vmp_global* g, const char* name, vm_int32 len);

// New type
extern vmp_func* vmp_func_new(const vm_string* name);

// New type
extern vmp_func* vmp_func_newsz(const char* name, vm_int32 name_len);

// Destroy type
extern void vmp_func_destroy(vmp_func* p);

// Add a specific flag
extern void vmp_func_add_flag(vmp_func* p, vm_int32 flag);

// Set the function
extern void vmp_func_set_nativefunc(vmp_func* p, vm_nativefunc func);

// Add an argument to this function
extern BOOL vmp_func_add_arg(vmp_func* f, vmp_arg* arg);

// Add an argument to this function
extern vmp_arg* vmp_func_new_arg(vmp_func* f, vmp_type* type);

// Search for an argument with the supplied name
extern vmp_arg* vmp_func_find_arg(vmp_func* f, const vm_string* name);

// Add an return value to this function
extern BOOL vmp_func_add_return(vmp_func* f, vmp_return* ret);

// Add a return value from this function
extern vmp_return* vmp_func_new_return(vmp_func* f, vmp_type* type);

// Add a local variable to this function
extern BOOL vmp_func_add_local(vmp_func* f, vmp_local* local);

// Search for a local variable with the supplied name
extern vmp_local* vmp_func_find_local(vmp_func* f, const vm_string* name);

// Add a local variable for this function of a specific type
extern vmp_local* vmp_func_new_local(vmp_func* f, vmp_type* type);

// Add a memory marker
extern vmp_marker* vmp_func_new_marker(vmp_func* f);

// Set the first instruction to be called when jumping to the supplied marker
extern void vmp_marker_set_instr(vmp_marker* m, vmp_instr* instr);

// Remove memory marker
extern void vmp_marker_free(vmp_marker* m);

// Add a new instruction. If the there's followup instructions attached to the instruction then those will be added
// as well. Returns the last added instruction
extern vmp_instr* vmp_func_add_instr(vmp_func* f, vmp_instr* instr);

// Inject a new instruction after another instruction. If NULL then it will be put at the beginning
extern vmp_instr* vmp_func_inject_after(vmp_func* f, vmp_instr* after, vmp_instr* instr);

// Remove the instruction and return it for deletion
extern vmp_instr* vmp_func_remove_instr(vmp_func* f, vmp_instr* instr);

// Find a generic keyword in the supplied package
extern vmp_keyword* vmp_func_find_keyword(vmp_func* f, const vm_string* name);

// Begin writing the body instructions
extern void vmp_func_begin_body(vmp_func* f);

// Instructions written to the body is now complete
extern void vmp_func_begin_end(vmp_func* f);

#endif
