#ifndef _VM_MESSAGE_H_
#define _VM_MESSAGE_H_

#include "vm_config.h"

// Structure containing the actual message
struct vm_message
{
	// Code prefix - used to represent which system this messages is raised from
	char prefix;

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

// All messages
struct vm_messages
{
	vm_message* first;
	vm_message* last;
};
typedef struct vm_messages vm_messages;

// Set error message and code to body. Will always return FALSE so that it can be part of the
// call structure.
extern BOOL vm_message_set(vm_message* msg, char prefix, int error_code, const char* format, ...);

// Initialize the supplied messages object
extern void vm_messages_init(vm_messages* m);

// Destroy the internals of the supplied messages object
extern void vm_messages_destroy(vm_messages* m);

// Check to see if the there exists at least one message
inline static BOOL vm_messages_has_messages(vm_messages* m)
{
	return m->first == NULL;
}

// Move all messages to a new destination
extern void vm_messages_move(vm_messages* src, vm_messages* dest);

// Add a new messages. Returns TRUE if the messages was added successfully
extern BOOL vm_messages_add(vm_messages* m, char prefix, int error_code, const char* format, ...);

#endif
