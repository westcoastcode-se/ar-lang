#ifndef _ARC_MESSAGE_CODES_H_
#define _ARC_MESSAGE_CODES_H_

typedef enum arC_message_code
{
	arC_MESSAGE_NONE = 0,

	// Messages of level 0 (critical) is below this 
	arC_MESSAGE_LEVEL0,

	// The device is out of memory
	arC_MESSAGE_OUT_OF_MEMORY,

	// This error code is raised if the source code contains one or many un-implemented features
	arC_MESSAGE_NOT_IMPLEMENTED,

	// Message raised when we expected a package, but was something else. All documents have 
	// to have a defined package at the start of each document
	arC_MESSAGE_EXPECTED_PACKAGE,

	// Compiler expects an identifier
	arC_MESSAGE_EXPECTED_IDENTIFIER,

	// Generic syntax error
	arC_MESSAGE_SYNTAX_ERROR,

	// A type or function is already defined
	arC_MESSAGE_ALREADY_DEFINED,

	// A symbol is unresolved
	arC_MESSAGE_SYMBOL_UNRESOLVED
} arC_message_code;

#endif

