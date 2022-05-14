#ifndef VMC_LEXER_H_
#define VMC_LEXER_H_

#include "../vm_string.h"
#include "../vm_message.h"

enum vmc_lexer_token_type
{
	// generic/unknown keyword, such as "int32"
	VMC_LEXER_TYPE_KEYWORD,
	// fn
	VMC_LEXER_TYPE_KEYWORD_FN,
	// any
	VMC_LEXER_TYPE_KEYWORD_ANY,
	// var
	VMC_LEXER_TYPE_KEYWORD_VAR,
	// type
	VMC_LEXER_TYPE_KEYWORD_TYPE,
	// const
	VMC_LEXER_TYPE_KEYWORD_CONST,
	// import
	VMC_LEXER_TYPE_KEYWORD_IMPORT,
	// struct
	VMC_LEXER_TYPE_KEYWORD_STRUCT,
	// extern
	VMC_LEXER_TYPE_KEYWORD_EXTERN,
	// return
	VMC_LEXER_TYPE_KEYWORD_RETURN,
	// package
	VMC_LEXER_TYPE_KEYWORD_PACKAGE,
	// interface
	VMC_LEXER_TYPE_KEYWORD_INTERFACE,

	// Integers
	VMC_LEXER_TYPE_INT,
	// Integer type in hex format
	VMC_LEXER_TYPE_HEX,
	// Booleans
	VMC_LEXER_TYPE_BOOL,
	// Decimal
	VMC_LEXER_TYPE_DECIMAL,
	// Single- and multiline string
	VMC_LEXER_TYPE_STRING,

	// comment, such as // or /**/
	VMC_LEXER_TYPE_COMMENT,

	// +
	VMC_LEXER_TYPE_PLUS,
	// -
	VMC_LEXER_TYPE_MINUS,
	// *, used both for mathematics and as a type modifier
	VMC_LEXER_TYPE_MULT,
	VMC_LEXER_TYPE_PTR = VMC_LEXER_TYPE_MULT,
	// /
	VMC_LEXER_TYPE_DIV,
	VMC_LEXER_TYPE_SLASH = VMC_LEXER_TYPE_DIV,

	// (
	VMC_LEXER_TYPE_PARAN_L,
	// )
	VMC_LEXER_TYPE_PARAN_R,

	// [
	VMC_LEXER_TYPE_SQUARE_L,
	// ]
	VMC_LEXER_TYPE_SQUARE_R,

	// {
	VMC_LEXER_TYPE_BRACKET_L,
	// }
	VMC_LEXER_TYPE_BRACKET_R,

	// ,
	VMC_LEXER_TYPE_COMMA,
	// .
	VMC_LEXER_TYPE_DOT,

	// &, used for both bit manipulations and as a value modifier
	VMC_LEXER_TYPE_BIT_AND,
	VMC_LEXER_TYPE_ADDR_OF = VMC_LEXER_TYPE_BIT_AND,

	// &&
	VMC_LEXER_TYPE_TEST_AND,

	// \n
	VMC_LEXER_TYPE_NEWLINE,

	// END OF FILE 
	VMC_LEXER_TYPE_EOF,

	// Represents an unknown token
	VMC_LEXER_TYPE_UNKNOWN
};
typedef enum vmc_lexer_token_type vmc_lexer_token_type;

// Check to see if the supplied lexer type is a keyword - known or unknown
static inline BOOL vmc_lexer_type_is_keyword(vmc_lexer_token_type type)
{
	switch (type) {
	case VMC_LEXER_TYPE_KEYWORD:
	case VMC_LEXER_TYPE_KEYWORD_FN:
	case VMC_LEXER_TYPE_KEYWORD_ANY:
	case VMC_LEXER_TYPE_KEYWORD_VAR:
	case VMC_LEXER_TYPE_KEYWORD_TYPE:
	case VMC_LEXER_TYPE_KEYWORD_CONST:
	case VMC_LEXER_TYPE_KEYWORD_IMPORT:
	case VMC_LEXER_TYPE_KEYWORD_STRUCT:
	case VMC_LEXER_TYPE_KEYWORD_EXTERN:
	case VMC_LEXER_TYPE_KEYWORD_RETURN:
	case VMC_LEXER_TYPE_KEYWORD_PACKAGE:
	case VMC_LEXER_TYPE_KEYWORD_INTERFACE:
		return TRUE;
	default:
		return FALSE;
	}
}

// Various modifiers that a type can have. For example, a string can be a single or a multi-line. An integer can be
// based on an integer, a binary or a hex value
enum vmc_lexer_token_modifier
{
	vml_modifier_none
};
typedef enum vmc_lexer_token_modifier vmc_lexer_token_modifier;

enum vmc_lexer_token_modifiers
{
	// Is the token a multiline token
	VMC_LEXER_TOKEN_MODIFIER_MULTILINE = (1 << 0),

	// Does the token contain escaped characters
	VMC_LEXER_TOKEN_MODIFIER_ESCAPED = (1 << 1)
};
typedef enum vmc_lexer_token_modifiers vmc_lexer_token_modifiers;

// Represents a specific token
struct vmc_lexer_token
{
	vmc_lexer_token_type type;
	vmc_lexer_token_modifier modifier;
	vm_string string;
};
typedef struct vmc_lexer_token vmc_lexer_token;

// Lexer
struct vmc_lexer
{
	const vm_byte* source_start;
	const vm_byte* source;

	// The current line in the source code
	int line;
	
	// Where the line starts
	const vm_byte* line_offset;

	// Messages raised by the lexer
	vm_messages messages;
};
typedef struct vmc_lexer vmc_lexer;

// Initialize the supplied lexer with the supplied source code
extern void vmc_lexer_init(vmc_lexer* l, const vm_byte* source);

// Parse the supplied source code and return byte code as the result.
extern vmc_lexer* vmc_lexer_parse(const vm_byte* source);

// Release internal properties of the supplied lexer
extern void vmc_lexer_release(vmc_lexer* l);

// Delete the supplied lexer
extern void vmc_lexer_destroy(vmc_lexer* l);

// Get next non-whitespace token
extern void vmc_lexer_next(vmc_lexer* l, vmc_lexer_token* token);

// Get the next non-whitespace token. A newline is not considered whitespace when calling this function
extern void vmc_lexer_next_newline(vmc_lexer* l, vmc_lexer_token* token);

// Parse the next token and return TRUE if the supplied token is the supplied type
extern BOOL vmc_lexer_next_type(vmc_lexer* l, vmc_lexer_token_modifier type, vmc_lexer_token* token);

// Peek at the next character
extern char vmc_lexer_peek(vmc_lexer* l);

// Get metadata where we are
inline static void vmc_lexer_get_metadata(vmc_lexer* l, int* line, int* line_offset, int* offset)
{
	*line = l->line;
	*line_offset = (int)(l->source - l->line_offset);
	*offset = (int)(l->source - l->source_start);
}

// Check to see if the lexer has compiled successfully
inline static BOOL vmc_lexer_success(vmc_lexer* l)
{
	return vm_messages_has_messages(&l->messages) == FALSE;
}

#endif
