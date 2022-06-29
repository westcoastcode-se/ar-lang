#ifndef _VMC_COMPILER_MESSAGES_H_
#define _VMC_COMPILER_MESSAGES_H_

#include "../vm_message.h"
#include "vmc_lexer.h"

// Prefix for all compiler messages
#define VMC_COMPILER_MESSAGE_PREFIX 'C'

// All messages
enum vmc_compiler_messages
{
	// Represents a non-existing message
	VMC_COMPILER_MSG_NONE = 0,

	// Messages that's a level 0 message are considered errors
	VMC_COMPILER_MSG_LEVEL0,

	// Panic!!!
	VMC_COMPILER_MSG_PANIC,
	
	// Received a token from the lexer which the compiler has no idea
	// on what to do with
	VMC_COMPILER_MSG_UNKNOWN_TOKEN,

	// Compiler requires an keyword that represents an identifier
	VMC_COMPILER_MSG_EXPECTED_IDENTIFIER,

	// Compiler requires an keyword that represents an type
	VMC_COMPILER_MSG_EXPECTED_TYPE,

	// The given type cannot be resolved
	VMC_COMPILER_MSG_TYPE_NOT_FOUND,

	// A keyword is expected, such as "push"
	VMC_COMPILER_MSG_EXPECTED_KEYWORD,

	// Expected an index, such as if we are referring to an argument
	VMC_COMPILER_MSG_EXPECTED_INDEX,

	// Expected an integer value. This is normally raised if you have a constant int value but with no integer as a value
	VMC_COMPILER_MSG_EXPECTED_INT,

	// Expected a string value
	VMC_COMPILER_MSG_EXPECTED_STRING,

	// Expected a decimal value
	VMC_COMPILER_MSG_EXPECTED_DECIMAL,

	// Message raised if a compiler feature is not implemented yet
	VMC_COMPILER_MSG_NOT_IMPLEMENTED,

	// Invalid index normally happens if you refer to an index that doesnt exist
	VMC_COMPILER_MSG_INVALID_INDEX,

	// Invalid size happens when you try to manage a larger value than the container allows
	VMC_COMPILER_MSG_INVALID_SIZE,

	// Unexpected end-of-file
	VMC_COMPILER_MSG_UNEXPECTED_EOF,

	// Generic syntax error
	VMC_COMPILER_MSG_SYNTAX_ERROR,

	// Function already has a body
	VMC_COMPILER_MSG_FUNC_BODY_EXISTS,

	// A memory marker already exist with the same name
	VMC_COMPILER_MSG_MEMORY_MARKER_ALREADY_EXIST,

	// A definition array is too small, such as the arguments array in the function definition
	VMC_COMPILER_MSG_DEFARRAY_TOO_SMALL,

	// Message raised if a compiler reaches a newline where it didnt expect it
	VMC_COMPILER_MSG_UNEXPECTED_NEWLINE
};

#define VMC_COMPILER_MSG_UNKNOWN_TOKEN_STR "unknown token: '%.*s' at %d:%d"
#define VMC_COMPILER_MSG_EXPECTED_IDENTIFIER_STR "expected identifier but was: '%.*s' at %d:%d"
#define VMC_COMPILER_MSG_EXPECTED_TYPE_STR "expected type but was: '%.*s' at %d:%d"
#define VMC_COMPILER_MSG_TYPE_NOT_FOUND_STR "type '%.*s' was not found at %d:%d"
#define VMC_COMPILER_MSG_EXPECTED_KEYWORD_STR "expected keyword but was '%.*s' at %d:%d"
#define VMC_COMPILER_MSG_EXPECTED_INDEX_STR "expected index but was '%.*s' at %d:%d"
#define VMC_COMPILER_MSG_EXPECTED_INT_STR "expected integer value but was '%.*s' at %d:%d"
#define VMC_COMPILER_MSG_EXPECTED_STRING_STR "expected string value but was '%.*s' at %d:%d"
#define VMC_COMPILER_MSG_EXPECTED_DECIMAL_STR "expected decimal value but was '%.*s' at %d:%d"
#define VMC_COMPILER_MSG_NOT_IMPLEMENTED_STR "keyword '%.*s' is not implemented yet at %d:%d"
#define VMC_COMPILER_MSG_INVALID_INDEX_STR "supplied index %d but it must be within range [%d,%d] at %d:%d"
#define VMC_COMPILER_MSG_INVALID_SIZE_STR "supplied size %d but it must be within range [%d,%d] at %d:%d"
#define VMC_COMPILER_MSG_UNEXPECTED_EOF_STR "unexpected end of file found at %d:%d"
#define VMC_COMPILER_MSG_SYNTAX_ERROR_STR "syntax error: missing '%c' before '%.*s' at %d:%d"
#define VMC_COMPILER_MSG_FUNC_BODY_EXISTS_STR "function '%.*s' already has a body at %d:%d"
#define VMC_COMPILER_MSG_MEMORY_MARKER_ALREADY_EXIST_STR "memory marker '%.*s' at %d:%d already exist"
#define VMC_COMPILER_MSG_DEFARRAY_TOO_SMALL_STR "you cannot have more definitions than %d at %d:%d"
#define VMC_COMPILER_MSG_UNEXPECTED_NEWLINE_STR "unexpected newline reached at %d:%d"

// 
// Functions which helps adding messages to the messages container
//

extern BOOL vmc_compiler_message_panic(vm_message* m, const char* str);
extern BOOL vmc_compiler_message_unknown_token(vm_messages* m, vmc_lexer_token* t);
extern BOOL vmc_compiler_message_expected_identifier(vm_messages* m, vmc_lexer_token* t);
extern BOOL vmc_compiler_message_expected_type(vm_messages* m, vmc_lexer_token* t);
extern BOOL vmc_compiler_message_type_not_found(vm_messages* m, vmc_lexer_token* t);
extern BOOL vmc_compiler_message_expected_keyword(vm_messages* m, vmc_lexer_token* t);
extern BOOL vmc_compiler_message_expected_index(vm_messages* m, vmc_lexer_token* t);
extern BOOL vmc_compiler_message_expected_int(vm_messages* m, vmc_lexer_token* t);
extern BOOL vmc_compiler_message_expected_string(vm_messages* m, vmc_lexer_token* t);
extern BOOL vmc_compiler_message_expected_decimal(vm_messages* m, vmc_lexer_token* t);
extern BOOL vmc_compiler_message_not_implemented(vm_messages* m, vmc_lexer_token* t);
extern BOOL vmc_compiler_message_invalid_index(vm_messages* m, vmc_lexer_token* t, vm_int32 index, vm_int32 min, vm_int32 max);
extern BOOL vmc_compiler_message_invalid_size(vm_messages* m, vmc_lexer_token* t, vm_int32 size, vm_int32 min, vm_int32 max);
extern BOOL vmc_compiler_message_unexpected_eof(vm_messages* m, vmc_lexer_token* t);
extern BOOL vmc_compiler_message_syntax_error(vm_messages* m, vmc_lexer_token* t, char expected);
extern BOOL vmc_compiler_message_func_body_exists(vm_messages* m, vmc_lexer_token* t, const vm_string* signature);
extern BOOL vmc_compiler_message_memory_marker_exists(vm_messages* m, vmc_lexer_token* t, const vm_string* memory_marker);
extern BOOL vmc_compiler_message_defarray_to_small(vm_messages* m, vmc_lexer_token* t, vm_int32 size);
extern BOOL vmc_compiler_message_unexpected_newline(vm_messages* m, vmc_lexer_token* t);

#endif

