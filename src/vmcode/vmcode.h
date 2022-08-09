#ifndef _VMCODE_H_
#define _VMCODE_H_

#include "../vmp/vmp.h"
#include "vmcd_lexer.h"
#include "vmcd_list_sources.h"
#include "vmcd_lists.h"
#include "../vm_string_pool.h"

// Source Code Processor
struct vmcode
{
	// A string pool
	vm_string_pool string_pool;

	// Source codes to be processed
	vmcd_list_sources source_codes;

	// All packages
	vmcd_list_packages packages;

	// Bytecode builders
	vmp_pipeline* pipeline;
	vmp_builder* builder;

	// Messages raised by the processor
	vm_messages messages;

	// If a panic error has occurred, such as if the computer is out of memory
	vm_message panic_error_message;
};
typedef struct vmcode vmcode;

// The compilation scope
typedef struct vmcd_scope
{
	vmcode* vmcd;
	vmp_pipeline* pipeline;
	vmcd_token* token;

	// Current package we are in
	struct vmcd_package* package;

	// The function we are in
	struct vmcd_func* func;
	
	// Parent function
	const struct vmcd_scope* parent;
} vmcd_scope;

// A parser responsible for parsing source code and building up a syntax tree from it
typedef struct vmcd_parser
{
	int val;
} vmcd_parser;

// Create a new source code processor
extern vmcode* vmcode_new();

// Destroy the memory of the source code processor
extern void vmcode_destroy(vmcode* p);

// Add source code
extern BOOL vmcode_add_source_code(vmcode* p, const vm_byte* source_code, const vm_byte* filename);

// Parse the supplied source code. The supplied filename points to the entrypoint for where the source code actually
// starts to be parsed
extern BOOL vmcode_parse(vmcode* p, const vm_byte* filename);

// Link the compiled source code
extern BOOL vmcode_link(vmcode* p);

// Get the compiled bytecode from the compiler
extern const vm_byte* vmcode_get_bytecode(vmcode* p);

#endif
