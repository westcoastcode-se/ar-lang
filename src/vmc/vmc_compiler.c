#include "vmc_compiler.h"
#include "vmc_lexer_logic.h"
#include "vmc_lexer_math.h"
#include "vmc_compiler_messages.h"
#include "../interpreter/vmi_ops.h"
#include "../interpreter/vmi_process.h"
#include "vmc_debug.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

//
// PRIVATE ///////////////////////////////////////////////////////////////////////////
// 

#define BODY_BRANCH_BEGIN if(0) {}
#define BODY_BRANCH_END
#define BODY_BRANCH(C) else if (vm_string_cmp(&t->string, VM_STRING_CONST_GET(C))) { if (!parse_##C(s, func)) return FALSE; }

VM_STRING_CONST(bool, "bool", 4);
VM_STRING_CONST(int8, "int8", 4);
VM_STRING_CONST(uint8, "uint8", 5);
VM_STRING_CONST(int16, "int16", 5);
VM_STRING_CONST(uint16, "uint16", 6);
VM_STRING_CONST(int32, "int32", 5);
VM_STRING_CONST(uint32, "uint32", 6);
VM_STRING_CONST(int64, "int64", 5);
VM_STRING_CONST(uint64, "uint64", 6);
VM_STRING_CONST(float32, "float32", 7);
VM_STRING_CONST(float64, "float64", 7);
VM_STRING_CONST(ptr, "ptr", 3);

VM_STRING_CONST(lda, "lda", 3);
VM_STRING_CONST(lda_a, "lda_a", 5);
VM_STRING_CONST(str, "str", 3);

VM_STRING_CONST(allocs, "allocs", 6);
VM_STRING_CONST(frees, "frees", 5);
VM_STRING_CONST(alloch, "alloch", 6);
VM_STRING_CONST(freeh, "freeh", 5);

VM_STRING_CONST(ret, "ret", 3);
VM_STRING_CONST(call, "call", 4);

VM_STRING_CONST(args, "args", 4);
VM_STRING_CONST(locals, "locals", 6);
VM_STRING_CONST(ldl, "ldl", 3);
VM_STRING_CONST(ldl_a, "ldl_a", 5);
VM_STRING_CONST(stl, "stl", 3);
VM_STRING_CONST(copy_s, "copy_s", 6);

VM_STRING_CONST(sturef, "sturef", 6);
VM_STRING_CONST(sturef_s, "sturef_s", 8);
VM_STRING_CONST(sturef_s_i1, "sturef_s_i1", 11);
VM_STRING_CONST(sturef_s_i2, "sturef_s_i2", 11);
VM_STRING_CONST(sturef_s_i4, "sturef_s_i4", 11);
VM_STRING_CONST(sturef_s_i8, "sturef_s_i8", 11);
VM_STRING_CONST(sturef_s_f4, "sturef_s_f4", 11);
VM_STRING_CONST(sturef_s_f8, "sturef_s_f8", 11);

VM_STRING_CONST(stelem, "stelem", 6);
VM_STRING_CONST(stelem_s, "stelem_s", 8);

VM_STRING_CONST(ldelem, "ldelem", 6);
VM_STRING_CONST(ldelem_s, "ldelem_s", 8);

VM_STRING_CONST(ldc_s_i1, "ldc_s_i1", 8);
VM_STRING_CONST(ldc_s_i2, "ldc_s_i2", 8);
VM_STRING_CONST(ldc_s_i4, "ldc_s_i4", 8);
VM_STRING_CONST(ldc_s_i8, "ldc_s_i8", 8);
VM_STRING_CONST(ldc_s_f4, "ldc_s_f4", 8);
VM_STRING_CONST(ldc_s_f8, "ldc_s_f8", 8);

VM_STRING_CONST(ldc_i1, "ldc_i1", 6);
VM_STRING_CONST(ldc_i2, "ldc_i2", 6);
VM_STRING_CONST(ldc_i4, "ldc_i4", 6);
VM_STRING_CONST(ldc_i8, "ldc_i8", 6);
VM_STRING_CONST(ldc_f4, "ldc_f4", 6);
VM_STRING_CONST(ldc_f8, "ldc_f8", 6);

VM_STRING_CONST(conv_i2_i4, "conv_i2_i4", 10);
VM_STRING_CONST(conv_i4_i2, "conv_i4_i2", 10);

VM_STRING_CONST(clt, "clt", 3);
VM_STRING_CONST(cgt, "cgt", 3);
VM_STRING_CONST(jmpt, "jmpt", 4);

VM_STRING_CONST(add, "add", 3);

VM_STRING_CONST(empty, "", 0);

const vmc_compiler_config _vmc_compiler_config_default = {
	NULL,
	&vmc_compiler_config_import
};

void _vmc_emit_opcode(const vmc_compiler_scope* s, vm_int32 opcode)
{
	vmi_instr_single_instruction instr;
	instr.header.opcode = opcode;
	vmc_write(s->compiler, &instr, sizeof(vmi_instr_single_instruction));
}

void _vmc_emit_icode(const vmc_compiler_scope* s, vm_int8 icode)
{
	vmi_instr_single_instruction instr;
	instr.header.opcode = 0;
	instr.header.icode = icode;
	vmc_write(s->compiler, &instr, sizeof(vmi_instr_single_instruction));
}

void _vmc_append_header(vmc_compiler* c)
{
	vmi_process_header header;
	header.header[0] = 'V';
	header.header[1] = 'M';
	header.header[2] = '0';
	header.version = VM_VERSION;
	header.data_offset = 0;
	header.code_offset = sizeof(vmi_process_header);
	header.packages_count = 0;
	header.functions_count = 0;
	header.first_package_offset = 0;
	vmc_write(c, &header, sizeof(header));
}

// Parse a type:
// pointer of_type KEYWORD
// ARRAY of_type KEYWORD
// pointer of_type ARRAY of_type KEYWORD
// ARRAY of_type POINTER of_type KEYWORD
// ...
// 
// KEYWORD
// *KEYWORD
// [INT]KEYWORD
// *[INT]KEYWORD
// [INT]*KEYWORD
// *[INT]*KEYWORD
// KEYWPORD<TYPE>
// *PACKAGE.KEYWORD
BOOL _vmc_parse_type(const vmc_compiler_scope* s, vmc_var* var) {
	vmc_compiler* const c = s->compiler;
	vmc_lexer_token* const t = s->token;
	vmc_type_header* header;

	// Is the type a pointer or an array?
	if (t->type == VMC_LEXER_TYPE_PTR) {
		char memory[128];
		const vm_string* pointer_type_name;
		vmc_type_header* pointer_type_header;
		vmc_lexer_next(t);
		if (!_vmc_parse_type(s, var)) {
			return FALSE;
		}

		// Stringify the type
		memory[0] = '*';
		vm_str_cpy(&memory[1], var->definition->name.start, vm_string_length(&var->definition->name));

		// Try find the "pointer" type. If not found then add it to it's owner package
		pointer_type_name = vmc_string_pool_stringsz(&c->string_pool, memory, vm_string_length(&var->definition->name) + 1);
		if (pointer_type_name == NULL) {
			return vmc_compiler_message_panic(s, "out of memory");
		}

		// Reuse the type if found, otherwise create it and add it to the package
		pointer_type_header = vmc_package_find(var->definition->package, pointer_type_name);
		if (pointer_type_header == NULL) {
			// Doesnt exist, so let's create it
			vmc_package* package = var->definition->package;
			var->definition = vmc_type_definition_of_type(pointer_type_name, var->definition);
			vmc_package_add_type(package, var->definition);
		}
		else if (pointer_type_header->type != VMC_TYPE_HEADER_TYPE) {
			return vmc_compiler_message_expected_type(&c->messages, t);			
		}
		else {
			var->definition = (vmc_type_definition*)pointer_type_header;
		}
		var->definition->size = sizeof(void*);
		return TRUE;
	}
	else if (t->type == VMC_LEXER_TYPE_SQUARE_L) {
		char memory[128];
		char* memory_ptr;
		const vm_string* array_type_name;
		vmc_type_header* array_type_header;
		vm_int32 size;
		vmc_lexer_token temp_token;
		vmc_lexer_next(t);
		if (t->type != VMC_LEXER_TYPE_INT) {
			// TODO: Add support for constants containing the integer size
			if (t->type == VMC_LEXER_TYPE_KEYWORD) {
				return vmc_compiler_message_not_implemented(&c->messages, t);
			}
			return vmc_compiler_message_expected_int(s);
		}

		size = vmc_lexer_token_toint32(t);
		temp_token = *t;
		vmc_lexer_next(t);
		if (t->type != VMC_LEXER_TYPE_SQUARE_R) {
			return vmc_compiler_message_syntax_error(&c->messages, t, ']');
		}
		vmc_lexer_next(t);
		
		// We now have a known array size of the upcoming type
		if (!_vmc_parse_type(s, var)) {
			return FALSE;
		}

		// Stringify the type
		memory[0] = '[';
		memory_ptr = vm_str_cpy(&memory[1], temp_token.string.start, vm_string_length(&temp_token.string));
		*memory_ptr++ = ']';
		memory_ptr = vm_str_cpy(memory_ptr, var->definition->name.start, vm_string_length(&var->definition->name));

		// Try find the "pointer" type. If not found then add it to it's owner package
		array_type_name = vmc_string_pool_stringsz(&c->string_pool, memory, (int)(memory_ptr - memory));
		if (array_type_name == NULL) {
			return vmc_compiler_message_panic(s, "out of memory");
		}

		// Reuse the type if found, otherwise create it and add it to the package
		array_type_header = vmc_package_find(var->definition->package, array_type_name);
		if (array_type_header == NULL) {
			// Doesnt exist, so let's create it
			vmc_package* package = var->definition->package;
			var->definition = vmc_type_definition_of_type(array_type_name, var->definition);
			vmc_package_add_type(package, var->definition);
			var->definition->size *= size;
		}
		else if (array_type_header->type != VMC_TYPE_HEADER_TYPE) {
			return vmc_compiler_message_expected_type(&c->messages, t);
		}
		else {
			var->definition = (vmc_type_definition*)array_type_header;
		}
		return TRUE;
	}
	
	if (t->type != VMC_LEXER_TYPE_KEYWORD) {
		return vmc_compiler_message_expected_type(&c->messages, t);
	}

	header = vmc_package_find(s->package, &t->string);
	if (header == NULL) {
		return vmc_compiler_message_type_not_found(&c->messages, t);
	}

	while (header->type == VMC_TYPE_HEADER_IMPORT_ALIAS) {
		vmc_import_alias* const alias = (vmc_import_alias*)header;
		// Dot delimiter
		vmc_lexer_next(t);
		if (t->type != VMC_LEXER_TYPE_DOT) {
			return vmc_compiler_message_syntax_error(&c->messages, t, '.');
		}
		// Next keyword
		vmc_lexer_next(t);
		if (t->type != VMC_LEXER_TYPE_KEYWORD) {
			return vmc_compiler_message_expected_type(&c->messages, t);
		}

		header = vmc_package_find(alias->package, &t->string);
	}

	if (header->type != VMC_TYPE_HEADER_TYPE) {
		return vmc_compiler_message_expected_type(&c->messages, t);
	}

	var->definition = (vmc_type_definition*)header;
	return TRUE;
}

BOOL _vmc_compiler_parse_type_decl_without_name(const vmc_compiler_scope* s,
	vmc_var* vars, vm_int32* out_count, vm_int32* out_total_size)
{
	vmc_compiler* const c = s->compiler;
	vmc_package* const p = s->package;
	vmc_lexer_token* const t = s->token;

	vm_int32 count = 0;
	vm_int32 total_size = 0;

	// Expected a '(' token
	if (!vmc_lexer_next_type(t, VMC_LEXER_TYPE_PARAN_L))
		return vmc_compiler_message_syntax_error(&c->messages, t, '(');

	// Parse until we reach that end ')' token
	while (!vmc_lexer_next_type(t, VMC_LEXER_TYPE_PARAN_R)) {
		vmc_var* const var = &vars[count];

		// Ignore comma
		if (t->type == VMC_LEXER_TYPE_COMMA) {
			vmc_lexer_next(t);
		}

		// Reset masks
		var->offset = 0;
		var->name = *VM_STRING_CONST_GET(empty);
		
		// Parse type
		if (!_vmc_parse_type(s, var)) {
			return FALSE;
		}

		// Argument now loaded
		count++;
		total_size += var->definition->size;
	}

	*out_count = count;
	*out_total_size = total_size;
	return TRUE;
}

// fn (<scope>) Name (<args>) (<returns>)
BOOL _vmc_parse_keyword_fn_locals(const vmc_compiler_scope* s, vmc_func* func)
{
	vmc_lexer_token* const t = s->token;
	vmc_compiler* const c = s->compiler;
	vmc_package* const p = s->package;

	func->locals_count = 0;
	func->locals_total_size = 0;

	// Expected a '(' token
	if (!vmc_lexer_next_type(t, VMC_LEXER_TYPE_PARAN_L))
		return vmc_compiler_message_syntax_error(&c->messages, t, '(');

	// Parse until we reach that end ')' token
	while (!vmc_lexer_next_type(t, VMC_LEXER_TYPE_PARAN_R)) {
		vmc_var* const var = &func->locals[func->locals_count];

		// Ignore comma
		if (t->type == VMC_LEXER_TYPE_COMMA) {
			vmc_lexer_next(t);
		}

		// Reset masks
		var->offset = 0;

		// Read var name
		if (t->type != VMC_LEXER_TYPE_KEYWORD)
			return vmc_compiler_message_expected_identifier(s);
		var->name = t->string;
		vmc_lexer_next(t);

		// Parse type
		if (!_vmc_parse_type(s, var)) {
			return FALSE;
		}

		// Argument now loaded
		func->locals_count++;
		func->locals_total_size += var->definition->size;
	}

	// Augment offsets (where is the memory located from the "ebp"'s point of view).
	// EBP points to where the stack is located 
	if (func->locals_count > 0) {
		vm_int32 i;
		vm_int32 offset = sizeof(vmi_ip) + sizeof(vm_byte*) + func->args_total_size + func->returns_total_size;
		for (i = 0; i < func->locals_count; ++i) {
			vmc_var* info = &func->locals[i];
			info->offset = offset;
			offset += info->definition->size;
		}
	}

	return TRUE;
}

// Parse optional argument names for the current function
// args (name1, name2, ...)
BOOL _vmc_parse_function_arg_names(const vmc_compiler_scope* s, vmc_func* func)
{
	vmc_lexer_token* const t = s->token;
	vmc_compiler* const c = s->compiler;
	vmc_package* const p = s->package;

	// Expected a (
	if (!vmc_lexer_next_type(t, VMC_LEXER_TYPE_PARAN_L))
		return vmc_compiler_message_syntax_error(&c->messages, t, '(');
	
	// Read each argument name
	for (int i = 0; i < func->args_count; ++i) {
		vmc_var* const arg = &func->args[i];
		if (!vmc_lexer_next_type(t, VMC_LEXER_TYPE_KEYWORD)) {
			return vmc_compiler_message_expected_keyword(&c->messages, t);
		}
		arg->name = t->string;
		// Ignore comma (or last ')'
		vmc_compiler_next(s);
	}

	// Expected a )
	if (t->type != VMC_LEXER_TYPE_PARAN_R)
		return vmc_compiler_message_syntax_error(&c->messages, t, ')');
	return TRUE;
}

BOOL _vmc_parse_keyword_fn_args(const vmc_compiler_scope* s, vmc_func* func)
{
	return _vmc_compiler_parse_type_decl_without_name(s, func->args, &func->args_count, &func->args_total_size);
}

BOOL _vmc_parse_keyword_fn_rets(const vmc_compiler_scope* s, vmc_func* func)
{
	return _vmc_compiler_parse_type_decl_without_name(s, func->returns, &func->returns_count, &func->returns_total_size);
}

vmc_linker_marker_addr* _vmc_func_add_marker_addr(vmc_compiler* c, vmc_func* func, const vm_string* name)
{
	vmc_linker_marker_addr* marker = vmc_linker_marker_addr_search(func->marker_local_addr_first, name);
	if (marker != NULL) {
		return marker;
	}
	marker = vmc_linker_marker_addr_alloc(name);
	if (marker == NULL)
		return NULL;
	if (func->marker_local_addr_first == NULL)
		func->marker_local_addr_first = func->marker_local_addr_last = marker;
	else {
		func->marker_local_addr_last->next = marker;
		func->marker_local_addr_last = marker;
	}
	return marker;
}

BOOL _vmc_func_add_unique_marker_addr(vmc_compiler* c, vmc_func* func, const vm_string* name)
{
	vmc_linker_marker_addr* marker = _vmc_func_add_marker_addr(c, func, name);
	if (marker != NULL) {
		// If the marker exists and is already set then this is considered an error
		if (marker->offset != 0) {
			return FALSE;
		}
		marker->offset = vm_bytestream_get_size(&c->bytecode);
	}
	return marker != NULL;
}

// Parse the source code for a signature
BOOL _vmc_seek_func_signature(vmc_compiler* c, vmc_lexer_token* t, vm_string* signature)
{
	int num_params;
	
	// Seek until we've reached the end param for the args
	signature->start = t->string.start;
	if (!vmc_lexer_next_type(t, VMC_LEXER_TYPE_PARAN_L)) {
		return vmc_compiler_message_syntax_error(&c->messages, t, '(');
	}
	num_params = 1;
	while (num_params > 0) {
		vmc_lexer_next(t);
		if (t->type == VMC_LEXER_TYPE_PARAN_L)
			num_params++;
		else if (t->type == VMC_LEXER_TYPE_PARAN_R)
			num_params--;
		else if (t->type == VMC_LEXER_TYPE_NEWLINE)
			return vmc_compiler_message_unexpected_newline(&c->messages, t);
	}

	// Seek until we've reached the end param for the returns
	if (!vmc_lexer_next_type(t, VMC_LEXER_TYPE_PARAN_L)) {
		return vmc_compiler_message_syntax_error(&c->messages, t, '(');
	}
	num_params = 1;
	while (num_params > 0) {
		vmc_lexer_next(t);
		if (t->type == VMC_LEXER_TYPE_PARAN_L)
			num_params++;
		else if (t->type == VMC_LEXER_TYPE_PARAN_R)
			num_params--;
		else if (t->type == VMC_LEXER_TYPE_NEWLINE)
			return vmc_compiler_message_unexpected_newline(&c->messages, t);
	}
	signature->end = t->string.end;
	return TRUE;
}

// Parse the function signature:
// [name:keyword]([type1:keyword],...)([type1:keyword])
BOOL _vmc_parse_func_signature(const vmc_compiler_scope* s, vmc_func* func)
{
	vmc_lexer_token* const t = s->token;

	// 1. Function name (The signature starts with the name and ends with the last return value)
	if (!vmc_lexer_next_type(t, VMC_LEXER_TYPE_KEYWORD)) {
		vmc_compiler_message_expected_identifier(s);
		return FALSE;
	}

	func->name = t->string;
	if (vmc_lexer_test_uppercase(*func->name.start))
		func->modifiers |= VMC_FUNC_MODIFIER_PUBLIC;

	// Parse args
	if (!_vmc_parse_keyword_fn_args(s, func))
		return FALSE;

	// Parse return types
	if (!_vmc_parse_keyword_fn_rets(s, func))
		return FALSE;

	// Figure out the signature
	vm_string_setsz(&func->signature, func->name.start, (vm_int32)(t->string.end - func->name.start));
	return TRUE;
}

#include "vmc_compiler_instr.inc.c"

BOOL _vmc_parse_keyword_fn_body(const vmc_compiler_scope* s, vmc_func* func)
{
	vmc_compiler* const c = s->compiler;
	vmc_lexer_token* const t = s->token;
	vmc_package* const p = s->package;

	// External functions are not allowed to have a body
	if (vmc_func_is_extern(func))
		return TRUE;

	// Expected a {
	if (!vmc_lexer_next_type(t, VMC_LEXER_TYPE_BRACKET_L))
		return vmc_compiler_message_syntax_error(&c->messages, t, '{');
	vmc_lexer_next(t);

	// Body is now definde and we know where the body bytecode starts
	func->offset = vm_bytestream_get_size(&c->bytecode);

	// The function is actually beginning here
	// TODO: Add support for reserving memory for local variables larger than 65535 bytes
	if ((vm_uint32)(func->args_total_size + func->returns_total_size) > UINT16_MAX)
		return vmc_compiler_message_not_implemented(&c->messages, t);

	while (1) {
		// Unexpected end of function body
		if (t->type == VMC_LEXER_TYPE_EOF) {
			return vmc_compiler_message_unexpected_eof(s);
		}

		// Break if we've reached the end of the function
		if (t->type == VMC_LEXER_TYPE_BRACKET_R) {
			break;
		}

		// Ignore comments
		if (t->type == VMC_LEXER_TYPE_COMMENT) {
			vmc_lexer_next(t);
			continue;
		}

		// Memory markers for jumps
		if (t->type == VMC_LEXER_TYPE_HASH) {
			// Get the keyword
			vmc_lexer_next(t);
			if (!vmc_lexer_type_is_keyword(t->type))
				return vmc_compiler_message_expected_keyword(&c->messages, t);
			if (!_vmc_func_add_unique_marker_addr(c, func, &t->string))
				return vmc_compiler_message_memory_marker_exists(&c->messages, t, &t->string);
			vmc_lexer_next(t);
			continue;
		}

		// Verify that a keyword is being processed
		if (!vmc_lexer_type_is_keyword(t->type)) {
			return vmc_compiler_message_expected_keyword(&c->messages, t);
		}

		BODY_BRANCH_BEGIN
			BODY_BRANCH(lda)
			BODY_BRANCH(lda_a)

			BODY_BRANCH(str)

			BODY_BRANCH(ldc_s_i1)
			BODY_BRANCH(ldc_s_i2)
			BODY_BRANCH(ldc_s_i4)
			BODY_BRANCH(ldc_s_i8)
			BODY_BRANCH(ldc_s_f4)
			BODY_BRANCH(ldc_s_f8)

			BODY_BRANCH(ldc_i1)
			BODY_BRANCH(ldc_i2)
			BODY_BRANCH(ldc_i4)
			BODY_BRANCH(ldc_i8)
			BODY_BRANCH(ldc_f4)
			BODY_BRANCH(ldc_f8)

			BODY_BRANCH(locals)
			BODY_BRANCH(ldl)
			BODY_BRANCH(ldl_a)
			BODY_BRANCH(stl)

			BODY_BRANCH(allocs)
			BODY_BRANCH(frees)
			BODY_BRANCH(alloch)
			BODY_BRANCH(freeh)

			BODY_BRANCH(copy_s)
			BODY_BRANCH(clt)
			BODY_BRANCH(cgt)
			BODY_BRANCH(add)

			BODY_BRANCH(conv_i2_i4)
			BODY_BRANCH(conv_i4_i2)

			BODY_BRANCH(sturef)

			BODY_BRANCH(sturef_s)
			BODY_BRANCH(sturef_s_i1)
			BODY_BRANCH(sturef_s_i2)
			BODY_BRANCH(sturef_s_i4)
			BODY_BRANCH(sturef_s_i8)
			BODY_BRANCH(sturef_s_f4)
			BODY_BRANCH(sturef_s_f8)

			BODY_BRANCH(stelem)
			BODY_BRANCH(stelem_s)

			BODY_BRANCH(ldelem)
			BODY_BRANCH(ldelem_s)
		BODY_BRANCH_END
		
		else if (vm_string_cmp(&t->string, VM_STRING_CONST_GET(args))) {
			// args (names..., )
			if (!_vmc_parse_function_arg_names(s, func)) {
				return FALSE;
			}
		}
		else if (vm_string_cmp(&t->string, VM_STRING_CONST_GET(jmpt))) {
			// jmpt <destination>

			vmi_instr_jmp instr;
			vmc_linker_marker_addr* marker;
			instr.opcode = 0;
			instr.icode = VMI_JMP;
			instr.props1 = VMI_INSTR_JMP_PROP1_TRUE;
			instr.destination = 0; // Jump forward one instruction

			vmc_lexer_next(t);
			if (t->type != VMC_LEXER_TYPE_KEYWORD) {
				return vmc_compiler_message_expected_keyword(&c->messages, t);
			}

			// Add a new marker. This is so that we can know the actual memory address during runtime.
			marker = _vmc_func_add_marker_addr(c, func, &t->string);
			if (marker == NULL) {
				vmc_compiler_message_panic(s, "out of memory");
				return FALSE;
			}

			if (vmc_linker_marker_add_inject_addr(&c->linker, marker, 
				vmc_compiler_bytecode_field_offset(c, OFFSETOF(vmi_instr_jmp, destination))) == NULL) {
				vmc_compiler_message_panic(s, "out of memory");
				return FALSE;
			}
			vmc_write(c, &instr, sizeof(vmi_instr_jmp));
		}
		else if (vm_string_cmp(&t->string, VM_STRING_CONST_GET(ret))) {
			// ret

			vmi_instr_ret instr;
			instr.opcode = 0;
			instr.icode = VMI_RET;
			instr.pop_stack_size = func->args_total_size;
			instr.pop_locals_size = func->locals_total_size;
#if defined(VM_STACK_DEBUG)
			// The values pushed on the stack when the function starts are
			// 1. Pointer to the return address location
			// 2. The previous ebp
			// 3. return values
			// 4. arguments
			instr.expected_ebp_offset = sizeof(vmi_ip) + sizeof(vm_byte*) + func->returns_total_size + func->args_total_size;
#endif
			vmc_write(c, &instr, sizeof(vmi_instr_ret));
		}
		else if (vm_string_cmp(&t->string, VM_STRING_CONST_GET(call))) {
			// call <definition>
			vmi_instr_call instr;
			vm_string signature;
			if (!vmc_lexer_next_type(t, VMC_LEXER_TYPE_KEYWORD)) {
				return vmc_compiler_message_expected_keyword(&c->messages, t);
			}
			if (!_vmc_seek_func_signature(c, t, &signature)) {
				return FALSE;
			}
			vmc_func* func2 = vmc_func_find(p, &signature);
			if (func2 == NULL) {
				return vmc_compiler_message_not_implemented(&c->messages, t);
			}
			instr.header.opcode = 0;
			instr.header.icode = VMI_CALL;
			instr.expected_stack_size = func2->args_total_size + func2->returns_total_size;
			instr.addr = OFFSET(func2->offset);
			vmc_write(c, &instr, sizeof(vmi_instr_call));
		}
		else {
			return vmc_compiler_message_not_implemented(&c->messages, t);
		}

		vmc_lexer_next(t);
	}
	func->modifiers |= VMC_FUNC_MODIFIER_HAS_BODY;
	vmc_lexer_next(t);
	return TRUE;
}

// Append a new function to the current package. The syntax is:
// [extern] fn name ([modifier][package.]type1,[modifier][package.]type2,...)([modifier][package.]ret1,[modifier][package.]ret2,...)
//
// If the function is external then a body is not required. You are allowed to have a body if you want to override the 
BOOL _vmc_parse_keyword_fn(const vmc_compiler_scope* s, vm_bits32 modifiers)
{
	vmc_compiler* const c = s->compiler;
	vmc_lexer_token* const t = s->token;

	vmc_func* const func = vmc_func_malloc();
	if (func == NULL) {
		return vmc_compiler_message_panic(s, "out of memory");
	}

	if (!_vmc_parse_func_signature(s, func)) {
		return FALSE;
	}
	vmc_package_add_func(s->package, func);
	func->id = c->functions_count++;
	func->modifiers |= modifiers;

	// External functions do not have a body
	if (vmc_func_is_extern(func))
		return TRUE;

	// Figure out the offset on the stack
	vmc_func_calculate_offsets(func);

	// Parse the function body
	if (!_vmc_parse_keyword_fn_body(s, func))
		return FALSE;
	return TRUE;
}

BOOL _vmc_parse_keyword_extern(const vmc_compiler_scope* s)
{
	vmc_lexer_token* const t = s->token;

	vmc_lexer_next(t);
	switch (t->type)
	{
	case VMC_LEXER_TYPE_KEYWORD_FN:
		return _vmc_parse_keyword_fn(s, VMC_FUNC_MODIFIER_EXTERN);
	default:
		return vmc_compiler_message_unknown_token(s);
	}
}

BOOL _vmc_parse_keyword_import(const vmc_compiler_scope* s)
{
	vmc_compiler* const c = s->compiler;
	vmc_lexer_token* const t = s->token;

	return vmc_compiler_message_not_implemented(&c->messages, t);
}

BOOL _vmc_parse_keyword(const vmc_compiler_scope* s)
{
	vmc_lexer_token* const t = s->token;

	switch (t->type)
	{
	case VMC_LEXER_TYPE_KEYWORD_EXTERN:
		return _vmc_parse_keyword_extern(s);
	case VMC_LEXER_TYPE_KEYWORD_FN:
		return _vmc_parse_keyword_fn(s, 0);
	case VMC_LEXER_TYPE_KEYWORD_IMPORT:
		return _vmc_parse_keyword_import(s);
	case VMC_LEXER_TYPE_KEYWORD_CONST:
	default:
		break;
	}
	vmc_compiler_message_unknown_token(s);
	_vmc_emit_icode(s, VMI_EOE);
	return FALSE;
}

// Parse the current lexers content and put the compiled byte-code into the compiler and put the metadata into the supplied package
void _vmc_parse(vmc_compiler* c, vmc_lexer* l, vmc_package* p)
{
	vmc_lexer_token token;
	vmc_lexer_token_init(l, &token);
	const vmc_compiler_scope scope = {
		c, &token, p, NULL
	};

	// Ignore all newlines at the start of the content
	vmc_lexer_next(&token);
	while (token.type == VMC_LEXER_TYPE_NEWLINE) vmc_lexer_next(&token);

	// Compile each instruction
	while (1)
	{
		// Is the supplied token a keyword?
		if (vmc_lexer_type_is_keyword(token.type)) {
			if (!_vmc_parse_keyword(&scope)) {
				break;
			}
			continue;
		}

		// 
		switch (token.type)
		{
		case VMC_LEXER_TYPE_NEWLINE:
		case VMC_LEXER_TYPE_COMMENT:
			break;
		case VMC_LEXER_TYPE_EOF:
			_vmc_emit_icode(&scope, VMI_EOE);
			return;
		default:
			vmc_compiler_message_unknown_token(&scope);
			_vmc_emit_icode(&scope, VMI_EOE);
			return;
		}
		vmc_lexer_next(&token);
	}
}

void _vmc_compiler_register_builtins(vmc_compiler* c)
{
	// Register the "vm" package and all type definitions that's built-in
	vmc_package* root = vmc_package_malloc("vm", 2);
	VM_LIST_ADD(c->packages, root);

	vmc_package_add_type(root, vmc_type_definition_new(VM_STRING_CONST_GET(int64), sizeof(vm_int64)));
	vmc_package_add_type(root, vmc_type_definition_new(VM_STRING_CONST_GET(uint64), sizeof(vm_uint64)));
	vmc_package_add_type(root, vmc_type_definition_new(VM_STRING_CONST_GET(int32), sizeof(vm_int32)));
	vmc_package_add_type(root, vmc_type_definition_new(VM_STRING_CONST_GET(uint32), sizeof(vm_uint32)));
	vmc_package_add_type(root, vmc_type_definition_new(VM_STRING_CONST_GET(int16), sizeof(vm_int16)));
	vmc_package_add_type(root, vmc_type_definition_new(VM_STRING_CONST_GET(uint16), sizeof(vm_uint16)));
	vmc_package_add_type(root, vmc_type_definition_new(VM_STRING_CONST_GET(int8), sizeof(vm_int8)));
	vmc_package_add_type(root, vmc_type_definition_new(VM_STRING_CONST_GET(uint8), sizeof(vm_uint8)));
	vmc_package_add_type(root, vmc_type_definition_new(VM_STRING_CONST_GET(bool), sizeof(vm_int8)));
	vmc_package_add_type(root, vmc_type_definition_new(VM_STRING_CONST_GET(float32), sizeof(vm_float32)));
	vmc_package_add_type(root, vmc_type_definition_new(VM_STRING_CONST_GET(float64), sizeof(vm_float64)));
	vmc_package_add_type(root, vmc_type_definition_new(VM_STRING_CONST_GET(ptr), sizeof(void*)));
}

vmi_process_header* _vmc_compiler_get_header(vmc_compiler* c)
{
	return ((vmi_process_header*)vm_bytestream_get(&c->bytecode, 0));
}

// Append bytecode with package, types and function information
void _vmc_append_package_info(vmc_compiler* c)
{
	const vm_int32 size = c->packages.count;
	int i, j;
	vmc_package* package;

	// Set how many packages compiled into the bytecode
	_vmc_compiler_get_header(c)->packages_count = size;
	// Set how many functions compiled into the bytecode
	_vmc_compiler_get_header(c)->functions_count = c->functions_count;
	// Set the actual offset of where the package information is found
	_vmc_compiler_get_header(c)->first_package_offset = vm_bytestream_get_size(&c->bytecode);

	// Memory structure for package information:
	// <Package Header>
	// char[]	| name bytes

	for (i = 0; i < size; ++i) {
		package = VM_LIST_GET(c->packages, vmc_package, i);
		vmi_package_bytecode_header package_header = {
			vm_string_length(&package->name),
			vmc_types_list_count_type(&package->types, VMC_TYPE_HEADER_FUNC),
			vmc_types_list_count_type(&package->types, VMC_TYPE_HEADER_TYPE),
			0,
			0
		};
		vmc_write(c, &package_header, sizeof(vmi_package_bytecode_header));
		vmc_write(c, (void*)package->name.start, vm_string_length(&package->name)); // name bytes
	}

	// Memory structure for function information:
	// int32	| name length
	// char[]	| name bytes
	// 
	// Memory structure for type information:
	// int32	| name length
	// char[]	| name bytes

	for (i = 0; i < size; ++i) {
		package = VM_LIST_GET(c->packages, vmc_package, i);
		for (j = 0; j < package->types.count; ++j) {
			vmc_type_header* header = package->types.memory[j];
			if (header->type == VMC_TYPE_HEADER_FUNC) {
				vmc_func* f = (vmc_func*)header;
				vmi_package_func_bytecode_header func_header = {
					vm_string_length(&f->name),
					f->offset,
					f->args_total_size + f->returns_total_size
				};
				vmc_write(c, &func_header, sizeof(vmi_package_func_bytecode_header));
				vmc_write(c, (void*)f->name.start, vm_string_length(&f->name)); // name bytes
			}
		}
	}
}

// Cleanup all packages
void _vmc_compiler_destroy_packages(vmc_compiler* c)
{
	vmc_package* package;
	const int size = c->packages.count;
	for (int i = 0; i < size; ++i) {
		package = VM_LIST_GET(c->packages, vmc_package, i);
		vmc_package_free(package);
	}
	vm_list_destroy(&c->packages);
}

//
// PUBLIC ///////////////////////////////////////////////////////////////////////////
// 
const vmc_compiler_config* vmc_compiler_config_default = &_vmc_compiler_config_default;

vmc_compiler* vmc_compiler_new(const vmc_compiler_config* config)
{
	vmc_compiler* c = (vmc_compiler*)vmc_malloc(sizeof(vmc_compiler));
	if (c == NULL)
		return c;
	if (config == NULL)
		config = vmc_compiler_config_default;
	c->config = *config;
	vm_bytestream_init(&c->bytecode);
	vm_messages_init(&c->messages);
	c->panic_error_message.code = VMC_COMPILER_MSG_NONE;
	vm_list_init(&c->packages);
	c->functions_count = 0;
	vmc_string_pool_init(&c->string_pool);
	vmc_linker_init(&c->linker);

	_vmc_compiler_register_builtins(c);
	vmc_package_new(c, "main", 4);
	return c;
}

void vmc_compiler_destroy(vmc_compiler* c)
{
	vmc_linker_release(&c->linker);
	vm_messages_destroy(&c->messages);
	_vmc_compiler_destroy_packages(c);
	vmc_string_pool_destroy(&c->string_pool);
	vm_bytestream_release(&c->bytecode);
	vmc_free(c);
}

BOOL vmc_compiler_compile(vmc_compiler* c, const vm_byte* src)
{
	vmc_lexer l;
	vmc_lexer_init(&l, src);
	_vmc_append_header(c);
	// TODO: Handle packages better. Hardcoded to be the main package is the second package
	_vmc_parse(c, &l, VM_LIST_GET(c->packages, vmc_package, 1));
	vm_messages_move(&l.messages, &c->messages);
	vmc_lexer_release(&l);
	if (vmc_compiler_success(c)) {
		_vmc_append_package_info(c);
		vmc_linker_process(&c->linker, &c->bytecode);
		// We might have gotten link errors, so let's verify success again
		return vmc_compiler_success(c);
	}
	else
		return FALSE;
}

vm_int32 vmc_compiler_config_import(struct vmc_compiler* c, const vm_string* path)
{
	return 0;
}

vmc_package* vmc_package_new(vmc_compiler* c, const char* name, int length)
{
	vmc_package* p = vmc_package_malloc(name, length);
	if (p == NULL)
		return NULL;
	p->global_package = VM_LIST_GET(c->packages, vmc_package, 0);
	// Always add the vm root scope as imported
	//vmc_package_add_import_alias(p, VM_LIST_GET(c->packages, vmc_package, 0), VM_STRING_CONST_GET(empty));
	p->id = vm_list_add(&c->packages, p);
	return p;
}
