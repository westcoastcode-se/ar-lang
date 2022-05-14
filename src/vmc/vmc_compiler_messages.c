#include "vmc_compiler_messages.h"

BOOL vmc_compiler_message_unknown_token(vm_messages* m, vmc_lexer* l, vmc_lexer_token* t)
{
	int line, line_offset, _;
	vmc_lexer_get_metadata(l, &line, &line_offset, &_);
	return vm_messages_add(m,
		VMC_COMPILER_MESSAGE_PREFIX,
		VMC_COMPILER_MSG_UNKNOWN_TOKEN,
		VMC_COMPILER_MSG_UNKNOWN_TOKEN_STR,
		vm_string_length(&t->string), t->string.start, line, line_offset);
}

BOOL vmc_compiler_message_expected_identifier(vm_messages* m, vmc_lexer* l, vmc_lexer_token* t)
{
	int line, line_offset, _;
	vmc_lexer_get_metadata(l, &line, &line_offset, &_);
	return vm_messages_add(m,
		VMC_COMPILER_MESSAGE_PREFIX,
		VMC_COMPILER_MSG_EXPECTED_IDENTIFIER,
		VMC_COMPILER_MSG_EXPECTED_TYPE_STR,
		vm_string_length(&t->string), t->string.start, line, line_offset);
}

BOOL vmc_compiler_message_expected_type(vm_messages* m, vmc_lexer* l, vmc_lexer_token* t)
{
	int line, line_offset, _;
	vmc_lexer_get_metadata(l, &line, &line_offset, &_);
	return vm_messages_add(m,
		VMC_COMPILER_MESSAGE_PREFIX,
		VMC_COMPILER_MSG_EXPECTED_TYPE,
		VMC_COMPILER_MSG_EXPECTED_TYPE_STR,
		vm_string_length(&t->string), t->string.start, line, line_offset);
}

BOOL vmc_compiler_message_type_not_found(vm_messages* m, vmc_lexer* l, vmc_lexer_token* t)
{
	int line, line_offset, _;
	vmc_lexer_get_metadata(l, &line, &line_offset, &_);
	return vm_messages_add(m,
		VMC_COMPILER_MESSAGE_PREFIX,
		VMC_COMPILER_MSG_TYPE_NOT_FOUND,
		VMC_COMPILER_MSG_TYPE_NOT_FOUND_STR,
		vm_string_length(&t->string), t->string.start, line, line_offset);
}

BOOL vmc_compiler_message_expected_keyword(vm_messages* m, vmc_lexer* l, vmc_lexer_token* t)
{
	int line, line_offset, _;
	vmc_lexer_get_metadata(l, &line, &line_offset, &_);
	return vm_messages_add(m,
		VMC_COMPILER_MESSAGE_PREFIX,
		VMC_COMPILER_MSG_EXPECTED_KEYWORD,
		VMC_COMPILER_MSG_EXPECTED_KEYWORD_STR,
		vm_string_length(&t->string), t->string.start, line, line_offset);
}

BOOL vmc_compiler_message_expected_index(vm_messages* m, vmc_lexer* l, vmc_lexer_token* t)
{
	int line, line_offset, _;
	vmc_lexer_get_metadata(l, &line, &line_offset, &_);
	return vm_messages_add(m,
		VMC_COMPILER_MESSAGE_PREFIX,
		VMC_COMPILER_MSG_EXPECTED_INDEX,
		VMC_COMPILER_MSG_EXPECTED_INDEX_STR,
		vm_string_length(&t->string), t->string.start, line, line_offset);
}

BOOL vmc_compiler_message_expected_int(vm_messages* m, vmc_lexer* l, vmc_lexer_token* t)
{
	int line, line_offset, _;
	vmc_lexer_get_metadata(l, &line, &line_offset, &_);
	return vm_messages_add(m,
		VMC_COMPILER_MESSAGE_PREFIX,
		VMC_COMPILER_MSG_EXPECTED_INT,
		VMC_COMPILER_MSG_EXPECTED_INT_STR,
		vm_string_length(&t->string), t->string.start, line, line_offset);
}

BOOL vmc_compiler_message_not_implemented(vm_messages* m, vmc_lexer* l, vmc_lexer_token* t)
{
	int line, line_offset, _;
	vmc_lexer_get_metadata(l, &line, &line_offset, &_);
	return vm_messages_add(m,
		VMC_COMPILER_MESSAGE_PREFIX,
		VMC_COMPILER_MSG_NOT_IMPLEMENTED,
		VMC_COMPILER_MSG_NOT_IMPLEMENTED_STR,
		vm_string_length(&t->string), t->string.start, line, line_offset);
}

BOOL vmc_compiler_message_invalid_index(vm_messages* m, vmc_lexer* l, vm_int32 index, vm_int32 min, vm_int32 max)
{
	int line, line_offset, _;
	vmc_lexer_get_metadata(l, &line, &line_offset, &_);
	return vm_messages_add(m,
		VMC_COMPILER_MESSAGE_PREFIX,
		VMC_COMPILER_MSG_INVALID_INDEX,
		VMC_COMPILER_MSG_INVALID_INDEX_STR,
		index, min, max, line, line_offset);
}

BOOL vmc_compiler_message_incomplete_body(vm_messages* m, vmc_lexer* l, vmc_lexer_token* t)
{
	int line, line_offset, _;
	vmc_lexer_get_metadata(l, &line, &line_offset, &_);
	return vm_messages_add(m,
		VMC_COMPILER_MESSAGE_PREFIX,
		VMC_COMPILER_MSG_INCOMPLETE_BODY,
		VMC_COMPILER_MSG_INCOMPLETE_BODY_STR,
		line, line_offset);
}
