#include "vmc_lexer.h"
#include "vmc_lexer_logic.h"
#include "vmc_error.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

BOOL _vmc_lexer_add_message(vmc_lexer* l, int error_code, const char* format, ...)
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

	if (l->messages_last == NULL)
		l->messages_last = l->messages_first = m;
	else
		l->messages_last->next = m;
	return FALSE;
}

BOOL _vmc_add_warn_escape_rune_unknown(vmc_lexer* l, vm_byte rune)
{
	int line, line_offset, offset;
	vmc_lexer_get_metadata(l, &line, &line_offset, &offset);
	return _vmc_lexer_add_message(l, VMC_WARN_CODE_ESCAPE_RUNE_UNKNOWN, "escape rune is unknown: '%c' at %d:%d",
		rune, line, line_offset);
}

BOOL _vmc_add_warn_unclosed_string(vmc_lexer* l)
{
	int line, line_offset, offset;
	vmc_lexer_get_metadata(l, &line, &line_offset, &offset);
	return _vmc_lexer_add_message(l, VMC_WARN_CODE_UNCLOSED_STRING, "unclosed string at %d:%d",
		line, line_offset);
}

BOOL _vmc_add_warn_unclosed_comment(vmc_lexer* l)
{
	int line, line_offset, offset;
	vmc_lexer_get_metadata(l, &line, &line_offset, &offset);
	return _vmc_lexer_add_message(l, VMC_WARN_CODE_UNCLOSED_COMMENT, "unclosed comment at %d:%d",
		line, line_offset);
}

void _vmc_lexer_atom(vmc_lexer* l, vmc_lexer_token_type type, vmc_lexer_token* token)
{
	token->type = type;
	token->modifier = 0;
	token->string.start = l->source;
	token->string.end = ++l->source;
}

void _vmc_lexer_unknown(vmc_lexer* l, vmc_lexer_token* token)
{
	token->string.start = l->source;
	token->string.end = l->source;
	token->type = VMC_LEXER_TYPE_UNKNOWN;
}

void _vmc_lexer_eof(vmc_lexer* l, vmc_lexer_token* token)
{
	token->string.start = l->source;
	token->string.end = l->source;
	token->type = VMC_LEXER_TYPE_EOF;
}

void _vmc_lexer_comment(vmc_lexer* lexer, vmc_lexer_token* token)
{
	const char* comment_start = lexer->source;
	char c = *lexer->source;

	// Seek until new-line or eof
	while (c != 0 && c != '\n')
		c = *++lexer->source;

	token->string.start = comment_start;
	token->string.end = lexer->source;
	token->type = VMC_LEXER_TYPE_COMMENT;
}

void _vmc_lexer_single_line_comment(vmc_lexer* l, vmc_lexer_token* t)
{
	const char* comment_start = ++l->source;
	char c = *comment_start;

	// Seek until new-line or eof
	while (c != 0 && c != '\n') {
		c = *(++l->source);
	}

	t->type = VMC_LEXER_TYPE_COMMENT;
	t->string.start = comment_start;
	t->string.end = l->source;
}

void _vmc_lexer_multi_line_comment(vmc_lexer* l, vmc_lexer_token* t)
{
	const char* comment_start = ++l->source;
	const char* commend_end = comment_start;
	char c = *comment_start;

	// Seek until */ or eof
	while (1) {
		if (c == 0) {
			commend_end = l->source;
			_vmc_add_warn_unclosed_comment(l);
			break;
		}
		// If we've reached */
		if (c == '*' && *(l->source + 1) == '/') {
			commend_end = l->source;
			l->source += 2;
			break;
		}
		c = *(++l->source);
	}

	t->type = VMC_LEXER_TYPE_COMMENT;
	t->modifier |= VMC_LEXER_TOKEN_MODIFIER_MULTILINE;
	t->string.start = comment_start;
	t->string.end = commend_end;
}

void _vmc_lexer_div_or_comment(vmc_lexer* l, vmc_lexer_token* t)
{
	const char* start = l->source++;
	char c = *l->source;

	switch (c)
	{
	case '/':
		_vmc_lexer_single_line_comment(l, t);
		return;
	case '*':
		_vmc_lexer_multi_line_comment(l, t);
		return;
	default:
		t->type = VMC_LEXER_TYPE_DIV;
		t->modifier = 0;
		t->string.start = start;
		t->string.end = l->source;
		return;
	}
}

void _vmc_lexer_bitand_or_and(vmc_lexer* l, vmc_lexer_token* t)
{
	const char* start = l->source++;
	char c = *l->source;

	switch (c)
	{
	case '&':
		l->source++;
		t->type = VMC_LEXER_TYPE_TEST_AND;
		t->modifier = 0;
		t->string.start = start;
		t->string.end = l->source;
		return;
	default:
		t->type = VMC_LEXER_TYPE_BIT_AND;
		t->modifier = 0;
		t->string.start = start;
		t->string.end = l->source;
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

void _vmc_lexer_keyword(vmc_lexer* l, vmc_lexer_token* token)
{
	// Remember the first character and step to next char
	const char* start = l->source;
	l->source++;

	// Ignore all characters
	while (vmc_lexer_test_char(*l->source)) l->source++;

	token->string.start = start;
	token->string.end = l->source;
	token->type = _vmc_lexer_find_keyword_type(&token->string);
}

void _vmc_lexer_single_line_string(vmc_lexer* l, vmc_lexer_token* token)
{
	BOOL escaped = FALSE;
	const char* start = ++l->source;
	char c = *l->source;

	while (1) {
		if (c == 0) {
			_vmc_add_warn_unclosed_string(l);
			break;
		}

		// Escape the next character
		if (escaped == TRUE) {
			if (!vmc_lexer_test_escapeable(c)) {
				_vmc_add_warn_escape_rune_unknown(l, c);
			}
			escaped = FALSE;
		} else {
			// String exit
			if (c == '"') {
				break;
			}
			else if (c == '\n') {
				_vmc_add_warn_unclosed_string(l);
				break;
			}
			else if (c == '\\') {
				escaped = TRUE;
				token->modifier |= VMC_LEXER_TOKEN_MODIFIER_ESCAPED;
			}
		}

		c = *++l->source;
	}

	token->type = VMC_LEXER_TYPE_STRING;
	token->string.start = start;
	token->string.end = l->source;

	// Ignore the character
	if (c != 0)
		l->source++;
}

void _vmc_lexer_multi_line_string(vmc_lexer* l, vmc_lexer_token* token)
{
	BOOL escaped = FALSE;
	const char* start = ++l->source;
	char c = *l->source;

	while (1) {
		if (c == 0) {
			_vmc_add_warn_unclosed_string(l);
			break;
		}

		// Escape the next character
		if (escaped == TRUE) {
			if (!vmc_lexer_test_escapeable(c)) {
				_vmc_add_warn_escape_rune_unknown(l, c);
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

		c = *++l->source;
	}

	token->type = VMC_LEXER_TYPE_STRING;
	token->modifier |= VMC_LEXER_TOKEN_MODIFIER_MULTILINE;
	token->string.start = start;
	token->string.end = l->source;

	// Ignore the character if not at the end
	if (c != 0)
		l->source++;
}

void _vmc_lexer_number(vmc_lexer* l, vmc_lexer_token* token)
{
	// Remember the first character and step to next char
	const char* start = l->source;
	l->source++;

	// Ignore all numbers
	while (vmc_lexer_test_number(*l->source)) l->source++;

	// Is this is a decimal?
	token->type = VMC_LEXER_TYPE_INT;
	if (*l->source == '.') {
		token->type = VMC_LEXER_TYPE_DECIMAL;
		l->source++;

		// Ignore all numbers
		while (vmc_lexer_test_number(*l->source)) l->source++;
	}

	// Is this a hex-decimal value?
	if (*l->source == 'x') {
		token->type = VMC_LEXER_TYPE_HEX;
		l->source++;
		// Ignore all hex values
		while (vml_test_hex(*l->source)) l->source++;
	}

	if (token->type == VMC_LEXER_TYPE_DECIMAL) {
		// This might be a value with format -3.402823466e+38f
		if (*l->source == 'e') {
			char peek = *(l->source + 1);
			if (peek == '+' || peek == '-') {
				l->source += 2;

				// Ignore all numbers
				while (vmc_lexer_test_number(*l->source)) l->source++;
			}
		}
		// Allow the leading "f". Used to differentiate between floats and doubles
		if (*l->source == 'f')
			l->source++;
	}

	token->string.start = start;
	token->string.end = l->source;
}

void _vmc_lexer_next(vmc_lexer* l, vmc_lexer_token* token)
{
	char ch = *l->source;

	// Reset modifier property
	token->modifier = 0;

	// Is this a potential keyword?
	if (vmc_lexer_test_keyword(ch)) {
		_vmc_lexer_keyword(l, token);
		return;
	}

	// Number?
	if (vmc_lexer_test_number(ch)) {
		_vmc_lexer_number(l, token);
		return;
	}

	switch (ch)
	{
	case '\n':
		_vmc_lexer_atom(l, VMC_LEXER_TYPE_NEWLINE, token);
		l->line++;
		l->line_offset = l->source;
		return;
	case '+':
		_vmc_lexer_atom(l, VMC_LEXER_TYPE_PLUS, token);
		return;
	case '-':
		_vmc_lexer_atom(l, VMC_LEXER_TYPE_MINUS, token);
		return;
	case '*':
		_vmc_lexer_atom(l, VMC_LEXER_TYPE_MULT, token);
		return;
	case '/':
		_vmc_lexer_div_or_comment(l, token);
		return;
	case '(':
		_vmc_lexer_atom(l, VMC_LEXER_TYPE_PARAN_L, token);
		return;
	case ')':
		_vmc_lexer_atom(l, VMC_LEXER_TYPE_PARAN_R, token);
		return;
	case '[':
		_vmc_lexer_atom(l, VMC_LEXER_TYPE_SQUARE_L, token);
		return;
	case ']':
		_vmc_lexer_atom(l, VMC_LEXER_TYPE_SQUARE_R, token);
		return;
	case '{':
		_vmc_lexer_atom(l, VMC_LEXER_TYPE_BRACKET_L, token);
		return;
	case '}':
		_vmc_lexer_atom(l, VMC_LEXER_TYPE_BRACKET_R, token);
		return;
	case '.':
		_vmc_lexer_atom(l, VMC_LEXER_TYPE_DOT, token);
		return;
	case ',':
		_vmc_lexer_atom(l, VMC_LEXER_TYPE_COMMA, token);
		return;
	case '&':
		_vmc_lexer_bitand_or_and(l, token);
		return;
	case '"':
		_vmc_lexer_single_line_string(l, token);
		return;
	case '`':
		_vmc_lexer_multi_line_string(l, token);
		return;
	case 0:
		_vmc_lexer_eof(l, token);
		return;
	default:
		_vmc_lexer_unknown(l, token);
		return;
	}
}

//
/////////////////////////////////////////////////////////////////////////////
// 

void vmc_lexer_init(vmc_lexer* l, const vm_byte* source)
{
	l->source_start = l->source = source;
	l->line = 0;
	l->line_offset = NULL;
	l->messages_first = l->messages_last = NULL;
}

vmc_lexer* vmc_lexer_parse(const vm_byte* source)
{
	vmc_lexer* l = (vmc_lexer*)malloc(sizeof(vmc_lexer));
	if (l == NULL)
		return NULL;
	vmc_lexer_init(l, source);
	return l;
}

void vmc_lexer_release(vmc_lexer* l)
{
	vm_message* m;

	// Cleanup messages
	m = l->messages_first;
	while (m != NULL) {
		vm_message* next = m->next;
		free(m);
		m = next;
	}
	l->messages_first = NULL;
}

void vmc_lexer_destroy(vmc_lexer* l)
{
	vmc_lexer_release(l);
	free(l);
}

void vmc_lexer_next(vmc_lexer* l, vmc_lexer_token* token)
{
	char ch = *l->source;
	if (ch == 0)
		return;
	while (vmc_lexer_test_whitespace(ch)) ch = *++l->source;
	_vmc_lexer_next(l, token);
}

void vmc_lexer_next_newline(vmc_lexer* l, vmc_lexer_token* token)
{
	char ch = *l->source;
	if (ch == 0)
		return;
	while (vmc_lexer_test_whitespace_ignore_nl(ch)) ch = *++l->source;
	_vmc_lexer_next(l, token);
}

BOOL vmc_lexer_next_type(vmc_lexer* l, vmc_lexer_token_type type, vmc_lexer_token* token)
{
	vmc_lexer_next(l, token);
	return token->type == type;
}

char vmc_lexer_peek(vmc_lexer* l)
{
	return *(l->source + 1);
}

BOOL vmc_lexer_next_ignore_ws_until(vmc_lexer* l, vmc_lexer_token_type type, vmc_lexer_token* token)
{
	vmc_lexer_next(l, token);
	while (token->type == VMC_LEXER_TYPE_NEWLINE) vmc_lexer_next(l, token);
	return token->type == type;
}