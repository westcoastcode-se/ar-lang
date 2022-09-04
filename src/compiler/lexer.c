#include "lexer.h"
#include "tokens.h"
#include "../arUtils.h"
#include <float.h>

// Is the supplied character a valid "first" keyword character
inline BOOL arC_token_test_keyword(arByte c)
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

inline BOOL arC_token_test_uppercase(arByte c)
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

inline BOOL arC_token_test_number(arByte c)
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

inline BOOL arC_token_test_hex(arByte c)
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

inline BOOL arC_token_test_char(arByte c)
{
	return arC_token_test_keyword(c) == TRUE || arC_token_test_number(c) == TRUE;
}

// Test if rune is a whitespace. Newlines are not considered white-space in this function
inline BOOL arC_token_test_whitespace_ignore_nl(arByte c)
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
inline BOOL arC_token_test_whitespace(arByte c)
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
inline BOOL arC_token_test_newline(arByte c)
{
	return c == '\n';
}

// Check to see if the supplied byte is escapable
inline BOOL arC_token_test_escapeable(arByte c)
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

void arC_lexer_init(arC_lexer* l, const arByte* source_code)
{
	l->source_start = source_code;
}

void arC_token_init(arC_token* t, arC_lexer* l)
{
	t->source = t->source_start = l->source_start;
	t->type = ARTOK_UNKNOWN;
	t->modifier = 0;
	arString_zero(&t->string);
	t->line = 0;
	t->line_offset = NULL;
	t->message.code = 0;

	// Prepare the first token
	arC_token_next(t);
}


void arC_token_atom(arC_tokens type, arC_token* token)
{
	token->type = type;
	token->modifier = 0;
	token->string.start = token->source;
	token->string.end = ++token->source;
}

void arC_token_unknown(arC_token* token)
{
	token->string.start = token->source;
	token->string.end = token->source;
	token->type = ARTOK_UNKNOWN;
}

void arC_token_eof(arC_token* t)
{
	t->string.start = t->source;
	t->string.end = t->source;
	t->type = ARTOK_EOF;
}

void arC_assign_or_equals(arC_token* t)
{
	const char* start = t->source++;
	char c = *t->source;

	switch (c)
	{
	case '=':
		t->source++;
		t->type = ARTOK_TEST_EQ;
		t->modifier = 0;
		t->string.start = start;
		t->string.end = t->source;
		return;
	default:
		t->type = ARTOK_ASSIGN;
		t->modifier = 0;
		t->string.start = start;
		t->string.end = t->source;
		return;
	}
}

void arC_lt_or_lte(arC_token* t)
{
	const char* start = t->source++;
	char c = *t->source;

	switch (c)
	{
	case '=':
		t->source++;
		t->type = ARTOK_TEST_LTE;
		t->modifier = 0;
		t->string.start = start;
		t->string.end = t->source;
		return;
	default:
		t->type = ARTOK_TEST_LT;
		t->modifier = 0;
		t->string.start = start;
		t->string.end = t->source;
		return;
	}
}

void arC_gt_or_gte(arC_token* t)
{
	const char* start = t->source++;
	char c = *t->source;

	switch (c)
	{
	case '=':
		t->source++;
		t->type = ARTOK_TEST_GTW;
		t->modifier = 0;
		t->string.start = start;
		t->string.end = t->source;
		return;
	default:
		t->type = ARTOK_TEST_GT;
		t->modifier = 0;
		t->string.start = start;
		t->string.end = t->source;
		return;
	}
}

void arC_colon_or_declassign(arC_token* t)
{
	const char* start = t->source++;
	char c = *t->source;

	switch (c)
	{
	case '=':
		t->source++;
		t->type = ARTOK_DECL_ASSIGN;
		t->modifier = 0;
		t->string.start = start;
		t->string.end = t->source;
		return;
	default:
		t->type = ARTOK_COLON;
		t->modifier = 0;
		t->string.start = start;
		t->string.end = t->source;
		return;
	}
}

void arC_token_single_line_comment(arC_token* t)
{
	const char* comment_start = ++t->source;
	char c = *comment_start;

	// Seek until new-line or eof
	while (c != 0 && c != '\n') {
		c = *(++t->source);
	}

	t->type = ARTOK_COMMENT;
	t->string.start = comment_start;
	t->string.end = t->source;
}

void arC_token_multi_line_comment(arC_token* t)
{
	const char* comment_start = ++t->source;
	const char* commend_end = comment_start;
	char c = *comment_start;

	// Seek until */ or eof
	while (1) {
		if (c == 0) {
			commend_end = t->source;
			//arC_token_message_unclosed_comment(t);
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

	t->type = ARTOK_COMMENT;
	t->string.start = comment_start;
	t->string.end = commend_end;
}

void arC_token_div_or_comment(arC_token* t)
{
	const char* start = t->source++;
	char c = *t->source;

	switch (c)
	{
	case '/':
		arC_token_single_line_comment(t);
		return;
	case '*':
		arC_token_multi_line_comment(t);
		return;
	default:
		t->type = ARTOK_OP_DIV;
		t->modifier = 0;
		t->string.start = start;
		t->string.end = t->source;
		return;
	}
}

void arC_token_plus_or_increment(arC_token* t)
{
	const char* start = t->source++;
	char c = *t->source;

	switch (c)
	{
	case '+':
		t->source++;
		t->type = ARTOK_OP_INC;
		t->modifier = 0;
		t->string.start = start;
		t->string.end = t->source;
		return;
	default:
		t->type = ARTOK_OP_PLUS;
		t->modifier = 0;
		t->string.start = start;
		t->string.end = t->source;
		return;
	}
}

void arC_token_minus_or_decrement(arC_token* t)
{
	const char* start = t->source++;
	char c = *t->source;

	switch (c)
	{
	case '+':
		t->source++;
		t->type = ARTOK_OP_DEC;
		t->modifier = 0;
		t->string.start = start;
		t->string.end = t->source;
		return;
	default:
		t->type = ARTOK_OP_MINUS;
		t->modifier = 0;
		t->string.start = start;
		t->string.end = t->source;
		return;
	}
}

arC_tokens arC_token_find_keyword_type(const arString* str)
{
	const int len = arString_length(str);
	if (len == 2) {
		if (arStrcmp(str->start, len, "if", 2)) {
			return ARTOK_KEYWORD_IF;
		}
	}
	else if (len == 3) {
		if (arStrcmp(str->start, len, "var", 3)) {
			return ARTOK_KEYWORD_VAR;
		}
		else if (arStrcmp(str->start, len, "for", 3)) {
			return ARTOK_KEYWORD_FOR;
		}
	}
	else if (len == 4) {
		if (arStrcmp(str->start, len, "func", 4)) {
			return ARTOK_KEYWORD_FUNC;
		}
		else if (arStrcmp(str->start, len, "type", 4)) {
			return ARTOK_KEYWORD_TYPE;
		}
		else if (arStrcmp(str->start, len, "else", 4)) {
			return ARTOK_KEYWORD_ELSE;
		}
	}
	else if (len == 5) {
		if (arStrcmp(str->start, len, "const", 5)) {
			return ARTOK_KEYWORD_CONST;
		}
	}
	else if (len == 6) {
		if (arStrcmp(str->start, len, "import", 6)) {
			return ARTOK_KEYWORD_IMPORT;
		}
		else if (arStrcmp(str->start, len, "struct", 6)) {
			return ARTOK_KEYWORD_STRUCT;
		}
		else if (arStrcmp(str->start, len, "return", 6)) {
			return ARTOK_KEYWORD_RETURN;
		}
		else if (arStrcmp(str->start, len, "extern", 6)) {
			return ARTOK_KEYWORD_EXTERN;
		}
	}
	else if (len == 7) {
		if (arStrcmp(str->start, len, "package", 7)) {
			return ARTOK_KEYWORD_PACKAGE;
		}
	}
	else if (len == 9) {
		if (arStrcmp(str->start, len, "interface", 9)) {
			return ARTOK_KEYWORD_INTERFACE;
		}
	}
	return ARTOK_IDENTITY;
}

void arC_token_number(arC_token* token)
{
	// Remember the first character and step to next char
	const char* start = token->source;
	token->source++;

	// Ignore all numbers
	while (arC_token_test_number(*token->source)) token->source++;

	// Is this is a decimal?
	token->type = ARTOK_VALUE_INT;
	if (*token->source == '.') {
		token->type = ARTOK_VALUE_DECIMAL;
		token->source++;

		// Ignore all numbers
		while (arC_token_test_number(*token->source)) token->source++;
	}

	// Is this a hex-decimal value?
	if (*token->source == 'x') {
		token->type = ARTOK_VALUE_HEX;
		token->source++;
		// Ignore all hex values
		while (arC_token_test_hex(*token->source)) token->source++;
	}

	if (token->type == ARTOK_VALUE_DECIMAL) {
		// This might be a value with format -3.402823466e+38f
		if (*token->source == 'e') {
			char peek = *(token->source + 1);
			if (peek == '+' || peek == '-') {
				token->source += 2;

				// Ignore all numbers
				while (arC_token_test_number(*token->source)) token->source++;
			}
		}
	}

	token->string.start = start;
	token->string.end = token->source;
}


void arC_token_keyword(arC_token* t)
{
	// Remember the first character and step to next char
	const char* start = t->source;
	t->source++;

	// Ignore all characters
	while (arC_token_test_char(*t->source)) t->source++;

	t->string.start = start;
	t->string.end = t->source;
	t->type = arC_token_find_keyword_type(&t->string);
}


void arC_token_next0(arC_token* t)
{
	char ch = *t->source;
	t->modifier = 0;

	// Is this a potential keyword or identity?
	if (arC_token_test_keyword(ch)) {
		arC_token_keyword(t);
		return;
	}

	// Number?
	if (arC_token_test_number(ch)) {
		arC_token_number(t);
		return;
	}

	switch (ch)
	{
	case '\n':
		arC_token_atom(ARTOK_NEWLINE, t);
		t->line++;
		t->line_offset = t->source;
		return;
	case '+':
		arC_token_plus_or_increment(t);
		return;
	case '-':
		arC_token_minus_or_decrement(t);
		return;
	case '*':
		arC_token_atom(ARTOK_OP_MULT, t);
		return;
	case '/':
		arC_token_div_or_comment(t);
		return;
	case '(':
		arC_token_atom(ARTOK_PARAN_L, t);
		return;
	case ')':
		arC_token_atom(ARTOK_PARAN_R, t);
		return;
	case '[':
		arC_token_atom(ARTOK_SQUARE_L, t);
		return;
	case ']':
		arC_token_atom(ARTOK_SQUARE_R, t);
		return;
	case '{':
		arC_token_atom(ARTOK_BRACKET_L, t);
		return;
	case '}':
		arC_token_atom(ARTOK_BRACKET_R, t);
		return;
	case '.':
		arC_token_atom(ARTOK_DOT, t);
		return;
	case ',':
		arC_token_atom(ARTOK_COMMA, t);
		return;
	case '=':
		arC_assign_or_equals(t);
		return;
	case '<':
		arC_lt_or_lte(t);
		return;
	case '>':
		arC_gt_or_gte(t);
		return;
	case '~':
		arC_token_atom(ARTOK_BIT_NOT, t);
		return;
	case ':':
		arC_colon_or_declassign(t);
		return;
	case 0:
		arC_token_eof(t);
		return;
	default:
		arC_token_unknown(t);
		return;
	}
}

arC_token_type arC_token_next(arC_token* t)
{
	char ch = *t->source;
	if (ch == 0) {
		arC_token_eof(t);
		return ARTOK_EOF;
	}
	while (arC_token_test_whitespace_ignore_nl(ch)) {
		if (ch == '\n') {
			t->line++;
			t->line_offset = t->source;
		}
		ch = *++t->source;
	}
	arC_token_next0(t);
	return t->type;
}

BOOL arC_token_is_keyword(arC_token* t)
{
	return t->type >= ARTOK_KEYWORD_FIRST && t->type <= ARTOK_KEYWORD_LAST;
}

BOOL arC_token_is_operator(arC_token* t)
{
	return t->type >= ARTOK_OP_FIRST && t->type <= ARTOK_OP_LAST;
}

void arC_token_get_metadata(const arC_token* t, int* line, int* line_offset, int* offset)
{
	*line = t->line;
	*line_offset = (int)(t->source - t->line_offset - arString_length(&t->string));
	*offset = (int)(t->source - t->source_start - arString_length(&t->string));
}

arInt32 arC_token_i4(arC_token* t)
{
	return (arInt32)arStrtoi64(t->string.start, arString_length(&t->string));
}

arUint32 arC_token_ui4(arC_token* t)
{
	return (arUint32)arStrtou64(t->string.start, arString_length(&t->string));
}

arInt64 arC_token_i8(arC_token* t)
{
	return arStrtoi64(t->string.start, arString_length(&t->string));
}

arUint64 arC_token_ui8(arC_token* t)
{
	return arStrtou64(t->string.start, arString_length(&t->string));
}

arFloat32 arC_token_f4(arC_token* t)
{
	char temp[64];
	temp[sprintf(temp, "%.*s", arString_length(&t->string), t->string.start)] = 0;

	const float v = strtof(temp, NULL);
	if (v < (FLT_MAX - FLT_EPSILON) && v >(FLT_MIN + FLT_EPSILON))
		return v;
	else
		return 0.0f;
}

arFloat64 arC_token_f8(arC_token* t)
{
	char temp[64];
	temp[sprintf(temp, "%.*s", arString_length(&t->string), t->string.start)] = 0;

	const double v = strtod(temp, NULL);
	if (v < (DBL_MAX - DBL_EPSILON) && v >(DBL_MIN + DBL_EPSILON))
		return v;
	else
		return 0.0;
}

BOOL arC_token_bool(arC_token* t)
{
	return arStrcmp(t->string.start, arString_length(&t->string), "true", 4);
}
