#include "vm_lexer.h"
#include "vm_lexer_logic.h"

void _vml_lexer_atom(vmc_lexer* l, vml_type type, vml_token* token)
{
	token->string.start = l->source;
	token->string.end = ++l->source;
	token->type = type;
}

void _vml_lexer_unknown(vmc_lexer* l, vml_token* token)
{
	token->string.start = l->source;
	token->string.end = l->source;
	token->type = VML_TYPE_UNKNOWN;
}

void _vml_lexer_eof(vmc_lexer* l, vml_token* token)
{
	token->string.start = l->source;
	token->string.end = l->source;
	token->type = VML_TYPE_EOF;
}

void _vml_lexer_comment(vmc_lexer* lexer, vml_token* token)
{
	const char* comment_start = lexer->source;
	char c = *lexer->source;

	// Ignore until new-line or eof
	while (c != 0 && c != '\n')
		c = *++lexer->source;

	token->string.start = comment_start;
	token->string.end = lexer->source;
	token->type = VML_TYPE_COMMENT;
}

void _vml_lexer_ins(vmc_lexer* l, vml_token* token)
{
	// Remember the first character and step to next char
	const char* start = l->source;
	l->source++;

	// Ignore all characters
	while (vml_test_char(*l->source)) l->source++;

	token->type = VML_TYPE_INS;
	token->string.start = start;
	token->string.end = l->source;
}

void _vml_lexer_argindex(vmc_lexer* l, vml_token* token)
{
	// Remember the first character and step to next char
	const char* start = l->source;
	l->source++;

	// Ignore all numbers
	while (vml_test_number(*l->source)) l->source++;

	token->type = VML_TYPE_ARGIDX;
	token->string.start = start;
	token->string.end = l->source;
}

void _vml_lexer_single_line_string(vmc_lexer* l, vml_token* token)
{
	BOOL escaped = FALSE;
	const char* start = ++l->source;
	char c = *l->source;

	// Ignore until new-line, eof or "
	while (c != 0 && c != '\n' && c != '"')
		c = *++l->source;

	token->string.start = start;
	token->string.end = l->source;
	token->type = VML_TYPE_STRING;

	// Ignore the last '"'
	if (*l->source == '"')
		l->source++;
}

void _vml_lexer_number(vmc_lexer* l, vml_token* token)
{
	// Remember the first character and step to next char
	const char* start = l->source;
	l->source++;

	// Ignore all numbers
	while (vml_test_number(*l->source)) l->source++;

	// Is this is a decimal?
	token->type = VML_TYPE_INT;
	if (*l->source == '.') {
		token->type = VML_TYPE_DECIMAL;
		l->source++;

		// Ignore all numbers
		while (vml_test_number(*l->source)) l->source++;
	}

	// Is this a hex-decimal value?
	if (*l->source == 'x') {
		token->type = VML_TYPE_HEX;
		l->source++;
		// Ignore all hex values
		while (vml_test_hex(*l->source)) l->source++;
	}

	// This might be a value with format -3.402823466e+38f
	if (*l->source == 'e') {
		char peek = *(l->source + 1);
		if (peek == '+' || peek == '-') {
			l->source += 2;

			// Ignore all numbers
			while (vml_test_number(*l->source)) l->source++;
		}
	}

	// Allow the leading "f". Used to differentiate between floats and doubles
	if (*l->source == 'f')
		l->source++;

	token->string.start = start;
	token->string.end = l->source;
}

//
/////////////////////////////////////////////////////////////////////////////
// 

vmc_lexer* vmc_lexer_parse(const vm_byte* source)
{
	vmc_lexer* l = (vmc_lexer*)malloc(sizeof(vmc_lexer));
	if (l == NULL)
		return NULL;
	l->source_start = l->source = source;
	l->line = l->line_offset = 0;
	return l;
}

void vmc_lexer_destroy(vmc_lexer* l)
{
	free(l);
}

void vmc_lexer_next(vmc_lexer* l, vml_token* token)
{
	char ch = *l->source;

	// Ignore all spaces
	while (vml_test_space(ch)) ch = *++l->source;

	// Is this a new instruction?
	if (vml_test_instruction_start(ch)) {
		_vml_lexer_ins(l, token);
		return;
	}

	// Number?
	if (vml_test_number(ch)) {
		_vml_lexer_number(l, token);
		return;
	}

	switch (ch)
	{
	case '\n':
		_vml_lexer_atom(l, VML_TYPE_NEWLINE, token);
		l->line++;
		l->line_offset = l->source;
		return;
	case '#':
		_vml_lexer_comment(l, token);
		return;
	case ',':
		_vml_lexer_atom(l, VML_TYPE_COMMA, token);
		return;
	case '-':
		_vml_lexer_atom(l, VML_TYPE_MINUS, token);
		return;
	case '+':
		_vml_lexer_atom(l, VML_TYPE_PLUS, token);
		return;
	case '[':
		_vml_lexer_atom(l, VML_TYPE_LSQUARE, token);
		return;
	case ']':
		_vml_lexer_atom(l, VML_TYPE_RSQUARE, token);
		return;
	case '$':
		_vml_lexer_argindex(l, token);
		return;
	case '"':
		_vml_lexer_single_line_string(l, token);
		return;
	case 0:
		_vml_lexer_eof(l, token);
		return;
	default:
		_vml_lexer_unknown(l, token);
		return;
	}
}

BOOL vmc_lexer_next_type(vmc_lexer* l, vml_type type, vml_token* token)
{
	vmc_lexer_next(l, token);
	return token->type == type;
}
