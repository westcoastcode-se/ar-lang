#ifndef _VMC_COMPILER_H_
#define _VMC_COMPILER_H_

#include "vm_lexer.h"
#include "../vm_bytestream.h"
#include "vm_error.h"
#include "../vm_string.h"

struct vmc_compiler;

// High INT8 (the four last bits)
#define VMC_PROPS_HIGH(P) ((P) << 4)
// Low INT8 (the four first bits)
#define VMC_PROPS_LOW(P) ((P))

#define VMC_PROPS1_OPCODE(P) ((P) << 8)
#define VMC_PROPS2_OPCODE(P) ((P) << 16)
#define VMC_PROPS3_OPCODE(P) ((P) << 24)

enum vmc_emit_types
{
	VMC_EMIT_TYPE_I8 = 0,
	VMC_EMIT_TYPE_I16,
	VMC_EMIT_TYPE_I32,
	VMC_EMIT_TYPE_I64,
	VMC_EMIT_TYPE_F32,
	VMC_EMIT_TYPE_F64,
};

enum vmc_emit_icodes
{
	VMC_EMIT_VARS = 1,
	VMC_EMIT_LDARG,
	VMC_EMIT_LDCONST,
	VMC_EMIT_CONV,
	VMC_EMIT_ADD,
	VMC_EMIT_RET,
};
typedef enum vmc_emit_icodes vmc_emit_icodes;

enum vmc_types
{
	VMC_TYPES_PRIMITIVE,
	VMC_TYPES_STRUCT,
	VMC_TYPES_INTERFACE
};
typedef enum vmc_types vmc_types;

// Represents a type
struct vmc_type_header
{
	// The name of the type
	vm_string name;

	// The size of the type
	int size;

	// Type
	vmc_types type;
};
typedef struct vmc_type vmc_type;

// A namespace definition
struct vmc_namespace
{
	// Name of the namespace
	vm_string name;

	//
	// Linked list
	//

	struct vmc_namespace* next;
};
typedef struct vmc_namespace vmc_namespace;

// Represents a type
struct vmc_type
{
	// The name of the type
	vm_string name;

	// The size of the type
	int size;

	// Namespace this type is part of
	vmc_namespace* ns;
};
typedef struct vmc_type vmc_type;

// A struct definition
struct vmc_struct
{
	// Name of the struct type
	vm_string name;

	// Size of the struct
	vm_int32 size;

	// Namespace this struct is part of
	vmc_namespace* ns;

	// The first field part of this struct
	struct vmc_field* first_field;

	// The last field part of this struct
	struct vmc_field* last_field;

	//
	// Linked list
	//

	struct vmc_struct* next;
};
typedef struct vmc_struct vmc_struct;

// Represents a field inside a struct
struct vmc_field
{
	// Name of the field
	vm_string name;

	// The type this field is part of
	const vmc_type* type;

	//
	// Linked list
	//

	struct vmc_field* next;
};
typedef struct vmc_field vmc_field;

// A function
struct vmc_def_func
{
	// Name of the function
	vm_string name;
};
typedef struct vmc_def_func vmc_def_func;

// An argument sent to a function
struct vmc_def_arg
{
	// Name of the function
	vm_string name;

	// Type of the argument
	vmc_type type;
};
typedef struct vmc_def_arg vmc_def_arg;

struct vmc_compiler
{
	vm_lexer* lexer;

	// Where compiled bytecode is put into
	vm_bytestream bytecode;
	
	// Messages raised by the compiler
	vm_message* messages;
	vm_message* last_message;
};
typedef struct vmc_compiler vmc_compiler;

// Create a new compiler
extern vmc_compiler* vmc_compiler_new(vm_lexer* l);

// Compile 
extern BOOL vmc_compiler_compile(vmc_compiler* c);

// Compile the supplied source code
extern vmc_compiler* vmc_compiler_compile_string(const vm_byte* src);

// Destroy the supplied compiler
extern void vmc_compiler_destroy(vmc_compiler* c);

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
	return c->messages == NULL;
}

// Get the bytecode created by the supplied compiler
inline static vm_byte* vmc_compiler_bytecode(vmc_compiler* c)
{
	return c->bytecode.memory;
}

#endif
