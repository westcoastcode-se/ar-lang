#include "vmc_lexer.h"
#include "vmc_lexer_logic.h"
#include "vmc_lexer_messages.h"
#include "vmc_lexer_math.h"
#include "vmc_debug.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

void _vmc_lexer_atom(vmc_lexer_token_type type, vmc_lexer_token* token)
{
	token->type = type;
	token->modifier = 0;
	token->string.start = token->source;
	token->string.end = ++token->source;
}

void _vmc_lexer_unknown(vmc_lexer_token* token)
{
	token->string.start = token->source;
	token->string.end = token->source;
	token->type = VMC_LEXER_TYPE_UNKNOWN;
}

void _vmc_lexer_eof(vmc_lexer_token* t)
{
	t->string.start = t->source;
	t->string.end = t->source;
	t->type = VMC_LEXER_TYPE_EOF;
}

void _vmc_lexer_comment(vmc_lexer_token* t)
{
	const char* comment_start = t->source;
	char c = *t->source;

	// Seek until new-line or eof
	while (c != 0 && c != '\n')
		c = *++t->source;

	t->string.start = comment_start;
	t->string.end = t->source;
	t->type = VMC_LEXER_TYPE_COMMENT;
}

void _vmc_lexer_single_line_comment(vmc_lexer_token* t)
{
	const char* comment_start = ++t->source;
	char c = *comment_start;

	// Seek until new-line or eof
	while (c != 0 && c != '\n') {
		c = *(++t->source);
	}

	t->type = VMC_LEXER_TYPE_COMMENT;
	t->string.start = comment_start;
	t->string.end = t->source;
}

void _vmc_lexer_multi_line_comment(vmc_lexer_token* t)
{
	const char* comment_start = ++t->source;
	const char* commend_end = comment_start;
	char c = *comment_start;

	// Seek until */ or eof
	while (1) {
		if (c == 0) {
			commend_end = t->source;
			vmc_lexer_message_unclosed_comment(t);
			break;
		}
		// If we've reached */
		if (c == '*' && *(t->source + 1) == '/') {
			commend_end = t->source;
			t->source += 2;
			break;
		}
		c = *(++t->source);
	}

	t->type = VMC_LEXER_TYPE_COMMENT;
	t->modifier |= VMC_LEXER_TOKEN_MODIFIER_MULTILINE;
	t->string.start = comment_start;
	t->string.end = commend_end;
}

void _vmc_lexer_div_or_comment(vmc_lexer_token* t)
{
	const char* start = t->source++;
	char c = *t->source;

	switch (c)
	{
	case '/':
		_vmc_lexer_single_line_comment(t);
		return;
	case '*':
		_vmc_lexer_multi_line_comment(t);
		return;
	default:
		t->type = VMC_LEXER_TYPE_DIV;
		t->modifier = 0;
		t->string.start = start;
		t->string.end = t->source;
		return;
	}
}

void _vmc_lexer_bitand_or_and(vmc_lexer_token* t)
{
	const char* start = t->source++;
	char c = *t->source;

	switch (c)
	{
	case '&':
		t->source++;
		t->type = VMC_LEXER_TYPE_TEST_AND;
		t->modifier = 0;
		t->string.start = start;
		t->string.end = t->source;
		return;
	default:
		t->type = VMC_LEXER_TYPE_BIT_AND;
		t->modifier = 0;
		t->string.start = start;
		t->string.end = t->source;
		return;
	}
}

vmc_lexer_token_type _vmc_lexer_find_keyword_type(const vm_string* str)
{
	const int len = vm_string_length(str);
	if (len == 2) {
		if (vm_str_cmp(str->start, len, "fn", 2)) {
			return VMC_LEXER_TYPE_KEYWORD_FN;
		}
	}
	else if (len == 3) {
		if (vm_str_cmp(str->start, len, "var", 3)) {
			return VMC_LEXER_TYPE_KEYWORD_VAR;
		}
		else if (vm_str_cmp(str->start, len, "any", 3)) {
			return VMC_LEXER_TYPE_KEYWORD_ANY;
		}
	}
	else if (len == 4) {
		if (vm_str_cmp(str->start, len, "type", 4)) {
			return VMC_LEXER_TYPE_KEYWORD_TYPE;
		}
	}
	else if (len == 5) {
		if (vm_str_cmp(str->start, len, "const", 5)) {
			return VMC_LEXER_TYPE_KEYWORD_CONST;
		}
	}
	else if (len == 6) {
		if (vm_str_cmp(str->start, len, "import", 6)) {
			return VMC_LEXER_TYPE_KEYWORD_IMPORT;
		}
		else if (vm_str_cmp(str->start, len, "struct", 6)) {
			return VMC_LEXER_TYPE_KEYWORD_STRUCT;
		}
		else if (vm_str_cmp(str->start, len, "return", 6)) {
			return VMC_LEXER_TYPE_KEYWORD_RETURN;
		}
		else if (vm_str_cmp(str->start, len, "extern", 6)) {
			return VMC_LEXER_TYPE_KEYWORD_EXTERN;
		}
	}
	else if (len == 7) {
		if (vm_str_cmp(str->start, len, "package", 7)) {
			return VMC_LEXER_TYPE_KEYWORD_PACKAGE;
		}
	}
	else if (len == 9) {
		if (vm_str_cmp(str->start, len, "interface", 9)) {
			return VMC_LEXER_TYPE_KEYWORD_INTERFACE;
		}
	}
	return VMC_LEXER_TYPE_KEYWORD;
}

void _vmc_lexer_keyword(vmc_lexer_token* t)
{
	// Remember the first character and step to next char
	const char* start = t->source;
	t->source++;

	// Ignore all characters
	while (vmc_lexer_test_char(*t->source)) t->source++;

	t->string.start = start;
	t->string.end = t->source;
	t->type = _vmc_lexer_find_keyword_type(&t->string);
}

void _vmc_lexer_single_line_string(vmc_lexer_token* token)
{
	BOOL escaped = FALSE;
	const char* start = ++token->source;
	char c = *token->source;

	while (1) {
		if (c == 0) {
			vmc_lexer_message_unclosed_string(token);
			break;
		}

		// Escape the next character
		if (escaped == TRUE) {
			if (!vmc_lexer_test_escapeable(c)) {
				vmc_lexer_message_unknown_escaped_char(token, c);
			}
			escaped = FALSE;
		} else {
			// String exit
			if (c == '"') {
				break;
			}
			else if (c == '\n') {
				vmc_lexer_message_unclosed_string(token);
				break;
			}
			else if (c == '\\') {
				escaped = TRUE;
				token->modifier |= VMC_LEXER_TOKEN_MODIFIER_ESCAPED;
			}
		}

		c = *++token->source;
	}

	token->type = VMC_LEXER_TYPE_STRING;
	token->string.start = start;
	token->string.end = token->source;

	// Ignore the character
	if (c != 0)
		token->source++;
}

void _vmc_lexer_multi_line_string(vmc_lexer_token* token)
{
	BOOL escaped = FALSE;
	const char* start = ++token->source;
	char c = *token->source;

	while (1) {
		if (c == 0) {
			vmc_lexer_message_unclosed_string(token);
			break;
		}

		// Escape the next character
		if (escaped == TRUE) {
			if (!vmc_lexer_test_escapeable(c)) {
				vmc_lexer_message_unknown_escaped_char(token, c);
			}
			escaped = FALSE;
		}
		else {
			// String exit
			if (c == '`') {
				break;
			}
			else if (c == '\\') {
				escaped = TRUE;
				token->modifier |= VMC_LEXER_TOKEN_MODIFIER_ESCAPED;
			}
		}

		c = *++token->source;
	}

	token->type = VMC_LEXER_TYPE_STRING;
	token->modifier |= VMC_LEXER_TOKEN_MODIFIER_MULTILINE;
	token->string.start = start;
	token->string.end = token->source;

	// Ignore the character if not at the end
	if (c != 0)
		token->source++;
}

void _vmc_lexer_number(vmc_lexer_token* token)
{
	// Remember the first character and step to next char
	const char* start = token->source;
	token->source++;

	// Ignore all numbers
	while (vmc_lexer_test_number(*token->source)) token->source++;

	// Is this is a decimal?
	token->type = VMC_LEXER_TYPE_INT;
	if (*token->source == '.') {
		token->type = VMC_LEXER_TYPE_DECIMAL;
		token->source++;

		// Ignore all numbers
		while (vmc_lexer_test_number(*token->source)) token->source++;
	}

	// Is this a hex-decimal value?
	if (*token->source == 'x') {
		token->type = VMC_LEXER_TYPE_HEX;
		token->source++;
		// Ignore all hex values
		while (vml_test_hex(*token->source)) token->source++;
	}

	if (token->type == VMC_LEXER_TYPE_DECIMAL) {
		// This might be a value with format -3.402823466e+38f
		if (*token->source == 'e') {
			char peek = *(token->source + 1);
			if (peek == '+' || peek == '-') {
				token->source += 2;

				// Ignore all numbers
				while (vmc_lexer_test_number(*token->source)) token->source++;
			}
		}
		// Allow the leading "f". Used to differentiate between floats and doubles
		if (*token->source == 'f')
			token->source++;
	}

	token->string.start = start;
	token->string.end = token->source;
}

void _vmc_lexer_next(vmc_lexer_token* token)
{
	char ch = *token->source;

	// Reset modifier property
	token->modifier = 0;

	// Is this a potential keyword?
	if (vmc_lexer_test_keyword(ch)) {
		_vmc_lexer_keyword(token);
		return;
	}

	// Number?
	if (vmc_lexer_test_number(ch)) {
		_vmc_lexer_number(token);
		return;
	}

	switch (ch)
	{
	case '\n':
		_vmc_lexer_atom(VMC_LEXER_TYPE_NEWLINE, token);
		token->line++;
		token->line_offset = token->source;
		return;
	case '+':
		_vmc_lexer_atom(VMC_LEXER_TYPE_PLUS, token);
		return;
	case '-':
		_vmc_lexer_atom(VMC_LEXER_TYPE_MINUS, token);
		return;
	case '*':
		_vmc_lexer_atom(VMC_LEXER_TYPE_MULT, token);
		return;
	case '/':
		_vmc_lexer_div_or_comment(token);
		return;
	case '(':
		_vmc_lexer_atom(VMC_LEXER_TYPE_PARAN_L, token);
		return;
	case ')':
		_vmc_lexer_atom(VMC_LEXER_TYPE_PARAN_R, token);
		return;
	case '[':
		_vmc_lexer_atom(VMC_LEXER_TYPE_SQUARE_L, token);
		return;
	case ']':
		_vmc_lexer_atom(VMC_LEXER_TYPE_SQUARE_R, token);
		return;
	case '{':
		_vmc_lexer_atom(VMC_LEXER_TYPE_BRACKET_L, token);
		return;
	case '}':
		_vmc_lexer_atom(VMC_LEXER_TYPE_BRACKET_R, token);
		return;
	case '.':
		_vmc_lexer_atom(VMC_LEXER_TYPE_DOT, token);
		return;
	case ',':
		_vmc_lexer_atom(VMC_LEXER_TYPE_COMMA, token);
		return;
	case '&':
		_vmc_lexer_bitand_or_and(token);
		return;
	case '"':
		_vmc_lexer_single_line_string(token);
		return;
	case '#':
		_vmc_lexer_atom(VMC_LEXER_TYPE_HASH, token);
		return;
	case '`':
		_vmc_lexer_multi_line_string(token);
		return;
	case 0:
		_vmc_lexer_eof(token);
		return;
	default:
		_vmc_lexer_unknown(token);
		return;
	}
}

//
/////////////////////////////////////////////////////////////////////////////
// 

void vmc_lexer_init(vmc_lexer* l, const vm_byte* source)
{
	l->source_start = source;
	vm_messages_init(&l->messages);
}

void vmc_lexer_token_init(vmc_lexer* l, vmc_lexer_token* t)
{
	t->type = VMC_LEXER_TYPE_UNKNOWN;
	t->modifier = 0;
	vm_string_zero(&t->string);
	t->source = t->source_start = l->source_start;
	t->line = 0;
	t->line_offset = NULL;
	t->messages = &l->messages;
}

vmc_lexer* vmc_lexer_parse(const vm_byte* source)
{
	vmc_lexer* l = (vmc_lexer*)vmc_malloc(sizeof(vmc_lexer));
	if (l == NULL)
		return NULL;
	vmc_lexer_init(l, source);
	return l;
}

void vmc_lexer_release(vmc_lexer* l)
{
	vm_messages_destroy(&l->messages);
}

void vmc_lexer_destroy(vmc_lexer* l)
{
	vmc_lexer_release(l);
	vmc_free(l);
}

void vmc_lexer_next(vmc_lexer_token* t)
{
	char ch = *t->source;
	if (ch == 0) {
		_vmc_lexer_eof(t);
		return;
	}
	while (vmc_lexer_test_whitespace(ch)) {
		if (ch == '\n') {
			t->line++;
			t->line_offset = t->source;
		}
		ch = *++t->source;
	}
	_vmc_lexer_next(t);
}

void vmc_lexer_next_newline(vmc_lexer_token* token)
{
	char ch = *token->source;
	if (ch == 0) {
		_vmc_lexer_eof(token);
		return;
	}
	while (vmc_lexer_test_whitespace_ignore_nl(ch)) ch = *++token->source;
	_vmc_lexer_next(token);
}

BOOL vmc_lexer_next_type(vmc_lexer_token* token, vmc_lexer_token_type type)
{
	vmc_lexer_next(token);
	return token->type == type;
}

char vmc_lexer_peek(vmc_lexer* l, vmc_lexer_token* t)
{
	return *(t->source + 1);
}

vm_int32 vmc_lexer_token_toint32(vmc_lexer_token* t)
{
	switch (t->type)
	{
	case VMC_LEXER_TYPE_HEX:
		return (vm_int32)hextoi64(t->string.start, vm_string_length(&t->string));
	case VMC_LEXER_TYPE_INT:
		return (vm_int32)strtoi64(t->string.start, vm_string_length(&t->string));
	default:
		return 0;
	}
}
