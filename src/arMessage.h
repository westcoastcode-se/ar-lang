#ifndef _ARLANG_MESSAGE_H_
#define _ARLANG_MESSAGE_H_

#include "arconf.h"

// Structure containing the actual message
typedef struct arMessage
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
	struct arMessage* next;
} arMessage;

// All messages
typedef struct arMessages
{
	arMessage* first;
	arMessage* last;
} arMessages;

// Set error message and code to body. Will always return FALSE so that it can be part of the
// call structure.
ARLANG_API BOOL arMessage_set(arMessage* msg, char prefix, int error_code, const char* format, ...);

// Initialize the supplied messages object
ARLANG_API void arMessages_init(arMessages* m);

// Initialize the supplied message object
ARLANG_API void arMessage_init(arMessage* m);

// Destroy the internals of the supplied messages object
ARLANG_API void arMessages_destroy(arMessages* m);

// Check to see if the there exists at least one message
ARLANG_API BOOL arMessages_has_messages(arMessages* m);

// Move all messages to a new destination
ARLANG_API void arMessages_move(arMessages* src, arMessages* dest);

// Add a new message. Returns the created message if the messages was added successfully
ARLANG_API arMessage* arMessages_add(arMessages* m, char prefix, int error_code, const char* format, ...);

// Append a new message
ARLANG_API void arMessages_append(arMessages* m, arMessage* msg);

#endif
