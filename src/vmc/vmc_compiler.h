#ifndef _VMC_COMPILER_H_
#define _VMC_COMPILER_H_

#include "vmc_lexer.h"
#include "vmc_error.h"
#include "../vm_bytestream.h"
#include "../vm_string.h"

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

	// The offset, from the start, where this var can be found
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

// Information of a function
struct vmc_func
{
	// Name of the function
	vm_string name;

	// Is this function an external function
	BOOL is_extern;

	// Is this function public
	BOOL is_public;

	// Offset where the bytecode for this function is found
	vm_int32 offset;

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
	// The short name of this package
	vm_string name;

	// The full name (including parent packages) of this package
	vm_string full_name;

	// Linked List functions
	vmc_func* func_first;
	vmc_func* func_last;

	// Linked list types
	vmc_type_definition* type_first;
	vmc_type_definition* type_last;

	// Root package, useful for searching for built-in types
	struct vmc_package* root_package;

	// Linked List next
	struct vmc_package* next;
};
typedef struct vmc_package vmc_package;

// Everything available where the compiler is located right now
struct vmc_scope
{
	// The parent scope
	struct vmc_scope* parent;

	// Free-scope linked list
	struct vmc_scope* next;
	struct vmc_scope* prev;
};
typedef struct vmc_scope vmc_scope;

// Entry point for the compiler
struct vmc_compiler
{
	// Configuration used by the compiler
	vmc_compiler_config config;

	// Where compiled bytecode is put into
	vm_bytestream bytecode;

	// Messages raised by the compiler
	vm_message* messages_first;
	vm_message* messages_last;

	// Scopes
	vmc_scope* scope_first;
	vmc_scope* scope_last;

	// Packages
	vmc_package* package_first;
	vmc_package* package_last;
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

// Create a new scope
extern vmc_scope* vmc_scope_push(vmc_compiler* c, vmc_scope* parent_scope);

// Return the scope to be reused by someone else. Returns the parent
extern vmc_scope* vmc_scope_pop(vmc_compiler* c, vmc_scope* scope);

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

// Check to see if the compiler has compiled successfully
inline static BOOL vmc_compiler_success(vmc_compiler* c)
{
	return c->messages_first == NULL;
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

// Try to create a new function. Returns a code that tells us if it was successful or not
#define VMC_FUNC_TRY_CREATE_OK (0)
#define VMC_FUNC_TRY_CREATE_OOM (1)
#define VMC_FUNC_TRY_CREATE_ALREADY_EXISTS (2)
extern int vmc_func_try_create(vmc_package* p, const vm_string* name, vm_int32 offset, vmc_func** func);

// Try to func a function in the supplied package
extern vmc_func* vmc_func_find(vmc_package* p, const vm_string* name);

// Create a new function
extern vmc_func* vmc_func_new(vmc_package* p, const vm_string* name, vm_int32 offset);

// Add a new type to a package
extern vmc_type_definition* vmc_type_definition_new(vmc_package* p, const vm_string* name, vm_int32 size);

#endif