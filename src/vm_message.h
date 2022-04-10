#ifndef _VM_MESSAGE_H_
#define _VM_MESSAGE_H_

#include "vm_config.h"

// Structure containing the actual message
struct vm_message
{
	int code;
	char message[4096];
	
	// The next message
	struct vm_message* next;
};
typedef struct vm_message vm_message;

// Set error message and code to body. Will always return FALSE so that it can be part of the
// call structure.
BOOL vm_message_set(vm_message* msg, int error_code, const char* format, ...);

#endif
