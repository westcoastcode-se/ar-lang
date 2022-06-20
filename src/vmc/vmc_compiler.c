#include "vmc_compiler.h"
#include "vmc_lexer_logic.h"
#include "vmc_lexer_math.h"
#include "vmc_compiler_messages.h"
#include "../interpreter/vmi_ops.h"
#include "../interpreter/vmi_process.h"
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
#define BODY_BRANCH(C) else if (vm_string_cmp(&t->string, VM_STRING_CONST_GET(C))) { if (!parse_##C(c, l, p, t, func)) return FALSE; }

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

VM_STRING_CONST(load_a, "load_a", 6);
VM_STRING_CONST(save_r, "save_r", 6);
VM_STRING_CONST(alloc_s, "alloc_s", 7);
VM_STRING_CONST(free_s, "free_s", 6);
VM_STRING_CONST(ret, "ret", 3);
VM_STRING_CONST(call, "call", 4);
VM_STRING_CONST(locals, "locals", 6);
VM_STRING_CONST(load_l, "load_l", 6);
VM_STRING_CONST(save_l, "save_l", 6);
VM_STRING_CONST(copy_s, "copy_s", 6);

VM_STRING_CONST(c_i32, "c_i32", 5);
VM_STRING_CONST(c_i16, "c_i16", 5);

VM_STRING_CONST(conv_i16_i32, "conv_i16_i32", 12);
VM_STRING_CONST(conv_i32_i16, "conv_i32_i16", 12);

VM_STRING_CONST(clt, "clt", 3);
VM_STRING_CONST(cgt, "cgt", 3);
VM_STRING_CONST(jmpt, "jmpt", 4);

VM_STRING_CONST(add, "add", 3);

VM_STRING_CONST(_0, "_0", 2);
VM_STRING_CONST(_1, "_1", 2);
VM_STRING_CONST(_2, "_2", 2);
VM_STRING_CONST(_3, "_3", 2);
VM_STRING_CONST(_4, "_4", 2);
VM_STRING_CONST(_5, "_5", 2);
VM_STRING_CONST(_6, "_6", 2);
VM_STRING_CONST(_7, "_7", 2);
VM_STRING_CONST(_8, "_8", 2);
VM_STRING_CONST(_9, "_9", 2);

const vm_string* _vm_string_const_anon_names[10] = {
	VM_STRING_CONST_GET(_0),
	VM_STRING_CONST_GET(_1),
	VM_STRING_CONST_GET(_2),
	VM_STRING_CONST_GET(_3),
	VM_STRING_CONST_GET(_4),
	VM_STRING_CONST_GET(_5),
	VM_STRING_CONST_GET(_6),
	VM_STRING_CONST_GET(_7),
	VM_STRING_CONST_GET(_8),
	VM_STRING_CONST_GET(_9)
};

const vmc_compiler_config _vmc_compiler_config_default = {
	NULL,
	&vmc_compiler_config_import
};

void _vmc_emit_begin(vmc_compiler* c, vm_int8 argument_total_size, vm_int8 return_total_size)
{
	vmi_instr_begin instr;
	instr.opcode = 0;
	instr.icode = VMI_BEGIN;
	instr.expected_stack_size = argument_total_size + return_total_size;
	vmc_write(c, &instr, sizeof(vmi_instr_begin));
}

void _vmc_emit_opcode(vmc_compiler* c, vm_int32 opcode)
{
	vmi_instr_single_instruction instr;
	instr.header.opcode = opcode;
	vmc_write(c, &instr, sizeof(vmi_instr_single_instruction));
}

void _vmc_emit_icode(vmc_compiler* c, vm_int8 icode)
{
	vmi_instr_single_instruction instr;
	instr.header.opcode = 0;
	instr.header.icode = icode;
	vmc_write(c, &instr, sizeof(vmi_instr_single_instruction));
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

const vm_string* _vmc_prepare_func_get_signature(vmc_compiler* c, vm_string name,
	vmc_var_definition* args, vm_int32 args_count,
	vmc_var_definition* returns, vm_int32 returns_count)
{
	char memory[2048];
	char* ptr = vm_str_cpy(memory, name.start, vm_string_length(&name));
	*ptr++ = '(';
	for (int i = 0; i < args_count; ++i) {
		const vm_int32 len = vm_string_length(&args[i].type.definition->name);
		const vmc_type_info* info = &args[i].type;
		if (i > 0)
			*ptr++ = ',';
		if ((info->masks & VMC_TYPE_INFO_MASK_ADDR) == VMC_TYPE_INFO_MASK_ADDR)
			*ptr++ = '*';
		ptr = vm_str_cpy(ptr, info->definition->name.start, len);
	}
	*ptr++ = ')';
	*ptr++ = '(';
	for (int i = 0; i < returns_count; ++i) {
		const vm_int32 len = vm_string_length(&returns[i].type.definition->name);
		const vmc_type_info* info = &returns[i].type;
		if (i > 0)
			*ptr++ = ',';
		if ((info->masks & VMC_TYPE_INFO_MASK_ADDR) == VMC_TYPE_INFO_MASK_ADDR)
			*ptr++ = '*';
		ptr = vm_str_cpy(ptr, info->definition->name.start, len);
	}
	*ptr++ = ')';
	return vmc_string_pool_stringsz(&c->string_pool, memory, (int)(ptr - memory));
}

BOOL _vmc_prepare_func_signature(vmc_compiler* c, vmc_func* func)
{
	func->signature = *_vmc_prepare_func_get_signature(c, func->name, func->args, func->args_count,
		func->returns, func->returns_count);
	return TRUE;
}

BOOL _vmc_compiler_parse_type_decl_without_name(vmc_compiler* c, vmc_lexer* l, vmc_package* p, vmc_lexer_token* t,
	vmc_var_definition* vars, vm_int32* out_count, vm_int32* out_total_size)
{
	vm_int32 count = 0;
	vm_int32 total_size = 0;
	vmc_package* type_package;
	vmc_type_definition* type_definition;

	// Expected a '(' token
	if (!vmc_lexer_next_type(l, t, VMC_LEXER_TYPE_PARAN_L))
		return vmc_compiler_message_syntax_error(&c->messages, l, t, '(');

	// Parse until we reach that end ')' token
	while (!vmc_lexer_next_type(l, t, VMC_LEXER_TYPE_PARAN_R)) {
		vmc_var_definition* const var = &vars[count];
		type_package = p;

		// Ignore comma
		if (t->type == VMC_LEXER_TYPE_COMMA) {
			vmc_lexer_next(l, t);
		}

		// Reset masks
		var->type.masks = 0;
		var->type.offset = 0;
		var->name = *_vm_string_const_anon_names[count];

		// types can be:
		// KEYWORD
		// *KEYWORD
		// [INT]KEYWORD
		// KEYWPORD<TYPE>

		// We can, quickly figure out if this is a type if the keyword is:
		// '[' or '*' since those are part of the type
		if (t->type == VMC_LEXER_TYPE_SQUARE_L || t->type == VMC_LEXER_TYPE_PTR) {
			// These types of types are not supported yet!
			return vmc_compiler_message_not_implemented(&c->messages, l, t);
		}

		// Expected type
		if (t->type != VMC_LEXER_TYPE_KEYWORD)
			return vmc_compiler_message_expected_type(&c->messages, l, t);

		type_definition = vmc_package_find_type(type_package, &t->string);
		if (type_definition == NULL)
			return vmc_compiler_message_type_not_found(&c->messages, l, t);
		var->type.definition = type_definition;

		// Figure out the size of the variable
		if ((var->type.masks & VMC_TYPE_INFO_MASK_ADDR) == VMC_TYPE_INFO_MASK_ADDR)
			var->type.size = sizeof(void*);
		else
			var->type.size = type_definition->size;

		// Argument now loaded
		count++;
		total_size += var->type.size;
	}

	*out_count = count;
	*out_total_size = total_size;
	return TRUE;
}

BOOL _vmc_parse_keyword_fn_args(vmc_compiler* c, vmc_lexer* l, vmc_package* p, vmc_lexer_token* t, vmc_func* func)
{
	func->args_count = 0;
	func->args_total_size = 0;
	vmc_package* type_package;
	vmc_type_definition* type_definition;

	// Expected a '(' token
	if (!vmc_lexer_next_type(l, t, VMC_LEXER_TYPE_PARAN_L))
		return vmc_compiler_message_syntax_error(&c->messages, l, t, '(');

	// Parse until we reach that end ')' token
	while (!vmc_lexer_next_type(l, t, VMC_LEXER_TYPE_PARAN_R)) {
		vmc_var_definition* const var = &func->args[func->args_count];
		type_package = p;

		// Ignore comma
		if (t->type == VMC_LEXER_TYPE_COMMA) {
			vmc_lexer_next(l, t);
		}

		// Reset masks
		var->type.masks = 0;
		var->type.offset = 0;

		// Read var name
		if (t->type != VMC_LEXER_TYPE_KEYWORD)
			return vmc_compiler_message_expected_identifier(&c->messages, l, t);
		var->name = t->string;
		vmc_lexer_next(l, t);

		// The first part can be a name OR a type
		// names are guaranteed to be a KEYWORD
		// vars can be:
		// KEYWORD
		// *KEYWORD
		// [INT]KEYWORD
		// KEYWPORD<TYPE>

		// We can, quickly figure out if this is a type if the keyword is:
		// '[' or '*' since those are part of the type
		if (t->type == VMC_LEXER_TYPE_SQUARE_L || t->type == VMC_LEXER_TYPE_PTR) {
			// These types of types are not supported yet!
			return vmc_compiler_message_not_implemented(&c->messages, l, t);
		}

		// Expected type
		if (t->type != VMC_LEXER_TYPE_KEYWORD)
			return vmc_compiler_message_expected_type(&c->messages, l, t);

		type_definition = vmc_package_find_type(type_package, &t->string);
		if (type_definition == NULL)
			return vmc_compiler_message_type_not_found(&c->messages, l, t);
		var->type.definition = type_definition;

		// Figure out the size of the variable
		if ((var->type.masks & VMC_TYPE_INFO_MASK_ADDR) == VMC_TYPE_INFO_MASK_ADDR)
			var->type.size = sizeof(void*);
		else
			var->type.size = type_definition->size;

		// Argument now loaded
		func->args_count++;
		func->args_total_size += var->type.size;
	}

	return TRUE;
}

// fn (<scope>) Name (<args>) (<returns>)

BOOL _vmc_parse_keyword_fn_locals(vmc_compiler* c, vmc_lexer* l, vmc_package* p, vmc_lexer_token* t, vmc_func* func)
{
	func->locals_count = 0;
	func->locals_total_size = 0;
	vmc_package* type_package;
	vmc_type_definition* type_definition;

	// Expected a '(' token
	if (!vmc_lexer_next_type(l, t, VMC_LEXER_TYPE_PARAN_L))
		return vmc_compiler_message_syntax_error(&c->messages, l, t, '(');

	// Parse until we reach that end ')' token
	while (!vmc_lexer_next_type(l, t, VMC_LEXER_TYPE_PARAN_R)) {
		vmc_var_definition* const var = &func->locals[func->locals_count];
		type_package = p;

		// Ignore comma
		if (t->type == VMC_LEXER_TYPE_COMMA) {
			vmc_lexer_next(l, t);
		}

		// Reset masks
		var->type.masks = 0;
		var->type.offset = 0;

		// Read var name
		if (t->type != VMC_LEXER_TYPE_KEYWORD)
			return vmc_compiler_message_expected_identifier(&c->messages, l, t);
		var->name = t->string;
		vmc_lexer_next(l, t);

		// The first part can be a name OR a type
		// names are guaranteed to be a KEYWORD
		// vars can be:
		// KEYWORD
		// *KEYWORD
		// [INT]KEYWORD
		// KEYWPORD<TYPE>

		// We can, quickly figure out if this is a type if the keyword is:
		// '[' or '*' since those are part of the type
		if (t->type == VMC_LEXER_TYPE_SQUARE_L || t->type == VMC_LEXER_TYPE_PTR) {
			// These types of types are not supported yet!
			return vmc_compiler_message_not_implemented(&c->messages, l, t);
		}

		// Expected type
		if (t->type != VMC_LEXER_TYPE_KEYWORD)
			return vmc_compiler_message_expected_type(&c->messages, l, t);

		type_definition = vmc_package_find_type(type_package, &t->string);
		if (type_definition == NULL)
			return vmc_compiler_message_type_not_found(&c->messages, l, t);
		var->type.definition = type_definition;

		// Figure out the size of the variable
		if ((var->type.masks & VMC_TYPE_INFO_MASK_ADDR) == VMC_TYPE_INFO_MASK_ADDR)
			var->type.size = sizeof(void*);
		else
			var->type.size = type_definition->size;

		// Argument now loaded
		func->locals_count++;
		func->locals_total_size += var->type.size;
	}

	// Augment offsets (where is the memory located from the "ebp"'s point of view).
	// EBP points to where the stack is located 
	if (func->locals_count > 0) {
		vm_int32 i;
		vm_int32 offset = sizeof(vmi_ip) + sizeof(vm_byte*) + func->args_total_size + func->returns_total_size;
		for (i = 0; i < func->locals_count; ++i) {
			vmc_type_info* info = &func->locals[i].type;
			info->offset = offset;
			offset += info->size;
		}
	}

	return TRUE;
}

BOOL _vmc_parse_keyword_fn_rets(vmc_compiler* c, vmc_lexer* l, vmc_package* p, vmc_lexer_token* t, vmc_func* func)
{
	return _vmc_compiler_parse_type_decl_without_name(c, l, p, t,
		func->returns, &func->returns_count, &func->returns_total_size);
}

vmc_linker_memory_marker* _vmc_func_add_empty_memory_marker(vmc_compiler* c, vmc_func* func, const vm_string* name)
{
	vmc_linker_memory_marker* const marker = vmc_linker_memory_marker_alloc();
	if (marker == NULL) {
		return FALSE;
	}
	marker->name = *name;
	if (func->memory_marker_last == NULL)
		func->memory_marker_first = func->memory_marker_last = marker;
	else {
		func->memory_marker_last->next = marker;
		func->memory_marker_last = marker;
	}
	return marker;
}

BOOL _vmc_func_add_memory_marker(vmc_compiler* c, vmc_func* func, const vm_string* name)
{
	vmc_linker_memory_marker* marker = vmc_linker_memory_marker_find_sibling(func->memory_marker_first, name);
	if (marker != NULL) {
		// If this marker already have an offset then this marker already exists
		if (marker->offset != 0) {
			return FALSE;
		}
		marker->offset = vm_bytestream_get_size(&c->bytecode);
		return TRUE;
	}
	marker = _vmc_func_add_empty_memory_marker(c, func, name);
	if (marker == NULL) {
		return FALSE;
	}
	marker->offset = vm_bytestream_get_size(&c->bytecode);
	return TRUE;
}

#include "vmc_compiler_instr.inc.c"

BOOL _vmc_parse_keyword_fn_body(vmc_compiler* c, vmc_lexer* l, vmc_package* p, vmc_lexer_token* t, vmc_func* func)
{
	// External functions are not allowed to have a body
	if (vmc_func_is_extern(func))
		return TRUE;

	// Expected a {
	if (!vmc_lexer_next_type(l, t, VMC_LEXER_TYPE_BRACKET_L))
		return vmc_compiler_message_syntax_error(&c->messages, l, t, '{');
	vmc_lexer_next(l, t);

	// Body is now definde and we know where the body bytecode starts
	func->offset = vm_bytestream_get_size(&c->bytecode);

	// The function is actually beginning here
	// TODO: Add support for reserving memory for local variables
	if ((vm_uint32)(func->args_total_size + func->returns_total_size) > UINT16_MAX)
		return vmc_compiler_message_not_implemented(&c->messages, l, t);
	_vmc_emit_begin(c, func->args_total_size, func->returns_total_size);

	while (1) {
		// Unexpected end of function body
		if (t->type == VMC_LEXER_TYPE_EOF) {
			return vmc_compiler_message_unexpected_eof(&c->messages, l, t);
		}

		// Break if we've reached the end of the function
		if (t->type == VMC_LEXER_TYPE_BRACKET_R) {
			break;
		}

		// Ignore comments
		if (t->type == VMC_LEXER_TYPE_COMMENT) {
			vmc_lexer_next(l, t);
			continue;
		}

		// Ignore memory markers for now
		if (t->type == VMC_LEXER_TYPE_MEMORY_MARKER) {
			if (!_vmc_func_add_memory_marker(c, func, &t->string))
				return vmc_compiler_message_memory_marker_exists(&c->messages, l, &t->string);
			vmc_lexer_next(l, t);
			continue;
		}

		// Verify that a keyword is being processed
		if (!vmc_lexer_type_is_keyword(t->type)) {
			return vmc_compiler_message_expected_keyword(&c->messages, l, t);
		}

		BODY_BRANCH_BEGIN
			BODY_BRANCH(load_a)
			BODY_BRANCH(save_r)
			BODY_BRANCH(c_i16)
			BODY_BRANCH(c_i32)
			BODY_BRANCH(locals)
			BODY_BRANCH(load_l)
			BODY_BRANCH(save_l)
			BODY_BRANCH(alloc_s)
			BODY_BRANCH(free_s)
			BODY_BRANCH(copy_s)
			BODY_BRANCH(clt)
			BODY_BRANCH(cgt)
			BODY_BRANCH(add)
			BODY_BRANCH(conv_i16_i32)
			BODY_BRANCH(conv_i32_i16)
		BODY_BRANCH_END
		
		else if (vm_string_cmp(&t->string, VM_STRING_CONST_GET(jmpt))) {
			// jmpt <destination>

			vmi_instr_jmp instr;
			vmc_linker_memory_marker* marker;
			instr.opcode = 0;
			instr.icode = VMI_JMP;
			instr.props1 = VMI_INSTR_JMP_PROP1_TRUE;
			instr.destination = 0; // Jump forward one instruction

			vmc_lexer_next(l, t);
			if (t->type != VMC_LEXER_TYPE_KEYWORD) {
				return vmc_compiler_message_expected_keyword(&c->messages, l, t);
			}
			// Try to find a marker. If no marker exist then create/add it
			marker = vmc_linker_memory_marker_find_sibling(func->memory_marker_first, &t->string);
			if (marker == NULL) {
				marker = _vmc_func_add_empty_memory_marker(c, func, &t->string);
				if (marker == NULL) {
					vmc_compiler_message_panic(&c->panic_error_message, "out of memory");
					return FALSE;
				}
			}
			if (marker->offset == 0) {
				if (vmc_linker_add_memory_marker(&c->linker, marker, OFFSETOF(vmi_instr_jmp, destination)) == NULL) {
					vmc_compiler_message_panic(&c->panic_error_message, "out of memory");
					return FALSE;
				}
			}
			else {
				instr.destination = OFFSET(marker->offset);
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
			vm_string name;
			vmc_var_definition args[9];
			vm_int32 args_count;
			vmc_var_definition returns[9];
			vm_int32 returns_count;
			vm_int32 _;
			vmc_func* func;
			vmi_instr_call instr;

			// Name of the function
			if (!vmc_lexer_next_type(l, t, VMC_LEXER_TYPE_KEYWORD))
				return vmc_compiler_message_expected_keyword(&c->messages, l, t);
			name = t->string;

			// Fetch arguments
			if (!_vmc_compiler_parse_type_decl_without_name(c, l, p, t, args, &args_count, &_))
				return FALSE;

			// Fetch returns
			if (!_vmc_compiler_parse_type_decl_without_name(c, l, p, t, returns, &returns_count, &_))
				return FALSE;

			func = vmc_func_find(p, _vmc_prepare_func_get_signature(c, name, args, args_count, returns, returns_count));
			if (func == NULL) {
				return vmc_compiler_message_not_implemented(&c->messages, l, t);
			}
			instr.header.opcode = 0;
			instr.header.icode = VMI_CALL;
			instr.addr = OFFSET(func->offset);
			vmc_write(c, &instr, sizeof(vmi_instr_call));
		}
		else {
			return vmc_compiler_message_not_implemented(&c->messages, l, t);
		}

		vmc_lexer_next(l, t);
	}
	func->modifiers |= VMC_FUNC_MODIFIER_HAS_BODY;
	vmc_lexer_next(l, t);
	return TRUE;
}

// Parse the function signature:
// [name:keyword]([arg1:keyword] [type1:keyword],...)([type1:keyword])
vmc_func* _vmc_parse_func_signature(vmc_compiler* c, vmc_lexer* l, vmc_package* p, vmc_lexer_token* t)
{
	vmc_func* const func = vmc_func_malloc();
	if (func == NULL) {
		vmc_compiler_message_panic(&c->panic_error_message, "out of memory");
		return NULL;
	}

	// 1. Function name
	if (!vmc_lexer_next_type(l, t, VMC_LEXER_TYPE_KEYWORD)) {
		vmc_compiler_message_expected_identifier(&c->messages, l, t);
		return NULL;
	}

	func->name = t->string;
	if (vmc_lexer_test_uppercase(*func->name.start))
		func->modifiers |= VMC_FUNC_MODIFIER_PUBLIC;

	// Parse args
	if (!_vmc_parse_keyword_fn_args(c, l, p, t, func))
		return NULL;

	// Parse return types
	if (!_vmc_parse_keyword_fn_rets(c, l, p, t, func))
		return NULL;

	// Build the signature string
	if (!_vmc_prepare_func_signature(c, func))
		return NULL;
	return func;
}

// Append a new function to the current package. The syntax is:
// [extern] fn name (arg1 [modifier][package.]type1, arg2 [modifier][package.]type2, ...) ([modifier][package.]ret1, [modifier][package.]ret2, ...)
//
// If the function is external then a body is not required. You are allowed to have a body if you want to override the 
BOOL _vmc_parse_keyword_fn(vmc_compiler* c, vmc_lexer* l, vmc_package* p, vmc_lexer_token* t, vm_bits32 modifiers)
{
	vmc_func* func;

	func = _vmc_parse_func_signature(c, l, p, t);
	if (func == NULL) {
		return FALSE;
	}
	vmc_package_add_func(p, func);
	func->id = c->functions_count++;
	func->modifiers |= modifiers;

	// External functions do not have a body
	if (vmc_func_is_extern(func))
		return TRUE;

	// Figure out the offset on the stack
	vmc_func_calculate_offsets(func);

	// Parse the function body
	if (!_vmc_parse_keyword_fn_body(c, l, p, t, func))
		return FALSE;
	return TRUE;
}

BOOL _vmc_parse_keyword_extern(vmc_compiler* c, vmc_lexer* l, vmc_package* p, vmc_lexer_token* t)
{
	vmc_lexer_next(l, t);
	switch (t->type)
	{
	case VMC_LEXER_TYPE_KEYWORD_FN:
		return _vmc_parse_keyword_fn(c, l, p, t, VMC_FUNC_MODIFIER_EXTERN);
	default:
		return vmc_compiler_message_unknown_token(&c->messages, l, t);
	}
}

BOOL _vmc_parse_keyword(vmc_compiler* c, vmc_lexer* l, vmc_package* p, vmc_lexer_token* t)
{
	switch (t->type)
	{
	case VMC_LEXER_TYPE_KEYWORD_EXTERN:
		return _vmc_parse_keyword_extern(c, l, p, t);
	case VMC_LEXER_TYPE_KEYWORD_FN:
		return _vmc_parse_keyword_fn(c, l, p, t, 0);
	case VMC_LEXER_TYPE_KEYWORD_CONST:
	default:
		break;
	}

	vmc_compiler_message_unknown_token(&c->messages, l, t);
	_vmc_emit_icode(c, VMI_EOE);
	return FALSE;
}

// Parse the current lexers content and put the compiled byte-code into the compiler and put the metadata into the supplied package
void _vmc_parse(vmc_compiler* c, vmc_lexer* l, vmc_package* p)
{
	vmc_lexer_token token;

	// Ignore all newlines at the start of the content
	vmc_lexer_next(l, &token);
	while (token.type == VMC_LEXER_TYPE_NEWLINE) vmc_lexer_next(l, &token);

	// Compile each instruction
	while (1)
	{
		// Is the supplied token a keyword?
		if (vmc_lexer_type_is_keyword(token.type)) {
			if (!_vmc_parse_keyword(c, l, p, &token)) {
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
			_vmc_emit_icode(c, VMI_EOE);
			return;
		default:
			vmc_compiler_message_unknown_token(&c->messages, l, &token);
			_vmc_emit_icode(c, VMI_EOE);
			return;
		}
		vmc_lexer_next(l, &token);
	}
}

void _vmc_compiler_register_builtins(vmc_compiler* c)
{
	// Register the "vm" package and all type definitions that's built-in
	c->package_first = c->package_last = vmc_package_malloc("vm", 2);
	c->package_first->id = c->packages_count++;
	vmc_type_definition_new(c->package_first, VM_STRING_CONST_GET(int64), sizeof(vm_int64));
	vmc_type_definition_new(c->package_first, VM_STRING_CONST_GET(uint64), sizeof(vm_uint64));
	vmc_type_definition_new(c->package_first, VM_STRING_CONST_GET(int32), sizeof(vm_int32));
	vmc_type_definition_new(c->package_first, VM_STRING_CONST_GET(uint32), sizeof(vm_uint32));
	vmc_type_definition_new(c->package_first, VM_STRING_CONST_GET(int16), sizeof(vm_int16));
	vmc_type_definition_new(c->package_first, VM_STRING_CONST_GET(uint16), sizeof(vm_uint16));
	vmc_type_definition_new(c->package_first, VM_STRING_CONST_GET(int8), sizeof(vm_int8));
	vmc_type_definition_new(c->package_first, VM_STRING_CONST_GET(uint8), sizeof(vm_uint8));
	vmc_type_definition_new(c->package_first, VM_STRING_CONST_GET(bool), sizeof(vm_int8));
	vmc_type_definition_new(c->package_first, VM_STRING_CONST_GET(float32), sizeof(vm_float32));
	vmc_type_definition_new(c->package_first, VM_STRING_CONST_GET(float64), sizeof(vm_float64));
	vmc_type_definition_new(c->package_first, VM_STRING_CONST_GET(ptr), sizeof(void*));
}

vmi_process_header* _vmc_compiler_get_header(vmc_compiler* c)
{
	return ((vmi_process_header*)vm_bytestream_get(&c->bytecode, 0));
}

// Append bytecode with package, types and function information
void _vmc_append_package_info(vmc_compiler* c)
{
	vmc_package* p = c->package_first;

	// Set how many packages compiled into the bytecode
	_vmc_compiler_get_header(c)->packages_count = c->packages_count;
	// Set how many functions compiled into the bytecode
	_vmc_compiler_get_header(c)->functions_count = c->functions_count;
	// Set the actual offset of where the package information is found
	_vmc_compiler_get_header(c)->first_package_offset = vm_bytestream_get_size(&c->bytecode);

	// Memory structure for package information:
	// <Package Header>
	// char[]	| name bytes

	while (p != NULL) {
		vmi_package_bytecode_header package_header = {
			vm_string_length(&p->name),
			p->func_count,
			p->type_count,
			0,
			0
		};
		vmc_write(c, &package_header, sizeof(vmi_package_bytecode_header));
		vmc_write(c, (void*)p->name.start, vm_string_length(&p->name)); // name bytes
		p = p->next;
	}

	// Memory structure for function information:
	// int32	| name length
	// char[]	| name bytes
	// 
	// Memory structure for type information:
	// int32	| name length
	// char[]	| name bytes

	p = c->package_first;
	while (p != NULL) {
		vmc_func* f = p->func_first;
		while (f != NULL) {
			vmi_package_func_bytecode_header func_header = {
				vm_string_length(&f->name),
				f->offset
			};
			vmc_write(c, &func_header, sizeof(vmi_package_func_bytecode_header));
			vmc_write(c, (void*)f->name.start, vm_string_length(&f->name)); // name bytes
			f = f->next;
		}
		p = p->next;
	}
}

// Cleanup all packages
void _vmc_compiler_destroy_packages(vmc_compiler* c)
{
	vmc_package* p = c->package_first;
	while (p != NULL) {
		vmc_package* const next = p->next;
		vmc_package_free(p);
		p = next;
	}
	c->package_first = c->package_last = NULL;
}

//
// PUBLIC ///////////////////////////////////////////////////////////////////////////
// 
const vmc_compiler_config* vmc_compiler_config_default = &_vmc_compiler_config_default;

vmc_compiler* vmc_compiler_new(const vmc_compiler_config* config)
{
	vmc_compiler* c = (vmc_compiler*)malloc(sizeof(vmc_compiler));
	if (c == NULL)
		return c;
	if (config == NULL)
		config = vmc_compiler_config_default;
	c->config = *config;
	vm_bytestream_init(&c->bytecode);
	vm_messages_init(&c->messages);
	c->panic_error_message.code = VMC_COMPILER_MSG_NONE;
	c->package_first = c->package_last = NULL;
	c->packages_count = 0;
	c->functions_count = 0;
	vmc_string_pool_init(&c->string_pool);
	vmc_linker_init(&c->linker, &c->bytecode);

	_vmc_compiler_register_builtins(c);
	vmc_package_new(c, "main", 4);
	return c;
}

void vmc_compiler_destroy(vmc_compiler* c)
{
	vmc_linker_release(&c->linker);
	vmc_string_pool_destroy(&c->string_pool);
	vm_messages_destroy(&c->messages);
	_vmc_compiler_destroy_packages(c);
	vm_bytestream_release(&c->bytecode);
	free(c);
}

BOOL vmc_compiler_compile(vmc_compiler* c, const vm_byte* src)
{
	vmc_lexer l;
	vmc_lexer_init(&l, src);
	_vmc_append_header(c);
	// Main package is the second package
	_vmc_parse(c, &l, c->package_first->next);
	vm_messages_move(&l.messages, &c->messages);
	vmc_lexer_release(&l);
	if (vmc_compiler_success(c)) {
		_vmc_append_package_info(c);
		vmc_linker_process(&c->linker);
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
	// Add the package to the linked list. Assume that vm/root package is in the beginning
	p->root_package = c->package_first;
	c->package_last->next = p;
	c->package_last = p;
	p->id = c->packages_count++;
	return p;
}
