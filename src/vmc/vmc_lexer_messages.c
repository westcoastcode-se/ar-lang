#include "vmc_lexer_messages.h"

BOOL vmc_lexer_message_unclosed_comment(vmc_lexer_token* l)
{
	int line, line_offset, _;
	vmc_lexer_get_metadata(l, &line, &line_offset, &_);
	return vm_messages_add(l->messages,
		VMC_LEXER_MESSAGE_PREFIX,
		VMC_LEXER_MSG_UNCLOSED_COMMENT,
		VMC_LEXER_MSG_UNCLOSED_COMMENT_STR,
		line, line_offset);
}

BOOL vmc_lexer_message_unclosed_string(vmc_lexer_token* l)
{
	int line, line_offset, _;
	vmc_lexer_get_metadata(l, &line, &line_offset, &_);
	return vm_messages_add(l->messages,
		VMC_LEXER_MESSAGE_PREFIX,
		VMC_LEXER_MSG_UNCLOSED_STRING,
		VMC_LEXER_MSG_UNCLOSED_STRING_STR,
		line, line_offset);
}

BOOL vmc_lexer_message_unknown_escaped_char(vmc_lexer_token* l, vm_byte rune)
{
	int line, line_offset, _;
	vmc_lexer_get_metadata(l, &line, &line_offset, &_);
	return vm_messages_add(l->messages,
		VMC_LEXER_MESSAGE_PREFIX,
		VMC_LEXER_MSG_UNKNOWN_ESCAPED_CHAR,
		VMC_LEXER_MSG_UNKNOWN_ESCAPED_CHAR_STR,
		rune, line, line_offset);
}
