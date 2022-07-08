#ifndef _VMCD_LEXER_H_
#define _VMCD_LEXER_H_

#include "../vm_string.h"
#include "../vm_message.h"

enum vmcd_token_type
{
	VMCD_TOKEN_UNKNOWN = 0,

	VMCD_TOKEN_KEYWORD,
	VMCD_TOKEN_KEYWORD_IF,
	VMCD_TOKEN_KEYWORD_FOR,
	VMCD_TOKEN_KEYWORD_VAR,
	VMCD_TOKEN_KEYWORD_FUNC,
	VMCD_TOKEN_KEYWORD_TYPE,
	VMCD_TOKEN_KEYWORD_ELSE,
	VMCD_TOKEN_KEYWORD_CONST,
	VMCD_TOKEN_KEYWORD_RETURN,
	VMCD_TOKEN_KEYWORD_STRUCT,
	VMCD_TOKEN_KEYWORD_IMPORT,
	VMCD_TOKEN_KEYWORD_EXTERN,
	VMCD_TOKEN_KEYWORD_PACKAGE,
	VMCD_TOKEN_KEYWORD_INTERFACE,
	VMCD_TOKEN_KEYWORD_END_KEYWORD = VMCD_TOKEN_KEYWORD_INTERFACE,


	VMCD_TOKEN_INT,
	VMCD_TOKEN_HEX,
	VMCD_TOKEN_BOOL,
	VMCD_TOKEN_DECIMAL,
	VMCD_TOKEN_STRING,

	VMCD_TOKEN_COMMENT,

	VMCD_TOKEN_PLUS,
	VMCD_TOKEN_MINUS,
	VMCD_TOKEN_MULT,
	VMCD_TOKEN_DIV,
	VMCD_TOKEN_PTR = VMCD_TOKEN_MULT,
	VMCD_TOKEN_SLASH = VMCD_TOKEN_DIV,

	// (
	VMCD_TOKEN_PARAN_L,
	// )
	VMCD_TOKEN_PARAN_R,

	// [
	VMCD_TOKEN_SQUARE_L,
	// ]
	VMCD_TOKEN_SQUARE_R,

	// {
	VMCD_TOKEN_BRACKET_L,
	// }
	VMCD_TOKEN_BRACKET_R,

	// ==
	VMCD_TOKEN_EQUALS,
	// !=
	VMCD_TOKEN_NOT_EQUALS,

	// ,
	VMCD_TOKEN_COMMA,
	// .
	VMCD_TOKEN_DOT,
	// :
	VMCD_TOKEN_COLON,
	// =
	VMCD_TOKEN_ASSIGN,
	// :=
	VMCD_TOKEN_DECL_ASSIGN,

	// |
	VMCD_TOKEN_BIT_OR,
	// &
	VMCD_TOKEN_BIT_AND,
	VMCD_TOKEN_ADDR_OF = VMCD_TOKEN_BIT_AND,

	// ||
	VMCD_TOKEN_OR,
	// &&
	VMCD_TOKEN_AND,
	// !
	VMCD_TOKEN_NOT,
	
	// \n
	VMCD_TOKEN_NEWLINE,
	
	// <EOF>
	VMCD_TOKEN_EOF,
};
typedef enum vmcd_token_type vmcd_token_type;

// Lexer used to interpret vmcd soure code
struct vmcd_lexer
{
	// The actual source we are parsing
	const vm_byte* source_start;
};
typedef struct vmcd_lexer vmcd_lexer;

// Lexer token used for vmcd soure code
struct vmcd_token
{
	const vm_byte* source_start;
	const vm_byte* source;

	vmcd_token_type type;
	vm_int32 modifier;
	vm_string string;

	// The current line in the source code
	int line;

	// Where the line starts
	const vm_byte* line_offset;

	// Messages raised by the lexer
	vm_messages messages;
};
typedef struct vmcd_token vmcd_token;

// Initialize the lexer
extern void vmcd_lexer_init(vmcd_lexer* l, const vm_byte* source_code);

// Release the lexer
extern void vmcd_lexer_release(vmcd_lexer* l);

// Start processing a token
extern void vmcd_token_init(vmcd_token* t, vmcd_lexer* l);

// Release the tokens internal resources
extern void vmcd_token_release(vmcd_token* t);

// Get the next token
extern void vmcd_token_next(vmcd_token* t);

// Get the next token
extern BOOL vmcd_token_next_type(vmcd_token* t, vmcd_token_type type);

// Get metadata where we are
inline static void vmcd_token_get_metadata(const vmcd_token* t, int* line, int* line_offset, int* offset)
{
	*line = t->line;
	*line_offset = (int)(t->source - t->line_offset);
	*offset = (int)(t->source - t->source_start);
}

// Is the supplied character a valid "first" keyword character
static inline BOOL vmcd_token_test_keyword(vm_byte c)
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

static inline BOOL vmcd_token_test_uppercase(vm_byte c)
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

static inline BOOL vmcd_token_test_number(vm_byte c)
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

static inline BOOL vmcd_token_test_hex(vm_byte c)
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

static inline BOOL vmc_token_test_char(vm_byte c)
{
	return vmcd_token_test_keyword(c) == TRUE || vmcd_token_test_number(c) == TRUE;
}

// Test if rune is a whitespace. Newlines are not considered white-space in this function
static inline BOOL vmcd_token_test_whitespace_ignore_nl(vm_byte c)
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
static inline BOOL vmcd_loken_test_whitespace(vm_byte c)
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

static inline BOOL vmcd_token_test_newline(vm_byte c)
{
	return c == '\n';
}

// Check to see if the supplied byte is escapable
static inline BOOL vmcd_token_test_escapeable(vm_byte c)
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

// Check to see if the supplied token is a keyword - known or unknown
static inline BOOL vmcd_token_is_keyword(vmcd_token* t)
{
	return t->type >= VMCD_TOKEN_KEYWORD && t->type <= VMCD_TOKEN_KEYWORD_END_KEYWORD;
}

// Convert the token content into a 4 byte integer
extern vm_int32 vmcd_token_i4(vmcd_token* t);

// Convert the token content into a 4 byte integer
extern vm_uint32 vmcd_token_ui4(vmcd_token* t);

// Convert the token content into a 8 byte integer
extern vm_int64 vmcd_token_i8(vmcd_token* t);

// Convert the token content into a 8 byte integer
extern vm_uint64 vmcd_token_ui8(vmcd_token* t);

// Convert the supplied c-string into a 64 bit unsigned integer. The size of the string is limited to the supplied length 
static inline vm_uint64 strtou64(const char* str, int len)
{
	vm_uint64 ret = 0;
	for (int i = 0; i < len; ++i)
		ret = (ret * (vm_uint64)10) + (vm_uint64)(str[i] - '0');
	return ret;
}

// Convert the supplied c-string into a 64 bit integer. The size of the string is limited to the supplied length 
static inline vm_int64 strtoi64(const char* str, int len)
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
static inline vm_int32 get_align_size2(vm_int32 size, vm_int32 align, vm_int32 ao)
{
	const vm_int32 padding = (align - (size & ao)) & ao;
	return size + padding;
}

// Figure out the size if we want a fixed memory alignment
static inline vm_int32 get_align_size(vm_int32 size, vm_int32 align)
{
	return get_align_size2(size, align, align - 1);
}

// Convert a hex key into an integer value
static inline vm_int32 hex_key_to_int(const char c)
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
static inline vm_int64 hextoi64(const char* hex, int len)
{
	vm_int64 ret = 0;
	const char* end = hex + len;
	if (len < 3)
		return 0;
	hex += 2;
	for (; hex != end; ++hex)
		ret = (ret * 16) + (vm_int64)hex_key_to_int(*hex);
	return ret;
}

// Convert a hex value into a 64 bit unsigned integer
static inline vm_uint64 hextou64(const char* hex, int len)
{
	vm_uint64 ret = 0;
	const char* end = hex + len;
	if (len < 3)
		return 0;
	hex += 2;
	for (; hex != end; ++hex)
		ret = (ret * 16) + (vm_uint64)hex_key_to_int(*hex);
	return ret;
}

#endif
