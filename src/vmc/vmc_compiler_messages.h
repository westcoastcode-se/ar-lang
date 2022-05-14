#ifndef _VMC_COMPILER_MESSAGES_H_
#define _VMC_COMPILER_MESSAGES_H_

// Prefix for all compiler messages
#define VMC_COMPILER_MESSAGE_PREFIX 'C'

// All messages
enum vmc_compiler_messages
{
	// Messages that's a level 0 message are considered errors
	VMC_COMPILER_MSG_LEVEL0 = 0,
	
	// Received a token from the lexer which the compiler has no idea
	// on what to do with
	VMC_COMPILER_MSG_UNKNOWN_TOKEN,

	// Compiler requires an keyword that represents an identifier
	VMC_COMPILER_MSG_EXPECTED_IDENTIFIER,

	// Compiler requires an keyword that represents an type
	VMC_COMPILER_MSG_EXPECTED_TYPE,

	// The given type cannot be resolved
	VMC_COMPILER_MSG_TYPE_NOT_FOUND,
};

#define VMC_COMPILER_MSG_UNKNOWN_TOKEN_STR "unknown token: '%.*s' at %d:%d"
#define VMC_COMPILER_MSG_EXPECTED_IDENTIFIER_STR "expected identifier but was: '%.*s' at %d:%d"
#define VMC_COMPILER_MSG_EXPECTED_TYPE_STR "expected type but was: '%.*s' at %d:%d"
#define VMC_COMPILER_MSG_TYPE_NOT_FOUND_STR "type '%.*s' was not found at %d:%d"

#endif

