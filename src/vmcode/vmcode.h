#ifndef _VMCODE_H_
#define _VMCODE_H_

#include "../vmp/vmp.h"
#include "vmcd_lexer.h"

// Source Code Processor
struct vmcode
{
	vmp_pipeline* pipeline;

	// Messages raised by the processor
	vm_messages messages;
	// If a panic error has occurred, such as if the computer is out of memory
	vm_message panic_error_message;
};
typedef struct vmcode vmcode;

// The compilation scope
struct vmcd_scope
{
	vmcode* vmcd;
	vmp_pipeline* pipeline;
	vmcd_token* token;

	// Current package we are in
	vmp_package* package;

	// The function we are in
	vmp_func* func;
	
	// Parent function
	const struct vmcd_scope* parent;
};
typedef struct vmcd_scope vmcd_scope;

// Create a new source code processor
extern vmcode* vmcode_new();

// Destroy the memory of the source code processor
extern void vmcode_destroy(vmcode* p);

// Parse the supplied source code
extern BOOL vmcode_parse(vmcode* p, const vm_byte* source_code);

// Link the compiled source code
extern BOOL vmcode_link(vmcode* p);

// Get the compiled bytecode from the compiler
extern const vm_byte* vmcode_get_bytecode(vmcode* p);

#endif
