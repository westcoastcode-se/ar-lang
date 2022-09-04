#ifndef _ARB_MESSAGE_CODES_H_
#define _ARB_MESSAGE_CODES_H_

// All messages
enum vmc_lexer_messages
{
	// Represents a non-existing message
	VMP_MESSAGE_NONE = 0,

	// Messages that's a level 0 message are considered errors
	VMP_MESSAGE_LEVEL0,

	// An unknown instruction is reached
	VMP_MESSAGE_UNKNOWN_INSTR,

	// A return index is missing
	VMP_MESSAGE_ARG_INDEX_MISSING,

	// A return index is missing
	VMP_MESSAGE_RETURN_INDEX_MISSING,

	// A local index is missing
	VMP_MESSAGE_LOCAL_INDEX_MISSING,

	// Failed to predict the header size
	VMP_MESSAGE_HEADER_PREDICTION_FAILED,

	// Two types are not directly compatible
	VMP_MESSAGE_TYPES_NOT_COMPATIBLE,

	// A type is too large
	VMP_MESSAGE_TYPE_TOO_LARGE,

	// A type is not defined (has an unknown size)
	VMP_MESSAGE_TYPE_NOT_DEFINED,

	// Message that's raised if a type is not an array type
	VMP_MESSAGE_TYPE_NOT_ARRAY,

	// An instruction requires a specific previous instruction to work
	VMP_MESSAGE_INSTR_REQUIRES_PREV_INSTR,

	// An instruction requires a specific value to be on the stack
	VMP_MESSAGE_EXPECTED_VALUE_ON_STACK,

	// Verify that a constant must be smaller than another value
	VMP_MESSAGE_EXPECTED_CONST_SMALLER_THAN,

	// Verify that a constant must be larger than another value
	VMP_MESSAGE_EXPECTED_CONST_LARGER_THAN,

	// A generic "not implemented" message. Used for features that will be implemented, but are not yet done so
	VMP_MESSAGE_NOT_IMPLEMENTED,

	// PANIC!
	VMP_MESSAGE_PANIC
};

#endif
