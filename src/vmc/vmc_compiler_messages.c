#include "vmc_compiler_messages.h"

BOOL vmc_compiler_message_panic(const vmc_compiler_scope* s, const char* str)
{
	vm_message* const m = &s->compiler->panic_error_message;
	m->prefix = VMC_COMPILER_MESSAGE_PREFIX;
	m->code = VMC_COMPILER_MSG_PANIC;
	m->next = NULL;
	m->line = m->line_offset = m->offset = 0;
	sprintf(m->message, str);
	return FALSE;
}

BOOL vmc_compiler_message_unknown_token(const vmc_compiler_scope* s)
{
	const vmc_lexer_token* const t = s->token;
	vm_messages* const m = &s->compiler->messages;

	int line, line_offset, _;
	vmc_lexer_get_metadata(t, &line, &line_offset, &_);
	return vm_messages_add(m,
		VMC_COMPILER_MESSAGE_PREFIX,
		VMC_COMPILER_MSG_UNKNOWN_TOKEN,
		VMC_COMPILER_MSG_UNKNOWN_TOKEN_STR,
		vm_string_length(&t->string), t->string.start, line, line_offset);
}

BOOL vmc_compiler_message_expected_identifier(const vmc_compiler_scope* s)
{
	vm_messages* const m = &s->compiler->messages;
	const vmc_lexer_token* const t = s->token;

	int line, line_offset, _;
	vmc_lexer_get_metadata(t, &line, &line_offset, &_);
	return vm_messages_add(m,
		VMC_COMPILER_MESSAGE_PREFIX,
		VMC_COMPILER_MSG_EXPECTED_IDENTIFIER,
		VMC_COMPILER_MSG_EXPECTED_IDENTIFIER_STR,
		vm_string_length(&t->string), t->string.start, line, line_offset);
}

BOOL vmc_compiler_message_expected_type(const vmc_compiler_scope* s)
{
	vm_messages* const m = &s->compiler->messages;
	const vmc_lexer_token* const t = s->token;

	int line, line_offset, _;
	vmc_lexer_get_metadata(t, &line, &line_offset, &_);
	return vm_messages_add(m,
		VMC_COMPILER_MESSAGE_PREFIX,
		VMC_COMPILER_MSG_EXPECTED_TYPE,
		VMC_COMPILER_MSG_EXPECTED_TYPE_STR,
		vm_string_length(&t->string), t->string.start, line, line_offset);
}

BOOL vmc_compiler_message_type_not_found(const vmc_compiler_scope* s)
{
	vm_messages* const m = &s->compiler->messages;
	const vmc_lexer_token* const t = s->token;

	int line, line_offset, _;
	vmc_lexer_get_metadata(t, &line, &line_offset, &_);
	return vm_messages_add(m,
		VMC_COMPILER_MESSAGE_PREFIX,
		VMC_COMPILER_MSG_TYPE_NOT_FOUND,
		VMC_COMPILER_MSG_TYPE_NOT_FOUND_STR,
		vm_string_length(&t->string), t->string.start, line, line_offset);
}

BOOL vmc_compiler_message_expected_keyword(const vmc_compiler_scope* s)
{
	vm_messages* const m = &s->compiler->messages;
	const vmc_lexer_token* const t = s->token;

	int line, line_offset, _;
	vmc_lexer_get_metadata(t, &line, &line_offset, &_);
	return vm_messages_add(m,
		VMC_COMPILER_MESSAGE_PREFIX,
		VMC_COMPILER_MSG_EXPECTED_KEYWORD,
		VMC_COMPILER_MSG_EXPECTED_KEYWORD_STR,
		vm_string_length(&t->string), t->string.start, line, line_offset);
}

BOOL vmc_compiler_message_expected_index(const vmc_compiler_scope* s)
{
	vm_messages* const m = &s->compiler->messages;
	const vmc_lexer_token* const t = s->token;

	int line, line_offset, _;
	vmc_lexer_get_metadata(t, &line, &line_offset, &_);
	return vm_messages_add(m,
		VMC_COMPILER_MESSAGE_PREFIX,
		VMC_COMPILER_MSG_EXPECTED_INDEX,
		VMC_COMPILER_MSG_EXPECTED_INDEX_STR,
		vm_string_length(&t->string), t->string.start, line, line_offset);
}

BOOL vmc_compiler_message_expected_int(const vmc_compiler_scope* s)
{
	const vmc_lexer_token* const t = s->token;
	vm_messages* const m = &s->compiler->messages;

	int line, line_offset, _;
	vmc_lexer_get_metadata(t, &line, &line_offset, &_);
	return vm_messages_add(m,
		VMC_COMPILER_MESSAGE_PREFIX,
		VMC_COMPILER_MSG_EXPECTED_INT,
		VMC_COMPILER_MSG_EXPECTED_INT_STR,
		vm_string_length(&t->string), t->string.start, line, line_offset);
}

BOOL vmc_compiler_message_expected_string(const vmc_compiler_scope* s)
{
	const vmc_lexer_token* const t = s->token;
	vm_messages* const m = &s->compiler->messages;

	int line, line_offset, _;
	vmc_lexer_get_metadata(t, &line, &line_offset, &_);
	return vm_messages_add(m,
		VMC_COMPILER_MESSAGE_PREFIX,
		VMC_COMPILER_MSG_EXPECTED_STRING,
		VMC_COMPILER_MSG_EXPECTED_STRING_STR,
		vm_string_length(&t->string), t->string.start, line, line_offset);
}

BOOL vmc_compiler_message_expected_decimal(const vmc_compiler_scope* s)
{
	const vmc_lexer_token* const t = s->token;
	vm_messages* const m = &s->compiler->messages;

	int line, line_offset, _;
	vmc_lexer_get_metadata(t, &line, &line_offset, &_);
	return vm_messages_add(m,
		VMC_COMPILER_MESSAGE_PREFIX,
		VMC_COMPILER_MSG_EXPECTED_DECIMAL,
		VMC_COMPILER_MSG_EXPECTED_DECIMAL_STR,
		vm_string_length(&t->string), t->string.start, line, line_offset);
}

BOOL vmc_compiler_message_not_implemented(const vmc_compiler_scope* s)
{
	const vmc_lexer_token* const t = s->token;
	vm_messages* const m = &s->compiler->messages;

	int line, line_offset, _;
	vmc_lexer_get_metadata(t, &line, &line_offset, &_);
	return vm_messages_add(m,
		VMC_COMPILER_MESSAGE_PREFIX,
		VMC_COMPILER_MSG_NOT_IMPLEMENTED,
		VMC_COMPILER_MSG_NOT_IMPLEMENTED_STR,
		vm_string_length(&t->string), t->string.start, line, line_offset);
}

BOOL vmc_compiler_message_invalid_index(const vmc_compiler_scope* s, vm_int32 index, vm_int32 min, vm_int32 max)
{
	const vmc_lexer_token* const t = s->token;
	vm_messages* const m = &s->compiler->messages;

	int line, line_offset, _;
	vmc_lexer_get_metadata(t, &line, &line_offset, &_);
	return vm_messages_add(m,
		VMC_COMPILER_MESSAGE_PREFIX,
		VMC_COMPILER_MSG_INVALID_INDEX,
		VMC_COMPILER_MSG_INVALID_INDEX_STR,
		index, min, max, line, line_offset);
}

BOOL vmc_compiler_message_invalid_size(const vmc_compiler_scope* s, vm_int32 size, vm_int32 min, vm_int32 max)
{
	const vmc_lexer_token* const t = s->token;
	vm_messages* const m = &s->compiler->messages;

	int line, line_offset, _;
	vmc_lexer_get_metadata(t, &line, &line_offset, &_);
	return vm_messages_add(m,
		VMC_COMPILER_MESSAGE_PREFIX,
		VMC_COMPILER_MSG_INVALID_SIZE,
		VMC_COMPILER_MSG_INVALID_SIZE_STR,
		size, min, max, line, line_offset);
}

BOOL vmc_compiler_message_unexpected_eof(const vmc_compiler_scope* s)
{
	vm_messages* const m = &s->compiler->messages;
	const vmc_lexer_token* const t = s->token;

	int line, line_offset, _;
	vmc_lexer_get_metadata(t, &line, &line_offset, &_);
	return vm_messages_add(m,
		VMC_COMPILER_MESSAGE_PREFIX,
		VMC_COMPILER_MSG_UNEXPECTED_EOF,
		VMC_COMPILER_MSG_UNEXPECTED_EOF_STR,
		line, line_offset);
}

BOOL vmc_compiler_message_syntax_error(const vmc_compiler_scope* s, char expected)
{
	vm_messages* const m = &s->compiler->messages;
	const vmc_lexer_token* const t = s->token;

	int line, line_offset, _;
	vmc_lexer_get_metadata(t, &line, &line_offset, &_);
	return vm_messages_add(m,
		VMC_COMPILER_MESSAGE_PREFIX,
		VMC_COMPILER_MSG_SYNTAX_ERROR,
		VMC_COMPILER_MSG_SYNTAX_ERROR_STR,
		expected, vm_string_length(&t->string), t->string.start, line, line_offset);
}

BOOL vmc_compiler_message_func_body_exists(const vmc_compiler_scope* s, const vm_string* signature)
{
	vm_messages* const m = &s->compiler->messages;
	const vmc_lexer_token* const t = s->token;

	int line, line_offset, _;
	vmc_lexer_get_metadata(t, &line, &line_offset, &_);
	return vm_messages_add(m,
		VMC_COMPILER_MESSAGE_PREFIX,
		VMC_COMPILER_MSG_FUNC_BODY_EXISTS,
		VMC_COMPILER_MSG_FUNC_BODY_EXISTS_STR,
		vm_string_length(signature), signature->start, line, line_offset);
}

BOOL vmc_compiler_message_memory_marker_exists(const vmc_compiler_scope* s, const vm_string* memory_marker)
{
	vm_messages* const m = &s->compiler->messages;
	const vmc_lexer_token* const t = s->token;

	int line, line_offset, _;
	vmc_lexer_get_metadata(t, &line, &line_offset, &_);
	return vm_messages_add(m,
		VMC_COMPILER_MESSAGE_PREFIX,
		VMC_COMPILER_MSG_MEMORY_MARKER_ALREADY_EXIST,
		VMC_COMPILER_MSG_MEMORY_MARKER_ALREADY_EXIST_STR,
		vm_string_length(memory_marker), memory_marker->start, line, line_offset);
}

BOOL vmc_compiler_message_defarray_to_small(const vmc_compiler_scope* s, vm_int32 size)
{
	vm_messages* const m = &s->compiler->messages;
	const vmc_lexer_token* const t = s->token;

	int line, line_offset, _;
	vmc_lexer_get_metadata(t, &line, &line_offset, &_);
	return vm_messages_add(m,
		VMC_COMPILER_MESSAGE_PREFIX,
		VMC_COMPILER_MSG_DEFARRAY_TOO_SMALL,
		VMC_COMPILER_MSG_DEFARRAY_TOO_SMALL_STR,
		size, line, line_offset);
}

BOOL vmc_compiler_message_unexpected_newline(const vmc_compiler_scope* s)
{
	vm_messages* const m = &s->compiler->messages;
	const vmc_lexer_token* const t = s->token;

	int line, line_offset, _;
	vmc_lexer_get_metadata(t, &line, &line_offset, &_);
	return vm_messages_add(m,
		VMC_COMPILER_MESSAGE_PREFIX,
		VMC_COMPILER_MSG_UNEXPECTED_NEWLINE,
		VMC_COMPILER_MSG_UNEXPECTED_NEWLINE_STR,
		line, line_offset);
}
