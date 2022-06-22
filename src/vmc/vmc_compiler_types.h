#ifndef _VMC_COMPILER_TYPES_H_
#define _VMC_COMPILER_TYPES_H_

#include "../vm_string.h"
#include "vmc_linker.h"

// What type of object is this
enum vmc_type_header_type
{
	// Unknown type. Usually happens if you refer to a type that's not defined yet
	VMC_TYPE_HEADER_UNKNOWN,
	// Tells us that a definition is a package
	VMC_TYPE_HEADER_PACKAGE = 1,
	// Tells us that a definition is a variable
	VMC_TYPE_HEADER_VAR,
	// Tells us that a definition is a structure
	VMC_TYPE_HEADER_STRUCT,
	// Tells us that a definition is a interface
	VMC_TYPE_HEADER_INTERFACE,
	// Tells us that a definition is a function
	VMC_TYPE_HEADER_FUNC
};
typedef enum vmc_type_header_type vmc_type_header_type;

// Header for all types
struct vmc_type_header
{
	// Tells what type of object this is
	vmc_type_header_type type;

	// A unique id of the definition
	vm_uint32 id;

	// Size of this type in memory
	vm_int32 size;

	// Name of the definition
	vm_string name;
};
typedef struct vmc_type_header vmc_type_header;

// Convert an object to the object's header
#define TO_TYPE_HEADER(x) (x->header)

// Initialize object header
#define VMC_INIT_TYPE_HEADER(PTR, TYPE, SIZE) PTR->header.type = TYPE; PTR->header.id = 0; PTR->size = SIZE

// Definition information of a script type
// *int - ptr of_type int
// [32]int - array[32] of_type int
// **int - ptr of_type ptr of_type int
// MyStruct - MyStruct
// MyStruct<int> - MyStruct targ[0] int
struct vmc_type_definition
{
	union
	{
		vmc_type_header header;
		struct
		{
			vmc_type_header_type type;
			vm_uint32 id;
			vm_int32 size;
			vm_string name;
		};
	};

	// Information of this type
	vm_int32 mask;

	// A type can be of another type. Used for typedef
	struct vmc_type_definition* of_type;

	// The package this type exists inside
	struct vmc_package* package;

	// Linked list next
	struct vmc_type_definition* next;
};
typedef struct vmc_type_definition vmc_type_definition;

// No mask
#define VMC_TYPE_DEF_MASK_NONE (0)
// Indicates that the type is an memory address/pointer
#define VMC_TYPE_DEF_MASK_ADDR (1 << 0)

// Create a new type definition
extern vmc_type_definition* vmc_type_definition_new(const vm_string* name, vm_int32 size);

// Create a new type definition of another type
extern vmc_type_definition* vmc_type_definition_of_type(const vm_string* name, vmc_type_definition* parent);

struct vmc_type_definition_castinfo
{
	// Where the memory starts from when an object is casted from one type to another
	vm_int32 offset;
};
typedef struct vmc_type_definition_castinfo vmc_type_definition_castinfo;

// Compare two types to see how they are compatible. 
extern BOOL vmc_type_definition_compare(const vmc_type_definition* from, const vmc_type_definition* to, vmc_type_definition_castinfo* result);

// Declaration of a variable
struct vmc_var
{
	// The definition itself
	vmc_type_definition* definition;

	// Name of the variable
	vm_string name;

	// The offset, from the start, where this var can be found on the stack
	vm_int32 offset;
};
typedef struct vmc_var vmc_var;

// Defines a function to be external - i.e. you can link functionality from 
// your compiled application into the script language
#define VMC_FUNC_MODIFIER_EXTERN (1 << 0)

// The function is public and can be accessed from outside the package
#define VMC_FUNC_MODIFIER_PUBLIC (1 << 1)

// The function has a body
#define VMC_FUNC_MODIFIER_HAS_BODY (1 << 2)

#define VMC_FUNC_MAX_ARGS 9
#define VMC_FUNC_MAX_RETURNS 9
#define VMC_FUNC_MAX_LOCALS 9

// Information of a function
struct vmc_func
{
	union
	{
		vmc_type_header header;
		struct
		{
			vmc_type_header_type type;
			vm_uint32 id;
			vm_int32 size;
			vm_string name;
		};
	};

	// The signature of the function
	vm_string signature;

	// Properties
	vm_bits32 modifiers;

	// Offset where the bytecode for this function is found. Will be -1 if the offset
	// is now known yet. The compiler will try to resolve the offset during the
	// link step
	vm_int32 offset;

	// How complex is this function. Complexity is normally used during the optimization
	// step of the machine code generation
	vm_int32 complexity;
	vm_int32 complexity_components;

	// Arguments
	vmc_var args[VMC_FUNC_MAX_ARGS];
	vm_int32 args_count;
	vm_int32 args_total_size;

	// Returns
	vmc_var returns[VMC_FUNC_MAX_RETURNS];
	vm_int32 returns_count;
	vm_int32 returns_total_size;

	// local variables
	vmc_var locals[VMC_FUNC_MAX_RETURNS];
	vm_int32 locals_count;
	vm_int32 locals_total_size;

	// Memory markers for locations in this function. These are only used for jumps within the function itself
	vmc_linker_memory_marker* memory_marker_first;
	vmc_linker_memory_marker* memory_marker_last;

	// Package this function can be found in
	struct vmc_package* package;

	// Next function
	struct vmc_func* next;
};
typedef struct vmc_func vmc_func;

// Initialize the memory for the supplied function
extern void vmc_func_init(vmc_func* func);

// Destroy memory allocated for the supplied function
extern void vmc_func_free(vmc_func* func);

// Allocate new memory for a potential function. Will return NULL if the system is out of memory
extern vmc_func* vmc_func_malloc();

// Calculate the offset on the stack for arguments, return values and local variables
extern void vmc_func_calculate_offsets(vmc_func* func);

// Check to see if the supplied function is public
static inline BOOL vmc_func_is_public(const vmc_func* func)
{
	return (func->modifiers & VMC_FUNC_MODIFIER_PUBLIC) == VMC_FUNC_MODIFIER_PUBLIC;
}

// Check to see if the supplied function is marked to be external
static inline BOOL vmc_func_is_extern(const vmc_func* func)
{
	return (func->modifiers & VMC_FUNC_MODIFIER_EXTERN) == VMC_FUNC_MODIFIER_EXTERN;
}

// Check to see if the supplied function has a body
static inline BOOL vmc_func_has_body(const vmc_func* func)
{
	return (func->modifiers & VMC_FUNC_MODIFIER_HAS_BODY) == VMC_FUNC_MODIFIER_HAS_BODY;
}

// A package containing functions and variables
struct vmc_package
{
	union
	{
		vmc_type_header header;
		struct
		{
			vmc_type_header_type type;
			vm_uint32 id;
			vm_int32 size;
			vm_string name;
		};
	};

	// The full name (including parent packages) of this package
	vm_string full_name;

	// Linked List functions
	vmc_func* func_first;
	vmc_func* func_last;

	// Number of functions
	vm_uint32 func_count;

	// Linked list types
	vmc_type_definition* type_first;
	vmc_type_definition* type_last;

	// Number of types
	vm_uint32 type_count;

	// Offset where the data information for this package can be found
	vm_uint32 data_offset;

	// Memory markers for functions and global variables
	vmc_linker_memory_marker* memory_marker_first;
	vmc_linker_memory_marker* memory_marker_last;

	// Root package, useful for searching for built-in types
	struct vmc_package* root_package;
};
typedef struct vmc_package vmc_package;

// Allocate memory for a new package with the supplied name
extern vmc_package* vmc_package_malloc(const char* name, int length);

// Free the allocated package memory including everything associated with it
extern void vmc_package_free(vmc_package* p);

// Add the supplied function to the supplied package
extern void vmc_package_add_func(vmc_package* p, vmc_func* f);

// Add the supplied type to the supplied package
extern void vmc_package_add_type(vmc_package* p, vmc_type_definition* type);

// Search for a type in the supplied package
extern vmc_type_definition* vmc_package_find_type(vmc_package* p, const vm_string* name);

// Search for a type in the supplied package
extern vmc_type_definition* vmc_package_find_type_with_parent(vmc_package* p, const vm_string* name, vmc_type_definition* parent);

// Search for a function with the supplied signature
extern vmc_func* vmc_func_find(vmc_package* p, const vm_string* signature);

#endif
