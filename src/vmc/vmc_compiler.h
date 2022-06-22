#ifndef _VMC_COMPILER_H_
#define _VMC_COMPILER_H_

#include "vmc_lexer.h"
#include "../vm_list.h"
#include "../vm_bytestream.h"
#include "vmc_string_pool.h"
#include "vmc_linker.h"
#include "vmc_compiler_types.h"

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
	vm_list packages;

	// Number of functions, in total
	vm_uint32 functions_count;

	// Linker
	vmc_linker linker;

	// Strings
	vmc_string_pool string_pool;
};
typedef struct vmc_compiler vmc_compiler;

struct vmc_parse_scope
{
	vmc_compiler* c;
	vmc_lexer* l;
	vmc_package* p; 
	vmc_lexer_token* t;
	vmc_func* func;
};
typedef struct vmc_parse_scope vmc_parse_scope;

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

#endif