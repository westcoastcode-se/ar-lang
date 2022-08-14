#ifndef _ZPP_LEXER_H_
#define _ZPP_LEXER_H_

#include "../vm_string.h"
#include "../vm_message.h"

typedef enum zpp_token_type
{
	ZPP_TOKEN_UNKNOWN = 0,

	ZPP_TOKEN_KEYWORD,
	ZPP_TOKEN_KEYWORD_IF,
	ZPP_TOKEN_KEYWORD_FOR,
	ZPP_TOKEN_KEYWORD_VAR,
	ZPP_TOKEN_KEYWORD_FUNC,
	ZPP_TOKEN_KEYWORD_TYPE,
	ZPP_TOKEN_KEYWORD_ELSE,
	ZPP_TOKEN_KEYWORD_CONST,
	ZPP_TOKEN_KEYWORD_RETURN,
	ZPP_TOKEN_KEYWORD_STRUCT,
	ZPP_TOKEN_KEYWORD_IMPORT,
	ZPP_TOKEN_KEYWORD_EXTERN,
	ZPP_TOKEN_KEYWORD_PACKAGE,
	ZPP_TOKEN_KEYWORD_INTERFACE,
	ZPP_TOKEN_KEYWORD_END_KEYWORD = ZPP_TOKEN_KEYWORD_INTERFACE,

	ZPP_TOKEN_VALUE,
	ZPP_TOKEN_VALUE_INT,
	ZPP_TOKEN_VALUE_HEX,
	ZPP_TOKEN_VALUE_BOOL,
	ZPP_TOKEN_VALUE_DECIMAL,
	ZPP_TOKEN_VALUE_STRING,
	ZPP_TOKEN_END_VALUE = ZPP_TOKEN_VALUE_STRING,

	ZPP_TOKEN_COMMENT,

	ZPP_TOKEN_OP,
	// +
	ZPP_TOKEN_OP_PLUS,
	// -
	ZPP_TOKEN_OP_MINUS,
	// *
	ZPP_TOKEN_OP_MULT,
	ZPP_TOKEN_PTR = ZPP_TOKEN_OP_MULT,
	// /
	ZPP_TOKEN_OP_DIV,
	// |
	ZPP_TOKEN_BIT_OR,
	// &
	ZPP_TOKEN_BIT_AND,
	ZPP_TOKEN_ADDR_OF = ZPP_TOKEN_BIT_AND,
	// ==
	ZPP_TOKEN_TEST_EQUALS,
	// !=
	ZPP_TOKEN_TEST_NOT_EQUALS,
	// ||
	ZPP_TOKEN_TEST_OR,
	// &&
	ZPP_TOKEN_TEST_AND,
	ZPP_TOKEN_END_OP,

	// !
	ZPP_TOKEN_NOT,

	// (
	ZPP_TOKEN_PARAN_L,
	// )
	ZPP_TOKEN_PARAN_R,

	// [
	ZPP_TOKEN_SQUARE_L,
	// ]
	ZPP_TOKEN_SQUARE_R,

	// {
	ZPP_TOKEN_BRACKET_L,
	// }
	ZPP_TOKEN_BRACKET_R,

	// <
	ZPP_TOKEN_TEST_LT,
	// <=
	ZPP_TOKEN_TEST_LTE,
	// >
	ZPP_TOKEN_TEST_GT,
	// >=
	ZPP_TOKEN_TEST_GTE,

	// ,
	ZPP_TOKEN_COMMA,
	// .
	ZPP_TOKEN_DOT,
	// :
	ZPP_TOKEN_COLON,
	// =
	ZPP_TOKEN_ASSIGN,
	// :=
	ZPP_TOKEN_DECL_ASSIGN,

	// \n
	ZPP_TOKEN_NEWLINE,

	// <EOF>
	ZPP_TOKEN_EOF,
} zpp_token_type;

// Modifier
typedef enum zpp_token_modifier
{
	ZPP_TOKEN_MOD_NONE = 0,
} zpp_token_modifier;

// Lexer used to interpret vmcd soure code
typedef struct zpp_lexer
{
	// The actual source we are parsing
	const vm_byte* source_start;
} zpp_lexer;

// A lexer token. This is where the actual meat is happening
typedef struct zpp_token
{
	const vm_byte* source_start;
	const vm_byte* source;

	zpp_token_type type;
	zpp_token_modifier modifier;
	vm_string string;

	// The current line in the source code
	vm_int32 line;

	// Where the line starts
	const vm_byte* line_offset;

	// Latest raised message
	vm_message message;
} zpp_token;

// Initialize the lexer
extern void zpp_lexer_init(zpp_lexer* l, const vm_byte* source_code);

// Start processing a token
extern void zpp_token_init(zpp_token* t, zpp_lexer* l);

// Get the next token
extern zpp_token_type zpp_token_next(zpp_token* t);

// Check to see if the supplied token is a keyword - known or unknown
static inline BOOL zpp_token_is_keyword(zpp_token* t)
{
	return t->type >= ZPP_TOKEN_KEYWORD && t->type <= ZPP_TOKEN_KEYWORD_END_KEYWORD;
}

// Check to see if the supplied token is a value
static inline BOOL zpp_token_is_value(zpp_token* t)
{
	return t->type >= ZPP_TOKEN_VALUE && t->type <= ZPP_TOKEN_END_VALUE;
}

// Check to see if the supplied token is a value
static inline BOOL zpp_token_is_operator(zpp_token* t)
{
	return t->type >= ZPP_TOKEN_OP && t->type <= ZPP_TOKEN_END_OP;
}

// Get metadata where we are
inline static void zpp_token_get_metadata(const zpp_token* t, int* line, int* line_offset, int* offset)
{
	*line = t->line;
	*line_offset = (int)(t->source - t->line_offset - vm_string_length(&t->string));
	*offset = (int)(t->source - t->source_start - vm_string_length(&t->string));
}

// Convert the token content into a 4 byte integer
extern vm_int32 zpp_token_i4(zpp_token* t);

// Convert the token content into a 4 byte integer
extern vm_uint32 zpp_token_ui4(zpp_token* t);

// Convert the token content into a 8 byte integer
extern vm_int64 zpp_token_i8(zpp_token* t);

// Convert the token content into a 8 byte integer
extern vm_uint64 zpp_token_ui8(zpp_token* t);

// Convert the token content into a 4 decimal number
extern vm_float32 zpp_token_f4(zpp_token* t);

// Convert the token content into a 8 decimal number
extern vm_float64 zpp_token_f8(zpp_token* t);

// Convert the token content into a boolean value
extern BOOL zpp_token_bool(zpp_token* t);

// Convert the supplied c-string into a 64 bit unsigned integer. The size of the string is limited to the supplied length 
static inline vm_uint64 zpp_strtou64(const char* str, int len)
{
	vm_uint64 ret = 0;
	for (int i = 0; i < len; ++i)
		ret = (ret * (vm_uint64)10) + (vm_uint64)(str[i] - '0');
	return ret;
}

// Convert the supplied c-string into a 64 bit integer. The size of the string is limited to the supplied length 
static inline vm_int64 zpp_strtoi64(const char* str, int len)
{
	vm_int64 ret = 0;

	// Special handling for a negative values
	vm_int64 multiplier = 1;
	if (len > 0 && *str == '-')
	{
		multiplier = -1;
		len--;
		str++;
	}

	for (int i = 0; i < len; ++i)
		ret = (ret * (vm_int64)10) + (vm_int64)(str[i] - '0');
	return ret * multiplier;
}

// Figure out the size if we want a fixed memory alignment
static inline vm_int32 zpp_get_align_size2(vm_int32 size, vm_int32 align, vm_int32 ao)
{
	const vm_int32 padding = (align - (size & ao)) & ao;
	return size + padding;
}

// Figure out the size if we want a fixed memory alignment
static inline vm_int32 zpp_get_align_size(vm_int32 size, vm_int32 align)
{
	return zpp_get_align_size2(size, align, align - 1);
}

// Convert a hex key into an integer value
static inline vm_int32 zpp_hex_key_to_int(const char c)
{
	switch (c)
	{
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
		return c - '0';
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
		return c - 'a' + 10;
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
		return c - 'A' + 10;
	default:
		return 0;
	}
}

// Convert a hex value into a 64 bit integer
static inline vm_int64 zpp_hextoi64(const char* hex, int len)
{
	vm_int64 ret = 0;
	const char* end = hex + len;
	if (len < 3)
		return 0;
	hex += 2;
	for (; hex != end; ++hex)
		ret = (ret * 16) + (vm_int64)zpp_hex_key_to_int(*hex);
	return ret;
}

// Convert a hex value into a 64 bit unsigned integer
static inline vm_uint64 zpp_hextou64(const char* hex, int len)
{
	vm_uint64 ret = 0;
	const char* end = hex + len;
	if (len < 3)
		return 0;
	hex += 2;
	for (; hex != end; ++hex)
		ret = (ret * 16) + (vm_uint64)zpp_hex_key_to_int(*hex);
	return ret;
}

#endif
