#ifndef _VM_MESSAGE_H_
#define _VM_MESSAGE_H_

#include "vm_config.h"

// Structure containing the actual message
struct vm_message
{
	// Unique code
	int code;

	// Message text
	char message[4096];

	// The line where the message originates
	int line;

	// The horizontal offset where the message originated
	int line_offset;

	// The offset on in the bytestream where the message originates
	int offset;

	// The next message
	struct vm_message* next;
};
typedef struct vm_message vm_message;

// Set error message and code to body. Will always return FALSE so that it can be part of the
// call structure.
BOOL vm_message_set(vm_message* msg, int error_code, const char* format, ...);

#endif
