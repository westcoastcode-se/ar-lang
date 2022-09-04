#ifndef _ARC_LEXER_H_
#define _ARC_LEXER_H_

#include "../arString.h"
#include "../arMessage.h"

// Represents a token
typedef int arC_token_type;

// Modifier
typedef int arC_token_modifier;

// Lexer used to interpret vmcd soure code
typedef struct arC_lexer
{
	// The actual source we are parsing
	const arByte* source_start;
} arC_lexer;

// A lexer token. This is where the actual meat is happening
typedef struct arC_token
{
	const arByte* source_start;
	const arByte* source;

	arC_token_type type;
	arC_token_modifier modifier;
	arString string;

	// The current line in the source code
	arInt32 line;

	// Where the line starts
	const arByte* line_offset;

	// Latest raised message
	arMessage message;
} arC_token;

// Initialize the lexer
ARLANG_API void arC_lexer_init(arC_lexer* l, const arByte* source_code);

// Start processing a token
ARLANG_API void arC_token_init(arC_token* t, arC_lexer* l);

// Get the next token
ARLANG_API arC_token_type arC_token_next(arC_token* t);

// Check to see if the supplied token is a keyword - known or unknown
ARLANG_API BOOL arC_token_is_keyword(arC_token* t);

// Check to see if the supplied token is a value
ARLANG_API BOOL arC_token_is_operator(arC_token* t);

// Get metadata where we are
ARLANG_API void arC_token_get_metadata(const arC_token* t, int* line, int* line_offset, int* offset);

// Convert the token content into a 4 byte integer
ARLANG_API arInt32 arC_token_i4(arC_token* t);

// Convert the token content into a 4 byte integer
ARLANG_API arUint32 arC_token_ui4(arC_token* t);

// Convert the token content into a 8 byte integer
ARLANG_API arInt64 arC_token_i8(arC_token* t);

// Convert the token content into a 8 byte integer
ARLANG_API arUint64 arC_token_ui8(arC_token* t);

// Convert the token content into a 4 decimal number
ARLANG_API arFloat32 arC_token_f4(arC_token* t);

// Convert the token content into a 8 decimal number
ARLANG_API arFloat64 arC_token_f8(arC_token* t);

// Convert the token content into a boolean value
ARLANG_API BOOL arC_token_bool(arC_token* t);

#endif
