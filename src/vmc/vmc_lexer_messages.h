#ifndef _VMC_LEXER_MESSAGES_H_
#define _VMC_LEXER_MESSAGES_H_

#include "../vm_message.h"
#include "vmc_lexer.h"

// Prefix for all lexer 'T'oken messages
#define VMC_LEXER_MESSAGE_PREFIX 'T'

// All messages
enum vmc_lexer_messages
{
	// Messages that's a level 0 message are considered errors
	VMC_LEXER_MSG_LEVEL0 = 0,

	// A multi-line comment is not closed
	VMC_LEXER_MSG_UNCLOSED_COMMENT,

	// A string is not closed
	VMC_LEXER_MSG_UNCLOSED_STRING,

	// Level 1 non-fatal error messages after this
	VMC_LEXER_MSG_LEVEL1,

	// An unknown escaped character was found
	VMC_LEXER_MSG_UNKNOWN_ESCAPED_CHAR
};

#define VMC_LEXER_MSG_UNCLOSED_COMMENT_STR "unclosed multi-line comment at %d:%d"
#define VMC_LEXER_MSG_UNCLOSED_STRING_STR "unclosed string at %d:%d"
#define VMC_LEXER_MSG_UNKNOWN_ESCAPED_CHAR_STR "unrecognized escaped character: %c at %d:%d"

// 
// Functions which helps adding messages to the messages container
//

extern BOOL vmc_lexer_message_unclosed_comment(vmc_lexer* l);
extern BOOL vmc_lexer_message_unclosed_string(vmc_lexer* l);
extern BOOL vmc_lexer_message_unknown_escaped_char(vmc_lexer* l, vm_byte rune);

#endif
