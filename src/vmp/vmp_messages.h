#ifndef _VMC_LEXER_MESSAGES_H_
#define _VMC_LEXER_MESSAGES_H_

#include "../vm_message.h"
#include "../vm_string.h"

// Prefix for all pipeline messages starts with a 'P'
#define VMP_MESSAGE_PREFIX 'P'

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

#define VMP_MESSAGE_UNKNOWN_INSTR_STR "unknown instruction %d"
#define VMP_MESSAGE_ARG_INDEX_MISSING_STR "argument index %d is missing"
#define VMP_MESSAGE_RETURN_INDEX_MISSING_STR "return index %d is missing"
#define VMP_MESSAGE_LOCAL_INDEX_MISSING_STR "local index %d is missing"
#define VMP_MESSAGE_HEADER_PREDICTION_FAILED_STR "expected header size to be %d but was %d"
#define VMP_MESSAGE_TYPES_NOT_COMPATIBLE_STR "identifier '%.*s' is not compatible with '%.*s'"
#define VMP_MESSAGE_TYPE_TOO_LARGE_STR "type '%.*s' is %d bytes but cannot be larger than %d"
#define VMP_MESSAGE_TYPE_NOT_DEFINED_STR "type '%.*s' is not defined"
#define VMP_MESSAGE_TYPE_NOT_ARRAY_STR "type '%.*s' not an array type"
#define VMP_MESSAGE_INSTR_REQUIRES_PREV_INSTR_STR "'%s' requires instructions beforehand"
#define VMP_MESSAGE_EXPECTED_VALUE_ON_STACK_STR "expected a '%.*s' to be on the stack"
#define VMP_MESSAGE_EXPECTED_CONST_SMALLER_THAN_STR "constant %lld must be smaller than %lld"
#define VMP_MESSAGE_EXPECTED_CONST_LARGER_THAN_STR "constant %lld must be larger than %lld"
#define VMP_MESSAGE_NOT_IMPLEMENTED_STR "'%s' is not implemented yet"

// 
// Functions which helps adding messages to the messages container
//

extern BOOL vmp_builder_message_unknown_instr(struct vmp_builder* b, vm_int32 instr_type);
extern BOOL vmp_builder_message_arg_index_missing(struct vmp_builder* b, vm_int32 index);
extern BOOL vmp_builder_message_return_index_missing(struct vmp_builder* b, vm_int32 index);
extern BOOL vmp_builder_message_local_index_missing(struct vmp_builder* b, vm_int32 index);
extern BOOL vmp_builder_message_header_prediction_failed(struct vmp_builder* b, vm_uint32 expected, vm_uint32 was);
extern BOOL vmp_builder_message_types_not_compatible(struct vmp_builder* b, const vm_string* from_name, const vm_string* to_name);
extern BOOL vmp_builder_message_type_too_large(struct vmp_builder* b, const vm_string* type_name, vm_int32 size, vm_int32 max_size);
extern BOOL vmp_builder_message_type_not_defined(struct vmp_builder* b, const vm_string* type_name);
extern BOOL vmp_builder_message_type_not_array(struct vmp_builder* b, const vm_string* type_name);
extern BOOL vmp_builder_message_instr_requires_prev_instr(struct vmp_builder* b, const char* instr_name);
extern BOOL vmp_builder_message_expected_value_on_stack(struct vmp_builder* b, const vm_string* type_name);
extern BOOL vmp_builder_message_expected_const_smaller_than(struct vmp_builder* b, vm_int64 value, vm_int64 max_value);
extern BOOL vmp_builder_message_expected_const_larger_than(struct vmp_builder* b, vm_int64 value, vm_int64 min_value);
extern BOOL vmp_builder_message_not_implemented(struct vmp_builder* b, const char* implementation);
extern BOOL vmp_builder_panic_out_of_memory(struct vmp_builder* b);

#endif
