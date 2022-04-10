#include "vm_compiler.h"
#include "../interpreter/vmi_ops.h"
#include "../interpreter/vmi_process.h"
#include "eoe.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

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

	if (c->last_message == NULL)
		c->last_message = c->messages = m;
	else
		c->last_message->next = m;
	return FALSE;
}

BOOL _vmc_add_error_unknown_token(vmc_compiler* c, vml_token* token)
{
	int line, line_offset, offset;
	vmc_lexer_get_metadata(c->lexer, &line, &line_offset, &offset);
	_vmc_add_message(c, VMC_ERROR_CODE_UNKNOWN_TOKEN, "unknown symbol: '%c' was found at %d:%d", *token->string.start,
		line, line_offset);
	return FALSE;
}

void _vmc_append_header(vmc_compiler* c)
{
	vmi_process_header header;
	header.header[0] = 'S';
	header.header[1] = 'C';
	header.header[2] = 'R';
	header.version = VM_VERSION;
	header.data_offset = 0;
	header.code_offset = sizeof(vmi_process_header);
	vmc_write(c, &header, sizeof(header));
}

void _vmc_append_lex(vmc_compiler* c, vmc_lexer* l)
{
	vml_token token;

	// Ignore all newlines at the start of the content
	vmc_lexer_next(l, &token);
	while (token.type == VML_TYPE_NEWLINE) vmc_lexer_next(l, &token);

	// Compile each instruction
	while (1)
	{
		switch (token.type)
		{
		case VML_TYPE_NEWLINE:
		case VML_TYPE_COMMENT:
			break;
		case VML_TYPE_EOF:
			vmc_write_eoe(c);
			return;
		default:
			_vmc_add_error_unknown_token(c, &token);
			vmc_write_eoe(c);
			return;
		}
		vmc_lexer_next(l, &token);
	}
}

//
/////////////////////////////////////////////////////////////////////////////
// 

vmc_compiler* vmc_compiler_new(vmc_lexer* l)
{
	vmc_compiler* c = (vmc_compiler*)malloc(sizeof(vmc_compiler));
	if (c == NULL)
		return c;
	vm_bytestream_init(&c->bytecode);
	c->lexer = l;
	c->messages = c->last_message = NULL;
	return c;
}

BOOL vmc_compiler_compile(vmc_compiler* c)
{
	_vmc_append_header(c);
	_vmc_append_lex(c, c->lexer);
	return vmc_compiler_success(c);
}

vmc_compiler* vmc_compiler_compile_string(const vm_byte* src)
{
	vmc_lexer* l = vmc_lexer_parse(src);
	vmc_compiler* c = vmc_compiler_new(l);
	vmc_compiler_compile(c);
	vmc_lexer_destroy(l);
	return c;
}

void vmc_compiler_destroy(vmc_compiler* c)
{
	// Cleanup messages
	vm_message* m = c->messages;
	while (m != NULL) {
		vm_message* next = m->next;
		free(m);
		m = next;
	}

	vm_bytestream_release(&c->bytecode);
	free(c);
}
