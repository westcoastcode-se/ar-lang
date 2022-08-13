#ifndef _ZPP_COMPILER_H_
#define _ZPP_COMPILER_H_

#include "../vm_message.h"
#include "source_code.h"
#include "symbols.h"
#include "syntax_tree.h"
#include "messages.h"

typedef struct zpp_compiler
{
	// Messages raised by the compiler
	vm_messages messages;
	// A panic message. This is normally used for when the device is out of memory
	vm_message panic;
	// Sources used by the compiler
	zpp_source_code* sources;
	zpp_source_code* sources_end;
	// The root node where everything originates from
	zpp_syntax_tree_root* root_node;
	// Packages generated during the compilation time
	zpp_package* packages;
	zpp_package* packages_end;
	// Pipeline and builder for generating the machine code
	vmp_pipeline* pipeline;
	vmp_builder* builder;
} zpp_compiler;

typedef struct zpp_compiler_state
{
	// The compiler
	zpp_compiler* compiler;
	// The token 
	zpp_token* token;
	// The parent node
	zpp_syntax_tree_node parent_node;
	// The closest parent package node
	zpp_syntax_tree_package* package_node;
	// The closest parent function node
	zpp_syntax_tree_func* func_node;
	// The closest parent type node
	zpp_syntax_tree_type* type_node;
} zpp_compiler_state;

// Create a new compiler for the z++ language
extern zpp_compiler* zpp_compiler_new();

// Destroy the compiler
extern void zpp_compiler_destroy(zpp_compiler* ptr);

// Add new source code. Returns FALSE if an message is raised that's considered an ERROR
extern BOOL zpp_compiler_add_source_code(zpp_compiler* c, zpp_source_code* source);

// Raise a compiler message
extern void zpp_compiler_raise_message(zpp_compiler* c, vm_message* msg);

// Compile the syntax tree into bytecode
extern vm_byte* zpp_compiler_compile(zpp_compiler* c);

#endif
