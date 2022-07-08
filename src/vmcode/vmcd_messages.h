#ifndef _VMCD_MESSAGES_H_
#define _VMCD_MESSAGES_H_

#include "../vm_message.h"
#include "../vm_string.h"

// Prefix for all messages
#define VMCD_MESSAGE_PREFIX 'C'

enum vmcd_messages
{
	// Represents a non-existing message
	VMCD_MESSAGE_NONE = 0,

	// Messages that's a level 0 message are considered errors
	VMCD_MESSAGE_LEVEL0,

	// Critical error
	VMCD_MESSAGE_PANIC,

	// Received a token from the lexer which the compiler has no idea on what to do with
	VMC_MESSAGE_UNKNOWN_TOKEN,

	// Could not find a required package
	VMC_MESSAGE_PACKAGE_NOT_FOUND,

	// Could not find a required local variable
	VMC_MESSAGE_LOCAL_NOT_FOUND,

	// Could not find a type
	VMC_MESSAGE_TYPE_NOT_FOUND,

	// Compiler requires an keyword that represents an identifier
	VMCD_MESSAGE_EXPECTED_IDENTIFIER,

	// Generic syntax error
	VMCD_MESSAGE_EXPECTED_TOKEN,

	// Functionality that's not implemented
	VMCD_MESSAGE_NOT_IMPLEMENTED,

	// A declaration is duplicated (exists more than once)
	VMCD_MESSAGE_DUPLICATED_DECLARATION,

};
typedef enum vmcd_messages vmcd_messages;

#define VMC_MESSAGE_UNKNOWN_TOKEN_STR "unknown token: '%.*s' at %d:%d"
#define VMCD_MESSAGE_EXPECTED_IDENTIFIER_STR "expected identifier but was: '%.*s' at %d:%d"
#define VMC_MESSAGE_PACKAGE_NOT_FOUND_STR "could not find package: '%.*s' at %d:%d"
#define VMC_MESSAGE_LOCAL_NOT_FOUND_STR "could not find local variable: '%.*s' at %d:%d"
#define VMC_MESSAGE_TYPE_NOT_FOUND_STR "could not find local variable: '%.*s' at %d:%d"
#define VMCD_MESSAGE_EXPECTED_TOKEN_STR "syntax error: missing '%c' before '%.*s' at %d:%d"
#define VMCD_MESSAGE_NOT_IMPLEMENTED_STR "not implemented: '%.*s' at %d:%d"
#define VMCD_MESSAGE_DUPLICATED_DECLARATION_STR "'%.*s' is already declared once at %d:%d"

extern BOOL vmcd_message_panic(const struct vmcd_scope* s, const char* str);
extern BOOL vmcd_message_out_of_memory(const struct vmcd_scope* s);
extern BOOL vmcd_message_unknown_token(const struct vmcd_scope* s);
extern BOOL vmcd_message_package_not_found(const struct vmcd_scope* s);
extern BOOL vmcd_message_local_not_found(const struct vmcd_scope* s, const vm_string* local);
extern BOOL vmcd_message_type_not_found(const struct vmcd_scope* s, const vm_string* type);
extern BOOL vmcd_message_expected_identifier(const struct vmcd_scope* s);
extern BOOL vmcd_message_syntax_error(const struct vmcd_scope* s, char c);
extern BOOL vmcd_message_not_implemented(const struct vmcd_scope* s);
extern BOOL vmcd_message_duplicated_declaration(const struct vmcd_scope* s, const vm_string* decl);

#endif