#include "lexer.h"
#include <float.h>

// Is the supplied character a valid "first" keyword character
inline BOOL zpp_token_test_keyword(vm_byte c)
{
	switch (c) {
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'g':
	case 'h':
	case 'i':
	case 'j':
	case 'k':
	case 'l':
	case 'm':
	case 'n':
	case 'o':
	case 'p':
	case 'q':
	case 'r':
	case 's':
	case 't':
	case 'u':
	case 'v':
	case 'w':
	case 'x':
	case 'y':
	case 'z':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
	case '_':
		return TRUE;
	default:
		return FALSE;
	}
}

inline BOOL zpp_token_test_uppercase(vm_byte c)
{
	switch (c) {
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
	case 'G':
	case 'H':
	case 'I':
	case 'J':
	case 'K':
	case 'L':
	case 'M':
	case 'N':
	case 'O':
	case 'P':
	case 'Q':
	case 'R':
	case 'S':
	case 'T':
	case 'U':
	case 'V':
	case 'W':
	case 'X':
	case 'Y':
	case 'Z':
		return TRUE;
	default:
		return FALSE;
	}
}

inline BOOL zpp_token_test_number(vm_byte c)
{
	switch (c) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		return TRUE;
	default:
		return FALSE;
	}
}

inline BOOL zpp_token_test_hex(vm_byte c)
{
	switch (c) {
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
		return TRUE;
	default:
		return FALSE;
	}
}

inline BOOL zpp_token_test_char(vm_byte c)
{
	return zpp_token_test_keyword(c) == TRUE || zpp_token_test_number(c) == TRUE;
}

// Test if rune is a whitespace. Newlines are not considered white-space in this function
inline BOOL zpp_token_test_whitespace_ignore_nl(vm_byte c)
{
	switch (c) {
	case ' ':
	case '\t':
	case '\r':
		return TRUE;
	default:
		return FALSE;
	}
}

// Test if rune is a whitespace
inline BOOL zpp_loken_test_whitespace(vm_byte c)
{
	switch (c) {
	case ' ':
	case '\t':
	case '\r':
	case '\n':
		return TRUE;
	default:
		return FALSE;
	}
}

// Check if the supplied rune is a newline character
inline BOOL zpp_token_test_newline(vm_byte c)
{
	return c == '\n';
}

// Check to see if the supplied byte is escapable
inline BOOL zpp_token_test_escapeable(vm_byte c)
{
	switch (c)
	{
	case '\\':
	case 't':
	case 'n':
	case 'r':
	case '0':
	case '\'':
	case '"':
	case '`':
		return TRUE;
	default:
		return FALSE;
	}
}

void zpp_lexer_init(zpp_lexer* l, const vm_byte* source_code)
{
	l->source_start = source_code;
}

void zpp_token_init(zpp_token* t, zpp_lexer* l)
{
	t->source = t->source_start = l->source_start;
	t->type = ZPP_TOKEN_UNKNOWN;
	t->modifier = 0;
	vm_string_zero(&t->string);
	t->line = 0;
	t->line_offset = NULL;
	t->message.code = 0;

	// Prepare the first token
	zpp_token_next(t);
}


void zpp_token_atom(zpp_token_type type, zpp_token* token)
{
	token->type = type;
	token->modifier = 0;
	token->string.start = token->source;
	token->string.end = ++token->source;
}

void zpp_token_unknown(zpp_token* token)
{
	token->string.start = token->source;
	token->string.end = token->source;
	token->type = ZPP_TOKEN_UNKNOWN;
}

void zpp_token_eof(zpp_token* t)
{
	t->string.start = t->source;
	t->string.end = t->source;
	t->type = ZPP_TOKEN_EOF;
}

void zpp_assign_or_equals(zpp_token* t)
{
	const char* start = t->source++;
	char c = *t->source;

	switch (c)
	{
	case '=':
		t->source++;
		t->type = ZPP_TOKEN_TEST_EQUALS;
		t->modifier = 0;
		t->string.start = start;
		t->string.end = t->source;
		return;
	default:
		t->type = ZPP_TOKEN_ASSIGN;
		t->modifier = 0;
		t->string.start = start;
		t->string.end = t->source;
		return;
	}
}

void zpp_lt_or_lte(zpp_token* t)
{
	const char* start = t->source++;
	char c = *t->source;

	switch (c)
	{
	case '=':
		t->source++;
		t->type = ZPP_TOKEN_TEST_LTE;
		t->modifier = 0;
		t->string.start = start;
		t->string.end = t->source;
		return;
	default:
		t->type = ZPP_TOKEN_TEST_LT;
		t->modifier = 0;
		t->string.start = start;
		t->string.end = t->source;
		return;
	}
}

void zpp_gt_or_gte(zpp_token* t)
{
	const char* start = t->source++;
	char c = *t->source;

	switch (c)
	{
	case '=':
		t->source++;
		t->type = ZPP_TOKEN_TEST_GTE;
		t->modifier = 0;
		t->string.start = start;
		t->string.end = t->source;
		return;
	default:
		t->type = ZPP_TOKEN_TEST_GT;
		t->modifier = 0;
		t->string.start = start;
		t->string.end = t->source;
		return;
	}
}

void zpp_colon_or_declassign(zpp_token* t)
{
	const char* start = t->source++;
	char c = *t->source;

	switch (c)
	{
	case '=':
		t->source++;
		t->type = ZPP_TOKEN_DECL_ASSIGN;
		t->modifier = 0;
		t->string.start = start;
		t->string.end = t->source;
		return;
	default:
		t->type = ZPP_TOKEN_COLON;
		t->modifier = 0;
		t->string.start = start;
		t->string.end = t->source;
		return;
	}
}

void zpp_token_single_line_comment(zpp_token* t)
{
	const char* comment_start = ++t->source;
	char c = *comment_start;

	// Seek until new-line or eof
	while (c != 0 && c != '\n') {
		c = *(++t->source);
	}

	t->type = ZPP_TOKEN_COMMENT;
	t->string.start = comment_start;
	t->string.end = t->source;
}

void zpp_token_multi_line_comment(zpp_token* t)
{
	const char* comment_start = ++t->source;
	const char* commend_end = comment_start;
	char c = *comment_start;

	// Seek until */ or eof
	while (1) {
		if (c == 0) {
			commend_end = t->source;
			//zpp_token_message_unclosed_comment(t);
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

	t->type = ZPP_TOKEN_COMMENT;
	t->string.start = comment_start;
	t->string.end = commend_end;
}

void zpp_token_div_or_comment(zpp_token* t)
{
	const char* start = t->source++;
	char c = *t->source;

	switch (c)
	{
	case '/':
		zpp_token_single_line_comment(t);
		return;
	case '*':
		zpp_token_multi_line_comment(t);
		return;
	default:
		t->type = ZPP_TOKEN_OP_DIV;
		t->modifier = 0;
		t->string.start = start;
		t->string.end = t->source;
		return;
	}
}

zpp_token_type zpp_token_find_keyword_type(const vm_string* str)
{
	const int len = vm_string_length(str);
	if (len == 2) {
		if (vm_str_cmp(str->start, len, "if", 2)) {
			return ZPP_TOKEN_KEYWORD_IF;
		}
	}
	else if (len == 3) {
		if (vm_str_cmp(str->start, len, "var", 3)) {
			return ZPP_TOKEN_KEYWORD_VAR;
		}
		else if (vm_str_cmp(str->start, len, "for", 3)) {
			return ZPP_TOKEN_KEYWORD_FOR;
		}
	}
	else if (len == 4) {
		if (vm_str_cmp(str->start, len, "func", 4)) {
			return ZPP_TOKEN_KEYWORD_FUNC;
		}
		else if (vm_str_cmp(str->start, len, "type", 4)) {
			return ZPP_TOKEN_KEYWORD_TYPE;
		}
		else if (vm_str_cmp(str->start, len, "else", 4)) {
			return ZPP_TOKEN_KEYWORD_ELSE;
		}
	}
	else if (len == 5) {
		if (vm_str_cmp(str->start, len, "const", 5)) {
			return ZPP_TOKEN_KEYWORD_CONST;
		}
	}
	else if (len == 6) {
		if (vm_str_cmp(str->start, len, "import", 6)) {
			return ZPP_TOKEN_KEYWORD_IMPORT;
		}
		else if (vm_str_cmp(str->start, len, "struct", 6)) {
			return ZPP_TOKEN_KEYWORD_STRUCT;
		}
		else if (vm_str_cmp(str->start, len, "return", 6)) {
			return ZPP_TOKEN_KEYWORD_RETURN;
		}
		else if (vm_str_cmp(str->start, len, "extern", 6)) {
			return ZPP_TOKEN_KEYWORD_EXTERN;
		}
	}
	else if (len == 7) {
		if (vm_str_cmp(str->start, len, "package", 7)) {
			return ZPP_TOKEN_KEYWORD_PACKAGE;
		}
	}
	else if (len == 9) {
		if (vm_str_cmp(str->start, len, "interface", 9)) {
			return ZPP_TOKEN_KEYWORD_INTERFACE;
		}
	}
	return ZPP_TOKEN_KEYWORD;
}

void zpp_token_number(zpp_token* token)
{
	// Remember the first character and step to next char
	const char* start = token->source;
	token->source++;

	// Ignore all numbers
	while (zpp_token_test_number(*token->source)) token->source++;

	// Is this is a decimal?
	token->type = ZPP_TOKEN_VALUE_INT;
	if (*token->source == '.') {
		token->type = ZPP_TOKEN_VALUE_DECIMAL;
		token->source++;

		// Ignore all numbers
		while (zpp_token_test_number(*token->source)) token->source++;
	}

	// Is this a hex-decimal value?
	if (*token->source == 'x') {
		token->type = ZPP_TOKEN_VALUE_HEX;
		token->source++;
		// Ignore all hex values
		while (zpp_token_test_hex(*token->source)) token->source++;
	}

	if (token->type == ZPP_TOKEN_VALUE_DECIMAL) {
		// This might be a value with format -3.402823466e+38f
		if (*token->source == 'e') {
			char peek = *(token->source + 1);
			if (peek == '+' || peek == '-') {
				token->source += 2;

				// Ignore all numbers
				while (zpp_token_test_number(*token->source)) token->source++;
			}
		}
	}

	token->string.start = start;
	token->string.end = token->source;
}


void zpp_token_keyword(zpp_token* t)
{
	// Remember the first character and step to next char
	const char* start = t->source;
	t->source++;

	// Ignore all characters
	while (zpp_token_test_char(*t->source)) t->source++;

	t->string.start = start;
	t->string.end = t->source;
	t->type = zpp_token_find_keyword_type(&t->string);
}


void zpp_token_next0(zpp_token* t)
{
	char ch = *t->source;
	t->modifier = 0;

	// Is this a potential keyword?
	if (zpp_token_test_keyword(ch)) {
		zpp_token_keyword(t);
		return;
	}

	// Number?
	if (zpp_token_test_number(ch)) {
		zpp_token_number(t);
		return;
	}

	switch (ch)
	{
	case '\n':
		zpp_token_atom(ZPP_TOKEN_NEWLINE, t);
		t->line++;
		t->line_offset = t->source;
		return;
	case '+':
		zpp_token_atom(ZPP_TOKEN_OP_PLUS, t);
		return;
	case '-':
		zpp_token_atom(ZPP_TOKEN_OP_MINUS, t);
		return;
	case '*':
		zpp_token_atom(ZPP_TOKEN_OP_MULT, t);
		return;
	case '/':
		zpp_token_div_or_comment(t);
		return;
	case '(':
		zpp_token_atom(ZPP_TOKEN_PARAN_L, t);
		return;
	case ')':
		zpp_token_atom(ZPP_TOKEN_PARAN_R, t);
		return;
	case '[':
		zpp_token_atom(ZPP_TOKEN_SQUARE_L, t);
		return;
	case ']':
		zpp_token_atom(ZPP_TOKEN_SQUARE_R, t);
		return;
	case '{':
		zpp_token_atom(ZPP_TOKEN_BRACKET_L, t);
		return;
	case '}':
		zpp_token_atom(ZPP_TOKEN_BRACKET_R, t);
		return;
	case '.':
		zpp_token_atom(ZPP_TOKEN_DOT, t);
		return;
	case ',':
		zpp_token_atom(ZPP_TOKEN_COMMA, t);
		return;
	case '=':
		zpp_assign_or_equals(t);
		return;
	case '<':
		zpp_lt_or_lte(t);
		return;
	case '>':
		zpp_gt_or_gte(t);
		return;
	case ':':
		zpp_colon_or_declassign(t);
		return;
		//case '&':
		//	_zpp_lexer_bitand_or_and(token);
		//	return;
		//case '"':
		//	_zpp_lexer_single_line_string(token);
		//	return;
		//case '#':
		//	zpp_token_atom(ZPP_TOKEN_HASH, token);
		//	return;
		//case '`':
		//	_zpp_lexer_multi_line_string(token);
		//	return;
	case 0:
		zpp_token_eof(t);
		return;
	default:
		zpp_token_unknown(t);
		return;
	}
}

zpp_token_type zpp_token_next(zpp_token* t)
{
	char ch = *t->source;
	if (ch == 0) {
		zpp_token_eof(t);
		return ZPP_TOKEN_EOF;
	}
	while (zpp_token_test_whitespace_ignore_nl(ch)) {
		if (ch == '\n') {
			t->line++;
			t->line_offset = t->source;
		}
		ch = *++t->source;
	}
	zpp_token_next0(t);
	return t->type;
}

vm_int32 zpp_token_i4(zpp_token* t)
{
	return (vm_int32)zpp_strtoi64(t->string.start, vm_string_length(&t->string));
}

vm_uint32 zpp_token_ui4(zpp_token* t)
{
	return (vm_uint32)zpp_strtou64(t->string.start, vm_string_length(&t->string));
}

vm_int64 zpp_token_i8(zpp_token* t)
{
	return zpp_strtoi64(t->string.start, vm_string_length(&t->string));
}

vm_uint64 zpp_token_ui8(zpp_token* t)
{
	return zpp_strtou64(t->string.start, vm_string_length(&t->string));
}

vm_float32 zpp_token_f4(zpp_token* t)
{
	char temp[64];
	temp[sprintf(temp, "%.*s", vm_string_length(&t->string), t->string.start)] = 0;

	const float v = strtof(temp, NULL);
	if (v < (FLT_MAX - FLT_EPSILON) && v >(FLT_MIN + FLT_EPSILON))
		return v;
	else
		return 0.0f;
}

vm_float64 zpp_token_f8(zpp_token* t)
{
	char temp[64];
	temp[sprintf(temp, "%.*s", vm_string_length(&t->string), t->string.start)] = 0;

	const double v = strtod(temp, NULL);
	if (v < (DBL_MAX - DBL_EPSILON) && v >(DBL_MIN + DBL_EPSILON))
		return v;
	else
		return 0.0;
}

BOOL zpp_token_bool(zpp_token* t)
{
	return vm_str_cmp(t->string.start, vm_string_length(&t->string), "true", 4);
}
