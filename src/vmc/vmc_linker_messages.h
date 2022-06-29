#ifndef _VMC_LINKER_MESSAGES_H_
#define _VMC_LINKER_MESSAGES_H_

#include "../vm_message.h"
#include "vmc_lexer.h"

// Prefix for all compiler messages
#define VMC_LINKER_MESSAGE_PREFIX 'L'

// All messages
enum vmc_linker_messages
{
	// Represents a non-existing message
	VMC_LINKER_MSG_NONE = 0,

	// An address could not be resolved. Normally happens if you refer to a function that doesn't exist
	VMC_LINKER_MSG_UNRESOLVED_ADDR,
};

#define VMC_LINKER_MSG_UNRESOLVED_ADDR_STR "could not resolve '%.*s'"

// 
// Functions which helps adding messages to the messages container
//

extern BOOL vmc_linker_message_unresolved_addr(vm_messages* m, const vm_string* signature);

#endif
