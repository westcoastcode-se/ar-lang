#include "messages.h"
#include "message_codes.h"
#include "../arBuilder.h"

// Prefix for all pipeline messages starts with a 'P'
#define VMP_MESSAGE_PREFIX 'P'

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

BOOL arB_message_unknown_instr(struct arBuilder* b, arInt32 instr_type)
{
	arMessages* const m = &b->messages;
	arMessage* const message = arMessages_add(m,
		VMP_MESSAGE_PREFIX,
		VMP_MESSAGE_UNKNOWN_INSTR,
		VMP_MESSAGE_UNKNOWN_INSTR_STR,
		instr_type);
	return FALSE;
}

BOOL arB_message_arg_index_missing(struct arBuilder* b, arInt32 index)
{
	arMessages* const m = &b->messages;
	arMessage* const message = arMessages_add(m,
		VMP_MESSAGE_PREFIX,
		VMP_MESSAGE_ARG_INDEX_MISSING,
		VMP_MESSAGE_ARG_INDEX_MISSING_STR,
		index);
	return FALSE;
}

BOOL arB_message_return_index_missing(struct arBuilder* b, arInt32 index)
{
	arMessages* const m = &b->messages;
	arMessage* const message = arMessages_add(m,
		VMP_MESSAGE_PREFIX,
		VMP_MESSAGE_RETURN_INDEX_MISSING,
		VMP_MESSAGE_RETURN_INDEX_MISSING_STR,
		index);
	return FALSE;
}

BOOL arB_message_local_index_missing(struct arBuilder* b, arInt32 index)
{
	arMessages* const m = &b->messages;
	arMessage* const message = arMessages_add(m,
		VMP_MESSAGE_PREFIX,
		VMP_MESSAGE_LOCAL_INDEX_MISSING,
		VMP_MESSAGE_LOCAL_INDEX_MISSING_STR,
		index);
	return FALSE;
}

BOOL arB_message_header_prediction_failed(struct arBuilder* b, arUint32 expected, arUint32 was)
{
	arMessages* const m = &b->messages;
	arMessage* const message = arMessages_add(m,
		VMP_MESSAGE_PREFIX,
		VMP_MESSAGE_HEADER_PREDICTION_FAILED,
		VMP_MESSAGE_HEADER_PREDICTION_FAILED_STR,
		expected, was);
	return FALSE;
}

BOOL arB_message_types_not_compatible(struct arBuilder* b, const arString* from_name, const arString* to_name)
{
	arMessages* const m = &b->messages;
	arMessage* const message = arMessages_add(m,
		VMP_MESSAGE_PREFIX,
		VMP_MESSAGE_TYPES_NOT_COMPATIBLE,
		VMP_MESSAGE_TYPES_NOT_COMPATIBLE_STR,
		arString_length(from_name), from_name->start,
		arString_length(to_name), to_name->start);
	return FALSE;
}

BOOL arB_message_type_too_large(struct arBuilder* b, const arString* type_name, arInt32 size, arInt32 max_size)
{
	arMessages* const m = &b->messages;
	arMessage* const message = arMessages_add(m,
		VMP_MESSAGE_PREFIX,
		VMP_MESSAGE_TYPE_TOO_LARGE,
		VMP_MESSAGE_TYPE_TOO_LARGE_STR,
		arString_length(type_name), type_name->start,
		size, max_size);
	return FALSE;
}

BOOL arB_message_type_not_defined(struct arBuilder* b, const arString* type_name)
{
	arMessages* const m = &b->messages;
	arMessage* const message = arMessages_add(m,
		VMP_MESSAGE_PREFIX,
		VMP_MESSAGE_TYPE_NOT_DEFINED,
		VMP_MESSAGE_TYPE_NOT_DEFINED_STR,
		arString_length(type_name), type_name->start);
	return FALSE;
}

BOOL arB_message_type_not_array(struct arBuilder* b, const arString* type_name)
{
	arMessages* const m = &b->messages;
	arMessage* const message = arMessages_add(m,
		VMP_MESSAGE_PREFIX,
		VMP_MESSAGE_TYPE_NOT_ARRAY,
		VMP_MESSAGE_TYPE_NOT_ARRAY_STR,
		arString_length(type_name), type_name->start);
	return FALSE;
}

BOOL arB_message_instr_requires_prev_instr(struct arBuilder* b, const char* instr_name)
{
	arMessages* const m = &b->messages;
	arMessage* const message = arMessages_add(m,
		VMP_MESSAGE_PREFIX,
		VMP_MESSAGE_INSTR_REQUIRES_PREV_INSTR,
		VMP_MESSAGE_INSTR_REQUIRES_PREV_INSTR_STR,
		instr_name);
	return FALSE;
}

BOOL arB_message_expected_value_on_stack(struct arBuilder* b, const arString* type_name)
{
	arMessages* const m = &b->messages;
	arMessage* const message = arMessages_add(m,
		VMP_MESSAGE_PREFIX,
		VMP_MESSAGE_EXPECTED_VALUE_ON_STACK,
		VMP_MESSAGE_EXPECTED_VALUE_ON_STACK_STR,
		arString_length(type_name), type_name->start);
	return FALSE;
}

BOOL arB_message_expected_const_smaller_than(struct arBuilder* b, arInt64 value, arInt64 max_value)
{
	arMessages* const m = &b->messages;
	arMessage* const message = arMessages_add(m,
		VMP_MESSAGE_PREFIX,
		VMP_MESSAGE_EXPECTED_CONST_SMALLER_THAN,
		VMP_MESSAGE_EXPECTED_CONST_SMALLER_THAN_STR,
		value, max_value);
	return FALSE;
}

BOOL arB_message_expected_const_larger_than(struct arBuilder* b, arInt64 value, arInt64 min_value)
{
	arMessages* const m = &b->messages;
	arMessage* const message = arMessages_add(m,
		VMP_MESSAGE_PREFIX,
		VMP_MESSAGE_EXPECTED_CONST_LARGER_THAN,
		VMP_MESSAGE_EXPECTED_CONST_LARGER_THAN_STR,
		value, min_value);
	return FALSE;
}

BOOL arB_message_not_implemented(struct arBuilder* b, const char* implementation)
{
	arMessages* const m = &b->messages;
	arMessage* const message = arMessages_add(m,
		VMP_MESSAGE_PREFIX,
		VMP_MESSAGE_NOT_IMPLEMENTED,
		VMP_MESSAGE_NOT_IMPLEMENTED_STR,
		implementation);
	return FALSE;
}

BOOL arB_panic_out_of_memory(struct arBuilder* b)
{
	arMessage* const m = &b->panic_error_message;
	m->prefix = VMP_MESSAGE_PREFIX;
	m->code = VMP_MESSAGE_PANIC;
	m->next = NULL;
	m->line = m->line_offset = m->offset = 0;
	sprintf(m->message, "out of memory");
	return FALSE;
}
