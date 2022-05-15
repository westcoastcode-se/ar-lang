#ifndef _VMC_COMPILER_H_
#define _VMC_COMPILER_H_

#include "vmc_lexer.h"
#include "../vm_bytestream.h"
#include "vmc_string_pool.h"

enum vmc_type_header_type
{
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

// Initialize object header
#define VMC_INIT_TYPE_HEADER(PTR, TYPE, SIZE) PTR->header.type = TYPE; PTR->header.id = 0; PTR->size = SIZE

// Type definition
struct vmc_type_definition
{
	// Name of the type
	vm_string name;

	// Size of this type
	vm_int32 size;

	// The package this type exists inside
	struct vmc_package* package;

	// Linked list next
	struct vmc_type_definition* next;
};
typedef struct vmc_type_definition vmc_type_definition;

#define VMC_TYPE_INFO_MASK_NONE (0)
#define VMC_TYPE_INFO_MASK_ADDR (1 << 0)

// Type definition
struct vmc_type_info
{
	vmc_type_definition* definition;
	vm_int32 masks;
	
	// The size this type takes in memory
	vm_int32 size;

	// The offset, from the start, where this var can be found on the stack
	vm_int32 offset;
};
typedef struct vmc_type_info vmc_type_info;

// Declaration of a variable
struct vmc_var_definition
{
	// Name of the variable
	vm_string name;

	// Variable type definition
	vmc_type_info type;
};
typedef struct vmc_var_definition vmc_var_definition;

// Defines a function to be external - i.e. you can link functionality from 
// your compiled application into the script language
#define VMC_FUNC_MODIFIER_EXTERN (1 << 0)

// The function is public and can be accessed from outside the package
#define VMC_FUNC_MODIFIER_PUBLIC (1 << 1)

// The function has a body
#define VMC_FUNC_MODIFIER_HAS_BODY (1 << 2)

// Information of a function
struct vmc_func
{
	// Type header
	union
	{
		vmc_type_header header;
		struct
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
	vmc_var_definition args[9];
	vm_int32 args_count;
	vm_int32 args_total_size;

	// Returns
	vmc_var_definition returns[9];
	vm_int32 returns_count;
	vm_int32 returns_total_size;

	// Package this function can be found in
	struct vmc_package* package;

	// Next function
	struct vmc_func* next;
};
typedef struct vmc_func vmc_func;

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

// Definition for what the import function looks like. Returns a success if the import was successful
typedef vm_int32(*vmc_compiler_import_fn)(struct vmc_compiler* c, const vm_string* path);

// Configuration on how 
struct vmc_compiler_config
{
	// User data. Can be used to attach extra, unmanaged, data to the compiler
	void* user_data;

	// Import function used when compiling the source code
	vmc_compiler_import_fn import_fn;
};
typedef struct vmc_compiler_config vmc_compiler_config;

// The default configuration
extern const vmc_compiler_config* vmc_compiler_config_default;

// A package containing functions and variables
struct vmc_package
{
	// Type header
	union
	{
		vmc_type_header header;
		struct
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

	// Root package, useful for searching for built-in types
	struct vmc_package* root_package;

	// Linked List next
	struct vmc_package* next;
};
typedef struct vmc_package vmc_package;

// Entry point for the compiler
struct vmc_compiler
{
	// Configuration used by the compiler
	vmc_compiler_config config;

	// Where compiled bytecode is put into
	vm_bytestream bytecode;

	// Messages raised by the compiler
	vm_messages messages;

	// If a panic error has occurred, such as if the computer is out of memory
	vm_message panic_error_message;

	// Packages
	vmc_package* package_first;
	vmc_package* package_last;

	// Number of packages collected
	vm_uint32 packages_count;

	// Number of functions, in total
	vm_uint32 functions_count;

	vmc_string_pool string_pool;

};
typedef struct vmc_compiler vmc_compiler;

// Create a new compiler. If the configuration is NULL then the default configuration will be used. Will return
// NULL if computer is out of memory
extern vmc_compiler* vmc_compiler_new(const vmc_compiler_config* config);

// Destroy the supplied compiler
extern void vmc_compiler_destroy(vmc_compiler* c);

// Compile the supplied source code. The supplied source code is considered the "entry" point
// of the application
extern BOOL vmc_compiler_compile(vmc_compiler* c, const vm_byte* src);

// Write data to the memory stream used by the compiler
inline static vm_byte* vmc_write(vmc_compiler* c, void* bytes, vm_int32 size)
{
	return vm_bytestream_write(&c->bytecode, bytes, size);
}

// Write data a null character
inline static void vmc_write_null(vmc_compiler* c)
{
	vm_bytestream_write_null(&c->bytecode);
}

// Write an integer to the memory stream used by the compiler
inline static void vmc_write_int32(vmc_compiler* c, vm_int32 value)
{
	vm_bytestream_write(&c->bytecode, &value, sizeof(vm_int32));
}

// Check to see if the compiler is panicing
inline static BOOL vmc_compiler_is_panicing(vmc_compiler* c)
{
	return c->panic_error_message.code != 0;
}

// Check to see if the compiler has compiled successfully
inline static BOOL vmc_compiler_success(vmc_compiler* c)
{
	return !vm_messages_has_messages(&c->messages) && !vmc_compiler_is_panicing(c);
}

// Get the bytecode created by the supplied compiler
inline static vm_byte* vmc_compiler_bytecode(vmc_compiler* c)
{
	return c->bytecode.memory;
}

// The default function used to import a file based on the import statement
extern vm_int32 vmc_compiler_config_import(vmc_compiler* c, const vm_string* path);

// Create a new package
extern vmc_package* vmc_package_new(vmc_compiler* c, const char* name, int length);

// Search for a type
extern vmc_type_definition* vmc_package_find_type(vmc_package* p, const vm_string* name);

// Try to func a function in the supplied package with a specific signature
extern vmc_func* vmc_func_find(vmc_package* p, const vm_string* signature);

// Add a new type to a package
extern vmc_type_definition* vmc_type_definition_new(vmc_package* p, const vm_string* name, vm_int32 size);

#endif