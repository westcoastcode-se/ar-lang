#ifndef _VMC_COMPILER_MESSAGES_H_
#define _VMC_COMPILER_MESSAGES_H_

#include "../vm_message.h"
#include "vmc_lexer.h"

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

	// A keyword is expected, such as "push"
	VMC_COMPILER_MSG_EXPECTED_KEYWORD,

	// Expected an index, such as if we are referring to an argument
	VMC_COMPILER_MSG_EXPECTED_INDEX,

	// Expected an integer value. This is normally raised if you have a constant int value but with no integer as a value
	VMC_COMPILER_MSG_EXPECTED_INT,
};

#define VMC_COMPILER_MSG_UNKNOWN_TOKEN_STR "unknown token: '%.*s' at %d:%d"
#define VMC_COMPILER_MSG_EXPECTED_IDENTIFIER_STR "expected identifier but was: '%.*s' at %d:%d"
#define VMC_COMPILER_MSG_EXPECTED_TYPE_STR "expected type but was: '%.*s' at %d:%d"
#define VMC_COMPILER_MSG_TYPE_NOT_FOUND_STR "type '%.*s' was not found at %d:%d"
#define VMC_COMPILER_MSG_EXPECTED_KEYWORD_STR "expected keyword but was '%.*s' at %d:%d"
#define VMC_COMPILER_MSG_EXPECTED_INDEX_STR "expected index but was '%.*s' at %d:%d"
#define VMC_COMPILER_MSG_EXPECTED_INT_STR "expected integer value but was '%.*s' at %d:%d"

// 
// Functions which helps adding messages to the messages container
//

extern BOOL vmc_compiler_message_unknown_token(vm_messages* m, vmc_lexer* l, vmc_lexer_token* t);
extern BOOL vmc_compiler_message_expected_identifier(vm_messages* m, vmc_lexer* l, vmc_lexer_token* t);
extern BOOL vmc_compiler_message_expected_type(vm_messages* m, vmc_lexer* l, vmc_lexer_token* t);
extern BOOL vmc_compiler_message_type_not_found(vm_messages* m, vmc_lexer* l, vmc_lexer_token* t);
extern BOOL vmc_compiler_message_expected_keyword(vm_messages* m, vmc_lexer* l, vmc_lexer_token* t);
extern BOOL vmc_compiler_message_expected_index(vm_messages* m, vmc_lexer* l, vmc_lexer_token* t);
extern BOOL vmc_compiler_message_expected_int(vm_messages* m, vmc_lexer* l, vmc_lexer_token* t);

#endif

