#ifndef _ZPP_MESSAGES_H_
#define _ZPP_MESSAGES_H_

#include "../vm_message.h"

typedef enum zpp_message_code
{
	ZPP_MESSAGE_NONE = 0,

	// Messages of level 0 (critical) is below this 
	ZPP_MESSAGE_LEVEL0,
	
	// The device is out of memory
	ZPP_MESSAGE_OUT_OF_MEMORY,

	// Not implemented
	ZPP_MESSAGE_NOT_IMPLEMENTED,

	// Compiler requires an keyword that represents an identifier
	ZPP_MESSAGE_EXPECTED_IDENTIFIER,

	// Generic syntax error
	ZPP_MESSAGE_SYNTAX_ERROR,
} zpp_message_code;

#define ZPP_MESSAGE_PREFIX 'C'
#define ZPP_MESSAGE_NOT_IMPLEMENTED_STR "not implemented: '%.*s' at %d:%d"
#define ZPP_MESSAGE_EXPECTED_IDENTIFIER_STR "expected identifier but was: '%.*s' at %d:%d"
#define ZPP_MESSAGE_SYNTAX_ERROR_STR "%s but was '%.*s' at %d:%d"

extern BOOL zpp_message_panic(const struct zpp_compiler_state* s, const char* str, zpp_message_code code);
extern BOOL zpp_message_out_of_memory(const struct zpp_compiler_state* s);
extern BOOL zpp_message_not_implemented(const struct zpp_compiler_state* s);
extern BOOL zpp_message_expected_identifier(const struct zpp_compiler_state* s);
extern BOOL zpp_message_syntax_error(const struct zpp_compiler_state* s, const char* msg);

#endif
