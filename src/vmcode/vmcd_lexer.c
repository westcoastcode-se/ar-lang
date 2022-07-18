#include "vmcd_lexer.h"
#include <float.h>

void vmcd_lexer_init(vmcd_lexer* l, const vm_byte* source_code)
{
	l->source_start = source_code;
}

void vmcd_lexer_release(vmcd_lexer* l)
{
	l->source_start = NULL;
}

void vmcd_token_init(vmcd_token* t, vmcd_lexer* l)
{
	t->source = t->source_start = l->source_start;
	t->type = VMCD_TOKEN_UNKNOWN;
	t->modifier = 0;
	vm_string_zero(&t->string);
	t->line = 0;
	t->line_offset = NULL;
	vm_messages_init(&t->messages);
	
	// Prepare the first token
	vmcd_token_next(t);
}

void vmcd_token_release(vmcd_token* t)
{
	vm_messages_destroy(&t->messages);
}

void vmcd_token_atom(vmcd_token_type type, vmcd_token* token)
{
	token->type = type;
	token->modifier = 0;
	token->string.start = token->source;
	token->string.end = ++token->source;
}

void vmcd_token_unknown(vmcd_token* token)
{
	token->string.start = token->source;
	token->string.end = token->source;
	token->type = VMCD_TOKEN_UNKNOWN;
}

void vmcd_token_eof(vmcd_token* t)
{
	t->string.start = t->source;
	t->string.end = t->source;
	t->type = VMCD_TOKEN_EOF;
}

void vmcd_assign_or_equals(vmcd_token* t)
{
	const char* start = t->source++;
	char c = *t->source;

	switch (c)
	{
	case '=':
		t->source++;
		t->type = VMCD_TOKEN_TEST_EQUALS;
		t->modifier = 0;
		t->string.start = start;
		t->string.end = t->source;
		return;
	default:
		t->type = VMCD_TOKEN_ASSIGN;
		t->modifier = 0;
		t->string.start = start;
		t->string.end = t->source;
		return;
	}
}

void vmcd_colon_or_declassign(vmcd_token* t)
{
	const char* start = t->source++;
	char c = *t->source;

	switch (c)
	{
	case '=':
		t->source++;
		t->type = VMCD_TOKEN_DECL_ASSIGN;
		t->modifier = 0;
		t->string.start = start;
		t->string.end = t->source;
		return;
	default:
		t->type = VMCD_TOKEN_COLON;
		t->modifier = 0;
		t->string.start = start;
		t->string.end = t->source;
		return;
	}
}

void vmcd_token_single_line_comment(vmcd_token* t)
{
	const char* comment_start = ++t->source;
	char c = *comment_start;

	// Seek until new-line or eof
	while (c != 0 && c != '\n') {
		c = *(++t->source);
	}

	t->type = VMCD_TOKEN_COMMENT;
	t->string.start = comment_start;
	t->string.end = t->source;
}

void vmcd_token_multi_line_comment(vmcd_token* t)
{
	const char* comment_start = ++t->source;
	const char* commend_end = comment_start;
	char c = *comment_start;

	// Seek until */ or eof
	while (1) {
		if (c == 0) {
			commend_end = t->source;
			//vmcd_token_message_unclosed_comment(t);
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

	t->type = VMCD_TOKEN_COMMENT;
	t->string.start = comment_start;
	t->string.end = commend_end;
}

void vmcd_token_div_or_comment(vmcd_token* t)
{
	const char* start = t->source++;
	char c = *t->source;

	switch (c)
	{
	case '/':
		vmcd_token_single_line_comment(t);
		return;
	case '*':
		vmcd_token_multi_line_comment(t);
		return;
	default:
		t->type = VMCD_TOKEN_OP_DIV;
		t->modifier = 0;
		t->string.start = start;
		t->string.end = t->source;
		return;
	}
}

vmcd_token_type vmcd_token_find_keyword_type(const vm_string* str)
{
	const int len = vm_string_length(str);
	if (len == 2) {
		if (vm_str_cmp(str->start, len, "if", 2)) {
			return VMCD_TOKEN_KEYWORD_IF;
		}
	}
	else if (len == 3) {
		if (vm_str_cmp(str->start, len, "var", 3)) {
			return VMCD_TOKEN_KEYWORD_VAR;
		}
		else if (vm_str_cmp(str->start, len, "for", 3)) {
			return VMCD_TOKEN_KEYWORD_FOR;
		}
	}
	else if (len == 4) {
		if (vm_str_cmp(str->start, len, "func", 4)) {
			return VMCD_TOKEN_KEYWORD_FUNC;
		} else if (vm_str_cmp(str->start, len, "type", 4)) {
			return VMCD_TOKEN_KEYWORD_TYPE;
		} else if (vm_str_cmp(str->start, len, "else", 4)) {
			return VMCD_TOKEN_KEYWORD_ELSE;
		}
	}
	else if (len == 5) {
		if (vm_str_cmp(str->start, len, "const", 5)) {
			return VMCD_TOKEN_KEYWORD_CONST;
		}
	}
	else if (len == 6) {
		if (vm_str_cmp(str->start, len, "import", 6)) {
			return VMCD_TOKEN_KEYWORD_IMPORT;
		}
		else if (vm_str_cmp(str->start, len, "struct", 6)) {
			return VMCD_TOKEN_KEYWORD_STRUCT;
		}
		else if (vm_str_cmp(str->start, len, "return", 6)) {
			return VMCD_TOKEN_KEYWORD_RETURN;
		}
		else if (vm_str_cmp(str->start, len, "extern", 6)) {
			return VMCD_TOKEN_KEYWORD_EXTERN;
		}
	}
	else if (len == 7) {
		if (vm_str_cmp(str->start, len, "package", 7)) {
			return VMCD_TOKEN_KEYWORD_PACKAGE;
		}
	}
	else if (len == 9) {
		if (vm_str_cmp(str->start, len, "interface", 9)) {
			return VMCD_TOKEN_KEYWORD_INTERFACE;
		}
	}
	return VMCD_TOKEN_KEYWORD;
}

void vmcd_token_number(vmcd_token* token)
{
	// Remember the first character and step to next char
	const char* start = token->source;
	token->source++;

	// Ignore all numbers
	while (vmcd_token_test_number(*token->source)) token->source++;

	// Is this is a decimal?
	token->type = VMCD_TOKEN_VALUE_INT;
	if (*token->source == '.') {
		token->type = VMCD_TOKEN_VALUE_DECIMAL;
		token->source++;

		// Ignore all numbers
		while (vmcd_token_test_number(*token->source)) token->source++;
	}

	// Is this a hex-decimal value?
	if (*token->source == 'x') {
		token->type = VMCD_TOKEN_VALUE_HEX;
		token->source++;
		// Ignore all hex values
		while (vmcd_token_test_hex(*token->source)) token->source++;
	}

	if (token->type == VMCD_TOKEN_VALUE_DECIMAL) {
		// This might be a value with format -3.402823466e+38f
		if (*token->source == 'e') {
			char peek = *(token->source + 1);
			if (peek == '+' || peek == '-') {
				token->source += 2;

				// Ignore all numbers
				while (vmcd_token_test_number(*token->source)) token->source++;
			}
		}
	}

	token->string.start = start;
	token->string.end = token->source;
}


void vmcd_token_keyword(vmcd_token* t)
{
	// Remember the first character and step to next char
	const char* start = t->source;
	t->source++;

	// Ignore all characters
	while (vmc_token_test_char(*t->source)) t->source++;

	t->string.start = start;
	t->string.end = t->source;
	t->type = vmcd_token_find_keyword_type(&t->string);
}

void vmcd_token_next0(vmcd_token* t)
{
	char ch = *t->source;
	t->modifier = 0;

	// Is this a potential keyword?
	if (vmcd_token_test_keyword(ch)) {
		vmcd_token_keyword(t);
		return;
	}

	// Number?
	if (vmcd_token_test_number(ch)) {
		vmcd_token_number(t);
		return;
	}

	switch (ch)
	{
	case '\n':
		vmcd_token_atom(VMCD_TOKEN_NEWLINE, t);
		t->line++;
		t->line_offset = t->source;
		return;
	case '+':
		vmcd_token_atom(VMCD_TOKEN_OP_PLUS, t);
		return;
	case '-':
		vmcd_token_atom(VMCD_TOKEN_OP_MINUS, t);
		return;
	case '*':
		vmcd_token_atom(VMCD_TOKEN_OP_MULT, t);
		return;
	case '/':
		vmcd_token_div_or_comment(t);
		return;
	case '(':
		vmcd_token_atom(VMCD_TOKEN_PARAN_L, t);
		return;
	case ')':
		vmcd_token_atom(VMCD_TOKEN_PARAN_R, t);
		return;
	case '[':
		vmcd_token_atom(VMCD_TOKEN_SQUARE_L, t);
		return;
	case ']':
		vmcd_token_atom(VMCD_TOKEN_SQUARE_R, t);
		return;
	case '{':
		vmcd_token_atom(VMCD_TOKEN_BRACKET_L, t);
		return;
	case '}':
		vmcd_token_atom(VMCD_TOKEN_BRACKET_R, t);
		return;
	case '.':
		vmcd_token_atom(VMCD_TOKEN_DOT, t);
		return;
	case ',':
		vmcd_token_atom(VMCD_TOKEN_COMMA, t);
		return;
	case '=':
		vmcd_assign_or_equals(t);
		return;
	case ':':
		vmcd_colon_or_declassign(t);
		return;
	//case '&':
	//	_vmc_lexer_bitand_or_and(token);
	//	return;
	//case '"':
	//	_vmc_lexer_single_line_string(token);
	//	return;
	//case '#':
	//	vmcd_token_atom(VMCD_TOKEN_HASH, token);
	//	return;
	//case '`':
	//	_vmc_lexer_multi_line_string(token);
	//	return;
	case 0:
		vmcd_token_eof(t);
		return;
	default:
		vmcd_token_unknown(t);
		return;
	}
}

void vmcd_token_next(vmcd_token* t)
{
	char ch = *t->source;
	if (ch == 0) {
		vmcd_token_eof(t);
		return;
	}
	while (vmcd_token_test_whitespace_ignore_nl(ch)) {
		if (ch == '\n') {
			t->line++;
			t->line_offset = t->source;
		}
		ch = *++t->source;
	}
	vmcd_token_next0(t);
}

void vmcd_token_skip_newline(vmcd_token* t)
{
	while (t->type == VMCD_TOKEN_NEWLINE) {
		vmcd_token_next(t);
	}
}

BOOL vmcd_token_next_type(vmcd_token* t, vmcd_token_type type)
{
	vmcd_token_next(t);
	return t->type == type;
}

vm_int32 vmcd_token_i4(vmcd_token* t)
{
	return (vm_int32)strtoi64(t->string.start, vm_string_length(&t->string));
}

vm_uint32 vmcd_token_ui4(vmcd_token* t)
{
	return (vm_uint32)strtou64(t->string.start, vm_string_length(&t->string));
}

vm_int64 vmcd_token_i8(vmcd_token* t)
{
	return strtoi64(t->string.start, vm_string_length(&t->string));
}

vm_uint64 vmcd_token_ui8(vmcd_token* t)
{
	return strtou64(t->string.start, vm_string_length(&t->string));
}

vm_float32 vmcd_token_f4(vmcd_token* t)
{
	char temp[64];
	temp[sprintf(temp, "%.*s", vm_string_length(&t->string), t->string.start)] = 0;

	const float v = strtof(temp, NULL);
	if (v < (FLT_MAX - FLT_EPSILON) && v >(FLT_MIN + FLT_EPSILON))
		return v;
	else
		return 0.0f;
}

vm_float64 vmcd_token_f8(vmcd_token* t)
{
	char temp[64];
	temp[sprintf(temp, "%.*s", vm_string_length(&t->string), t->string.start)] = 0;

	const double v = strtod(temp, NULL);
	if (v < (DBL_MAX - DBL_EPSILON) && v >(DBL_MIN + DBL_EPSILON))
		return v;
	else
		return 0.0;
}

BOOL vmcd_token_bool(vmcd_token* t)
{
	return vm_str_cmp(t->string.start, vm_string_length(&t->string), "true", 4);
}
