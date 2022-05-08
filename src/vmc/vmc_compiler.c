#include "vmc_compiler.h"
#include "vmc_lexer_logic.h"
#include "vmc_lexer_math.h"
#include "../interpreter/vmi_ops.h"
#include "../interpreter/vmi_process.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>

//
// PRIVATE ///////////////////////////////////////////////////////////////////////////
// 

VM_STRING_CONST(int8, "int8", 4);
VM_STRING_CONST(int16, "int16", 5);
VM_STRING_CONST(int32, "int32", 5);
VM_STRING_CONST(int64, "int64", 5);

VM_STRING_CONST(load_a, "load_a", 6);
VM_STRING_CONST(const, "const", 5);
VM_STRING_CONST(save_r, "save_r", 6);
VM_STRING_CONST(ret, "ret", 3);
VM_STRING_CONST(add, "add", 3);
VM_STRING_CONST(call, "call", 4);

const vmc_compiler_config _vmc_compiler_config_default = {
	NULL,
	&vmc_compiler_config_import
};

BOOL _vmc_add_message(vmc_compiler* c, int error_code, const char* format, ...)
{
	va_list argptr;
	vm_message* m = (vm_message*)malloc(sizeof(vm_message));
	if (m == NULL)
		return FALSE;

	m->code = error_code;
	m->next = NULL;

	va_start(argptr, format);
	vsprintf(m->message, format, argptr);
	va_end(argptr);

	if (c->messages_last == NULL)
		c->messages_last = c->messages_first = m;
	else
		c->messages_last->next = m;
	return FALSE;
}

BOOL _vmc_add_error_unknown_token(vmc_compiler* c, vmc_lexer* l, vmc_lexer_token* token)
{
	int line, line_offset, offset;
	vmc_lexer_get_metadata(l, &line, &line_offset, &offset);
	return _vmc_add_message(c, VMC_ERROR_CODE_UNKNOWN_TOKEN, "unknown symbol: '%.*s' at %d:%d",
		vm_string_length(&token->string), token->string.start,
		line, line_offset);
}

BOOL _vmc_add_error_expected_identifier(vmc_compiler* c, vmc_lexer* l, vmc_lexer_token* token)
{
	int line, line_offset, offset;
	vmc_lexer_get_metadata(l, &line, &line_offset, &offset);
	return _vmc_add_message(c, VMC_ERROR_CODE_EXPECTED_IDENTIFIER, "expected identifier but was '%.*s' at %d:%d",
		vm_string_length(&token->string), token->string.start,
		line, line_offset);
}

BOOL _vmc_add_error_expected_type(vmc_compiler* c, vmc_lexer* l, vmc_lexer_token* token)
{
	int line, line_offset, offset;
	vmc_lexer_get_metadata(l, &line, &line_offset, &offset);
	return _vmc_add_message(c, VMC_ERROR_CODE_EXPECTED_TYPE, "expected type but was '%.*s' at %d:%d",
		vm_string_length(&token->string), token->string.start,
		line, line_offset);
}

BOOL _vmc_add_error_expected_keyword(vmc_compiler* c, vmc_lexer* l, vmc_lexer_token* token)
{
	int line, line_offset, offset;
	vmc_lexer_get_metadata(l, &line, &line_offset, &offset);
	return _vmc_add_message(c, VMC_ERROR_CODE_EXPECTED_KEYWORD, "expected keyword but was '%.*s' at %d:%d",
		vm_string_length(&token->string), token->string.start,
		line, line_offset);
}

BOOL _vmc_add_error_expected_index(vmc_compiler* c, vmc_lexer* l, vmc_lexer_token* token)
{
	int line, line_offset, offset;
	vmc_lexer_get_metadata(l, &line, &line_offset, &offset);
	return _vmc_add_message(c, VMC_ERROR_CODE_EXPECTED_INDEX, "expected index but was '%.*s' at %d:%d",
		vm_string_length(&token->string), token->string.start,
		line, line_offset);
}

BOOL _vmc_add_error_expected_integer(vmc_compiler* c, vmc_lexer* l, vmc_lexer_token* token)
{
	int line, line_offset, offset;
	vmc_lexer_get_metadata(l, &line, &line_offset, &offset);
	return _vmc_add_message(c, VMC_ERROR_CODE_EXPECTED_INTEGER, "expected integer but was '%.*s' at %d:%d",
		vm_string_length(&token->string), token->string.start,
		line, line_offset);
}

BOOL _vmc_add_error_not_implemented_yet(vmc_compiler* c, vmc_lexer* l, vmc_lexer_token* token)
{
	int line, line_offset, offset;
	vmc_lexer_get_metadata(l, &line, &line_offset, &offset);
	return _vmc_add_message(c, VMC_ERROR_CODE_NOT_IMPLEMENTED_YET, "'%.*s' is not implemented yet at %d:%d",
		vm_string_length(&token->string), token->string.start,
		line, line_offset);
}

BOOL _vmc_add_error_invalid_arg(vmc_compiler* c, vmc_lexer* l, vm_int32 index, vm_int32 max_index)
{
	int line, line_offset, offset;
	vmc_lexer_get_metadata(l, &line, &line_offset, &offset);
	return _vmc_add_message(c, VMC_ERROR_CODE_INVALID_ARG, "invalid index %d but the maximum is %d at %d:%d",
		index, max_index, line, line_offset);
}

BOOL _vmc_add_error_type_not_found(vmc_compiler* c, vmc_lexer* l, vmc_lexer_token* token)
{
	int line, line_offset, offset;
	vmc_lexer_get_metadata(l, &line, &line_offset, &offset);
	return _vmc_add_message(c, VMC_ERROR_CODE_TYPE_NOT_FOUND, "type '%.*s' was not found at %d:%d",
		vm_string_length(&token->string), token->string.start,
		line, line_offset);
}

BOOL _vmc_add_error_expected_parant(vmc_compiler* c, vmc_lexer* l, vmc_lexer_token* token)
{
	int line, line_offset, offset;
	vmc_lexer_get_metadata(l, &line, &line_offset, &offset);
	return _vmc_add_message(c, VMC_ERROR_CODE_EXPECTED_PARANT, "expected paranthesis but was '%.*s' at %d:%d",
		vm_string_length(&token->string), token->string.start,
		line, line_offset);
}

BOOL _vmc_add_error_extern_unknown(vmc_compiler* c, vmc_lexer* l, vmc_lexer_token* token)
{
	int line, line_offset, offset;
	vmc_lexer_get_metadata(l, &line, &line_offset, &offset);
	return _vmc_add_message(c, VMC_ERROR_CODE_EXTERN_UNKNOWN, "expected fn or type keyword but was '%.*s' at %d:%d",
		vm_string_length(&token->string), token->string.start,
		line, line_offset);
}

BOOL _vmc_add_error_expected_bracket(vmc_compiler* c, vmc_lexer* l, vmc_lexer_token* token)
{
	int line, line_offset, offset;
	vmc_lexer_get_metadata(l, &line, &line_offset, &offset);
	return _vmc_add_message(c, VMC_ERROR_CODE_EXPECTED_BRACKET, "expected bracket start/end but was '%.*s' at %d:%d",
		vm_string_length(&token->string), token->string.start,
		line, line_offset);
}

BOOL _vmc_add_error_incomplete_fn_body(vmc_compiler* c, vmc_lexer* l, vmc_lexer_token* token)
{
	int line, line_offset, offset;
	vmc_lexer_get_metadata(l, &line, &line_offset, &offset);
	return _vmc_add_message(c, VMC_ERROR_CODE_INCOMPLETE_FN_BODY, "incomplete function body but was '%.*s' at %d:%d",
		vm_string_length(&token->string), token->string.start,
		line, line_offset);
}

BOOL _vmc_add_error_invalid_segment(vmc_compiler* c, vmc_lexer* l, vmc_lexer_token* token)
{
	int line, line_offset, offset;
	vmc_lexer_get_metadata(l, &line, &line_offset, &offset);
	return _vmc_add_message(c, VMC_ERROR_CODE_INVALID_SEGMENT, "unknown segment type %.*s at %d:%d",
		vm_string_length(&token->string), token->string.start,
		line, line_offset);
}

BOOL _vmc_add_error_out_of_memory(vmc_compiler* c, vmc_lexer* l, vmc_lexer_token* token)
{
	int line, line_offset, offset;
	vmc_lexer_get_metadata(l, &line, &line_offset, &offset);
	return _vmc_add_message(c, VMC_CERR_OUT_OF_MEMORY, "out of memory");
}

BOOL vmc_error_symbol_already_exists(vmc_compiler* c, vmc_lexer* l, vmc_lexer_token* token)
{
	int line, line_offset, offset;
	vmc_lexer_get_metadata(l, &line, &line_offset, &offset);
	return _vmc_add_message(c, VMC_CERR_SYMBOL_ALREADY_EXIST, "symbol already exists: '%.*s' at %d:%d",
		vm_string_length(&token->string), token->string.start,
		line, line_offset);
}

void _vmc_emit_begin(vmc_compiler* c, vm_int8 argument_total_size, vm_int8 return_total_size)
{
	vmi_instr_begin instr;
	instr.opcode = 0;
	instr.icode = VMI_BEGIN;
	instr.argument_total_size = argument_total_size;
	instr.return_total_size = return_total_size;
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

vmc_package* _vmc_package_malloc(const char* name, int length)
{
	vmc_package* p = (vmc_package*)malloc(sizeof(vmc_package));
	if (p == NULL)
		return NULL;
	p->name.start = name;
	p->name.end = name + length;
	p->full_name = p->name;
	p->func_first = p->func_last = NULL;
	p->type_first = p->type_last = NULL;
	p->root_package = NULL;
	p->next = NULL;
	return p;
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
	vmc_write(c, &header, sizeof(header));
}

void _vmc_compiler_scope_init(vmc_scope* s, vmc_scope* parent)
{
	s->parent = parent;
	s->next = s->prev = NULL;
}

vm_int32 _vmc_calculate_var_offset(const vmc_var_definition* defs, vm_int32 count) {
	vm_int32 i, offset = 0;
	for (i = 0; i < count; ++i) {
		offset += defs[i].type.size;
	}
	return offset;
}

BOOL _vmc_parse_keyword_fn_args(vmc_compiler* c, vmc_lexer* l, vmc_package* p, vmc_lexer_token* t, vmc_func* func)
{
	// Expected (
	vmc_lexer_next(l, t);
	if (t->type != VMC_LEXER_TYPE_PARAN_L)
		return _vmc_add_error_expected_parant(c, l, t);
	vmc_lexer_next(l, t);

	// Empty arguments
	if (t->type == VMC_LEXER_TYPE_PARAN_R) {
		vmc_lexer_next(l, t);
		return TRUE;
	}

	// Collect information of all arguments
	func->args_count = 0;
	func->args_total_size = 0;
	while (1)
	{
		vmc_package* type_package = p;
		vmc_var_definition* var = &func->args[func->args_count];
		vmc_type_definition* type_definition;

		// Reset masks
		var->type.masks = 0;
		var->type.offset = _vmc_calculate_var_offset(func->args, func->args_count);

		// Read var name
		if (t->type != VMC_LEXER_TYPE_KEYWORD)
			return _vmc_add_error_expected_identifier(c, l, t);
		var->name = t->string;

		// The type might be a '*', '[]', package or type
		vmc_lexer_next(l, t);

		// * (memory address)
		if (t->type == VMC_LEXER_TYPE_PTR) {
			var->type.masks |= VMC_TYPE_INFO_MASK_ADDR;
			vmc_lexer_next(l, t);
		}

		// Expected type
		if (t->type != VMC_LEXER_TYPE_KEYWORD)
			return _vmc_add_error_expected_type(c, l, t);

		// If the next character is a package delimiter than figure out the package
		//if (vmc_lexer_peek(l) == '.') {
		//	vmc_lexer_next(l, t);
		//	vmc_lexer_next(l, t);
			//type_package = find_package_among_imports(...);
		//}
		type_definition = vmc_package_find_type(type_package, &t->string);
		if (type_definition == NULL)
			return _vmc_add_error_type_not_found(c, l, t);
		var->type.definition = type_definition;
		
		// Figure out the size of the variable
		if ((var->type.masks & VMC_TYPE_INFO_MASK_ADDR) == VMC_TYPE_INFO_MASK_ADDR)
			var->type.size = sizeof(void*);
		else
			var->type.size = type_definition->size;

		// Argument now loaded
		func->args_count++;
		func->args_total_size += var->type.size;

		vmc_lexer_next(l, t);
		if (t->type == VMC_LEXER_TYPE_COMMA) {
			vmc_lexer_next(l, t);
		}
		else if (t->type == VMC_LEXER_TYPE_PARAN_R) {
			vmc_lexer_next(l, t);
			break;
		}
	}

	return TRUE;
}

BOOL _vmc_parse_keyword_fn_rets(vmc_compiler* c, vmc_lexer* l, vmc_package* p, vmc_lexer_token* t, vmc_func* func)
{
	func->returns_count = 0;
	func->returns_total_size = 0;

	if (t->type != VMC_LEXER_TYPE_PARAN_L)
		return _vmc_add_error_expected_parant(c, l, t);
	vmc_lexer_next(l, t);
	
	// Did we close the opened parantheses immediately?
	if (t->type == VMC_LEXER_TYPE_PARAN_R) {	
		vmc_lexer_next(l, t);
		return TRUE;
	}

	// Parse each return type 
	while (1) {
		vmc_package* type_package = p;
		vmc_var_definition* var = &func->returns[func->returns_count];
		vmc_type_definition* type_definition;

		// Reset masks
		var->type.masks = 0;
		var->type.offset = _vmc_calculate_var_offset(func->returns, func->returns_count);

		// We might've prefixed the type with a modifier, such as a pointer
		if (t->type == VMC_LEXER_TYPE_PTR) {
			var->type.masks |= VMC_TYPE_INFO_MASK_ADDR;
			vmc_lexer_next(l, t);
		}

		// Expected type
		if (t->type != VMC_LEXER_TYPE_KEYWORD)
			return _vmc_add_error_expected_type(c, l, t);

		// If the next character is a package delimiter than figure out the package
		//if (vmc_lexer_peek(l) == '.') {
		//	vmc_lexer_next(l, t);
		//	vmc_lexer_next(l, t);
			//type_package = find_package_among_imports(...);
		//}
		type_definition = vmc_package_find_type(type_package, &t->string);
		if (type_definition == NULL)
			return _vmc_add_error_type_not_found(c, l, t);
		var->type.definition = type_definition;

		// Figure out the size of the variable
		if ((var->type.masks & VMC_TYPE_INFO_MASK_ADDR) == VMC_TYPE_INFO_MASK_ADDR)
			var->type.size = sizeof(void*);
		else
			var->type.size = type_definition->size;

		// Argument now loaded
		func->returns_count++;
		func->returns_total_size += var->type.size;

		vmc_lexer_next(l, t);
		if (t->type == VMC_LEXER_TYPE_COMMA) {
			vmc_lexer_next(l, t);
		}
		else if (t->type == VMC_LEXER_TYPE_PARAN_R) {
			vmc_lexer_next(l, t);
			break;
		}
	}

	return TRUE;
}

BOOL _vmc_parse_keyword_fn_body(vmc_compiler* c, vmc_lexer* l, vmc_package* p, vmc_lexer_token* t, vmc_func* func)
{
	// If function is external then allow body to be missing.
	if (func->is_extern && t->type != VMC_LEXER_TYPE_BRACKET_L)
		return TRUE;

	// The function is actually beginning here
	// TODO: Add support for reserving memory for local variables
	if (func->args_total_size > INT8_MAX)
		return _vmc_add_error_not_implemented_yet(c, l, t);
	if (func->returns_total_size > INT8_MAX)
		return _vmc_add_error_not_implemented_yet(c, l, t);
	_vmc_emit_begin(c, func->args_total_size, func->returns_total_size);

	while (1) {
		// Unexpected end of function body
		if (t->type == VMC_LEXER_TYPE_EOF) {
			return _vmc_add_error_incomplete_fn_body(c, l, t);
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

		// Verify that a keyword is being processed
		if (!vmc_lexer_type_is_keyword(t->type)) {
			return _vmc_add_error_expected_keyword(c, l, t);
		}

		if (vm_string_cmp(&t->string, VM_STRING_CONST_GET(load_a))) {
			// load_a <i32>

			vmi_instr_load_a instr;
			vm_int32 index;

			vmc_lexer_next(l, t);
			if (t->type != VMC_LEXER_TYPE_INT) {
				return _vmc_add_error_expected_index(c, l, t);
			}

			index = (vm_int32)strtoi64(t->string.start, vm_string_length(&t->string));
			if (func->args_count <= index) {
				return _vmc_add_error_invalid_arg(c, l, index, func->args_count);
			}
			instr.opcode = 0;
			instr.icode = VMI_LOAD_A;
			instr.size = func->args[index].type.size;
			instr.offset = func->args[index].type.offset;
			vmc_write(c, &instr, sizeof(vmi_instr_load_a));
		}
		else if (vm_string_cmp(&t->string, VM_STRING_CONST_GET(const))) {
			// const <type> <value>

			vmc_lexer_next(l, t);
			if (t->type != VMC_LEXER_TYPE_KEYWORD) {
				return _vmc_add_error_expected_index(c, l, t);
			}

			if (vm_string_cmp(&t->string, VM_STRING_CONST_GET(int32))) {
				vmi_instr_const_int32 instr;
				vmc_lexer_next(l, t);
				if (t->type != VMC_LEXER_TYPE_INT)
					return _vmc_add_error_expected_integer(c, l, t);
				instr.value = (vm_int32)strtoi64(t->string.start, vm_string_length(&t->string));
				instr.opcode = 0;
				instr.icode = VMI_CONST;
				vmc_write(c, &instr, sizeof(vmi_instr_const_int32));
			}
			else
				return _vmc_add_error_not_implemented_yet(c, l, t);			
		}
		else if (vm_string_cmp(&t->string, VM_STRING_CONST_GET(add))) {
			// add <type>

			vmi_opcode opcode = VMI_ADD;

			vmc_lexer_next(l, t);
			if (t->type != VMC_LEXER_TYPE_KEYWORD)
				return _vmc_add_error_expected_type(c, l, t);
			if (vm_string_cmp(&t->string, VM_STRING_CONST_GET(int32)))
				opcode |= VMI_PROPS1_OPCODE(0);
			_vmc_emit_opcode(c, opcode);
		}
		else if (vm_string_cmp(&t->string, VM_STRING_CONST_GET(save_r))) {
			// saver <i32>

			vmi_instr_save_r instr;
			vm_int32 index;

			vmc_lexer_next(l, t);
			if (t->type != VMC_LEXER_TYPE_INT) {
				return _vmc_add_error_expected_index(c, l, t);
			}

			index = (vm_int32)strtoi64(t->string.start, vm_string_length(&t->string));
			if (func->returns_count <= index) {
				return _vmc_add_error_invalid_arg(c, l, index, func->returns_count);
			}
			instr.opcode = 0;
			instr.icode = VMI_SAVE_R;
			instr.size = func->returns[index].type.size;
			instr.offset = func->returns[index].type.offset;
			vmc_write(c, &instr, sizeof(vmi_instr_save_r));
		}
		else if (vm_string_cmp(&t->string, VM_STRING_CONST_GET(ret))) {
			// ret

			vmi_instr_ret instr;
			instr.opcode = 0;
			instr.icode = VMI_RET;
			vmc_write(c, &instr, sizeof(vmi_instr_ret));
		}
		else if (vm_string_cmp(&t->string, VM_STRING_CONST_GET(call))) {
			// call <definition>

			return _vmc_add_error_not_implemented_yet(c, l, t);
		}

		vmc_lexer_next(l, t);
	}

	vmc_lexer_next(l, t);
	return TRUE;
}

// Append a new function to the current package. The syntax is:
// [extern] fn name (arg1 [modifier][package.]type1, arg2 [modifier][package.]type2, ...) ([modifier][package.]ret1, [modifier][package.]ret2, ...)
//
// If the function is external then a body is not required. You are allowed to have a body if you want to override the 
BOOL _vmc_parse_keyword_fn(vmc_compiler* c, vmc_lexer* l, vmc_package* p, vmc_lexer_token* t, BOOL is_extern)
{
	vmc_func* func;
	int func_ret;

	// Expected an function name
	vmc_lexer_next(l, t);
	if (t->type != VMC_LEXER_TYPE_KEYWORD)
		return _vmc_add_error_expected_identifier(c, l, t);

	// Try to create a new function
	func_ret = vmc_func_try_create(p, &t->string, vm_bytestream_get_size(&c->bytecode), &func);
	switch (func_ret)
	{
	case VMC_FUNC_TRY_CREATE_OOM:
		return _vmc_add_error_out_of_memory(c, l, t);
	case VMC_FUNC_TRY_CREATE_ALREADY_EXISTS:
		return vmc_error_symbol_already_exists(c, l, t);
	default:
		break;
	}
	func->is_extern = is_extern;

	// Parse args
	if (!_vmc_parse_keyword_fn_args(c, l, p, t, func))
		return FALSE;
	
	// Parse return types
	if (!_vmc_parse_keyword_fn_rets(c, l, p, t, func))
		return FALSE;

	if (t->type != VMC_LEXER_TYPE_BRACKET_L) {
		// The next token is assumed to not be part of the function if it's modified with an "extern" modifier
		if (is_extern) {
			return TRUE;
		}
		else {
			return _vmc_add_error_expected_bracket(c, l, t);
		}
	}
	vmc_lexer_next(l, t);
	
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
		return _vmc_parse_keyword_fn(c, l, p, t, TRUE);
	default:
		_vmc_add_error_extern_unknown(c, l, t);
		return FALSE;
	}
}

BOOL _vmc_parse_keyword(vmc_compiler* c, vmc_lexer* l, vmc_package* p, vmc_lexer_token* t)
{
	switch (t->type)
	{
	case VMC_LEXER_TYPE_KEYWORD_EXTERN:
		return _vmc_parse_keyword_extern(c, l, p, t);
	case VMC_LEXER_TYPE_KEYWORD_FN:
		return _vmc_parse_keyword_fn(c, l, p, t, FALSE);
	case VMC_LEXER_TYPE_KEYWORD_CONST:
	default:
		break;
	}

	_vmc_add_error_unknown_token(c, l, t);
	_vmc_emit_icode(c, VMI_EOE);
	return FALSE;
}

// Take all messages from the lexer
void _vmc_take_messages(vmc_compiler* c, vmc_lexer* l) {
	vm_message* message_first;
	vm_message* message_last;
	if (vmc_lexer_success(l))
		return;

	message_first = l->messages_first;
	message_last = l->messages_last;
	l->messages_first = l->messages_last = NULL;
	
	if (c->messages_last == NULL) {
		c->messages_first = message_first;
		c->messages_last = message_last;
	}
	else {
		c->messages_last->next = message_first;
		c->messages_last = message_last;
	}
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
			_vmc_add_error_unknown_token(c, l, &token);
			_vmc_emit_icode(c, VMI_EOE);
			return;
		}
		vmc_lexer_next(l, &token);
	}
}

void _vmc_compiler_register_builtins(vmc_compiler* c)
{
	// Register the "vm" package and all type definitions
	c->package_first = c->package_last = _vmc_package_malloc("vm", 2);
	vmc_type_definition_new(c->package_first, VM_STRING_CONST_GET(int32), sizeof(vm_int32));
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
	c->messages_first = c->messages_last = NULL;
	c->scope_first = c->scope_last = NULL;
	c->package_first = c->package_last = NULL;
	_vmc_compiler_register_builtins(c);
	vmc_package_new(c, "main", 4);
	return c;
}

void vmc_package_destroy(vmc_package* p)
{
	vmc_func* f;
	vmc_type_definition* t;

	// Cleanup functions
	f = p->func_first;
	while (f != NULL) {
		vmc_func* next = f->next;
		free(f);
		f = next;
	}

	// Cleanup types
	t = p->type_first;
	while (t != NULL) {
		vmc_type_definition* next = t->next;
		free(t);
		t = next;
	}

	free(p);
}

void vmc_compiler_destroy(vmc_compiler* c)
{
	vm_message* m;
	vmc_scope* s;
	vmc_package* p;

	// Cleanup messages
	m = c->messages_first;
	while (m != NULL) {
		vm_message* next = m->next;
		free(m);
		m = next;
	}
	c->messages_first = NULL;

	// Cleanup scopes
	s = c->scope_first;
	while (s != NULL) {
		vmc_scope* next = s->next;
		free(s);
		s = next;
	}
	c->scope_first = NULL;

	// Cleanup packages
	p = c->package_first;
	while (p != NULL) {
		vmc_package* next = p->next;
		vmc_package_destroy(p);
		p = next;
	}
	c->package_first = NULL;

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
	_vmc_take_messages(c, &l);
	vmc_lexer_release(&l);
	return vmc_compiler_success(c);
}

vmc_scope* vmc_scope_push(vmc_compiler* c, vmc_scope* parent_scope)
{
	vmc_scope* scope;

	// Extract the last scope if one exists
	if (c->scope_last != NULL) {
		scope = c->scope_last;
		c->scope_last = c->scope_last->prev;
		c->scope_last->next = NULL;
		_vmc_compiler_scope_init(scope, parent_scope);
		return scope;
	}

	scope = (vmc_scope*)malloc(sizeof(vmc_scope));
	_vmc_compiler_scope_init(scope, parent_scope);
	return scope;
}

vmc_scope* vmc_scope_pop(vmc_compiler* c, vmc_scope* scope)
{
	if (c->scope_last == NULL) {
		c->scope_first = c->scope_last = scope;
		return scope->parent;
	}
	c->scope_last->next = scope;
	scope->prev = c->scope_last;
	c->scope_last = scope;
	return scope->parent;
}

vm_int32 vmc_compiler_config_import(struct vmc_compiler* c, const vm_string* path)
{
	return 0;
}

vmc_package* vmc_package_new(vmc_compiler* c, const char* name, int length)
{
	vmc_package* p = _vmc_package_malloc(name, length);
	if (p == NULL)
		return NULL;
	// Add the package to the linked list. Assume that vm/root package is in the beginning
	p->root_package = c->package_first;
	c->package_last->next = p;
	c->package_last = p;
	return p;
}

vmc_type_definition* vmc_package_find_type(vmc_package* p, const vm_string* name)
{
	vmc_type_definition* type = p->type_first;
	while (type != NULL) {
		if (vm_string_cmp(&type->name, name))
			return type;
		type = type->next;
	}
	if (p->root_package) {
		return vmc_package_find_type(p->root_package, name);
	}
	return NULL;
}

int vmc_func_try_create(vmc_package* p, const vm_string* name, vm_int32 offset, vmc_func** func)
{
	*func = vmc_func_find(p, name);
	if (*func != NULL)
		return VMC_FUNC_TRY_CREATE_ALREADY_EXISTS;
	*func = vmc_func_new(p, name, offset);
	if (*func == NULL)
		return VMC_FUNC_TRY_CREATE_OOM;
	return VMC_FUNC_TRY_CREATE_OK;
}

vmc_func* vmc_func_find(vmc_package* p, const vm_string* name)
{
	vmc_func* func = p->func_first;
	while (func != NULL) {
		if (vm_string_cmp(&func->name, name)) {
			return func;
		}
		func = func->next;
	}
	return NULL;
}

vmc_func* vmc_func_new(vmc_package* p, const vm_string* name, vm_int32 offset)
{
	vmc_func* func = (vmc_func*)malloc(sizeof(vmc_func));
	if (func == NULL)
		return NULL;
	func->package = p;
	func->name = *name;
	func->offset = offset;
	func->next = NULL;
	func->is_extern = FALSE;
	func->is_public = vmc_lexer_test_uppercase(*name->start); // TODO: A function should not be public 
	func->complexity = 0;
	func->complexity_components = 0;
	func->args_count = 0;
	func->returns_count = 0;
	if (p->func_last == NULL) {
		p->func_first = p->func_last = func;
	}
	else {
		p->func_last->next = func;
		p->func_last = func;
	}
	return func;
}

vmc_type_definition* vmc_type_definition_new(vmc_package* p, const vm_string* name, vm_int32 size)
{
	vmc_type_definition* type = (vmc_type_definition*)malloc(sizeof(vmc_type_definition));
	if (type == NULL)
		return type;
	type->name = *name;
	type->package = p;
	type->size = size;
	type->next = NULL;
	if (p->type_last == NULL) {
		p->type_first = p->type_last = type;
	}
	else {
		p->type_last->next = type;
		p->type_last = type;
	}
	return type;
}
