#include "vmp_messages.h"
#include "vmp.h"

BOOL vmp_builder_message_unknown_instr(struct vmp_builder* b, vm_int32 instr_type)
{
	vm_messages* const m = &b->messages;
	return vm_messages_add(m,
		VMP_MESSAGE_PREFIX,
		VMP_MESSAGE_UNKNOWN_INSTR,
		VMP_MESSAGE_UNKNOWN_INSTR_STR,
		instr_type);
}

BOOL vmp_builder_message_arg_index_missing(struct vmp_builder* b, vm_int32 index)
{
	vm_messages* const m = &b->messages;
	return vm_messages_add(m,
		VMP_MESSAGE_PREFIX,
		VMP_MESSAGE_ARG_INDEX_MISSING,
		VMP_MESSAGE_ARG_INDEX_MISSING_STR,
		index);
}

BOOL vmp_builder_message_return_index_missing(struct vmp_builder* b, vm_int32 index)
{
	vm_messages* const m = &b->messages;
	return vm_messages_add(m,
		VMP_MESSAGE_PREFIX,
		VMP_MESSAGE_RETURN_INDEX_MISSING,
		VMP_MESSAGE_RETURN_INDEX_MISSING_STR,
		index);
}

BOOL vmp_builder_message_local_index_missing(struct vmp_builder* b, vm_int32 index)
{
	vm_messages* const m = &b->messages;
	return vm_messages_add(m,
		VMP_MESSAGE_PREFIX,
		VMP_MESSAGE_LOCAL_INDEX_MISSING,
		VMP_MESSAGE_LOCAL_INDEX_MISSING_STR,
		index);
}

BOOL vmp_builder_message_header_prediction_failed(struct vmp_builder* b, vm_uint32 expected, vm_uint32 was)
{
	vm_messages* const m = &b->messages;
	return vm_messages_add(m,
		VMP_MESSAGE_PREFIX,
		VMP_MESSAGE_HEADER_PREDICTION_FAILED,
		VMP_MESSAGE_HEADER_PREDICTION_FAILED_STR,
		expected, was);
}

BOOL vmp_builder_message_types_not_compatible(struct vmp_builder* b, const vm_string* from_name, const vm_string* to_name)
{
	vm_messages* const m = &b->messages;
	return vm_messages_add(m,
		VMP_MESSAGE_PREFIX,
		VMP_MESSAGE_TYPES_NOT_COMPATIBLE,
		VMP_MESSAGE_TYPES_NOT_COMPATIBLE_STR,
		vm_string_length(from_name), from_name->start,
		vm_string_length(to_name), to_name->start);
}

BOOL vmp_builder_message_type_too_large(struct vmp_builder* b, const vm_string* type_name, vm_int32 size, vm_int32 max_size)
{
	vm_messages* const m = &b->messages;
	return vm_messages_add(m,
		VMP_MESSAGE_PREFIX,
		VMP_MESSAGE_TYPE_TOO_LARGE,
		VMP_MESSAGE_TYPE_TOO_LARGE_STR,
		vm_string_length(type_name), type_name->start,
		size, max_size);
}

BOOL vmp_builder_message_type_not_defined(struct vmp_builder* b, const vm_string* type_name)
{
	vm_messages* const m = &b->messages;
	return vm_messages_add(m,
		VMP_MESSAGE_PREFIX,
		VMP_MESSAGE_TYPE_NOT_DEFINED,
		VMP_MESSAGE_TYPE_NOT_DEFINED_STR,
		vm_string_length(type_name), type_name->start);
}

BOOL vmp_builder_message_type_not_array(struct vmp_builder* b, const vm_string* type_name)
{
	vm_messages* const m = &b->messages;
	return vm_messages_add(m,
		VMP_MESSAGE_PREFIX,
		VMP_MESSAGE_TYPE_NOT_ARRAY,
		VMP_MESSAGE_TYPE_NOT_ARRAY_STR,
		vm_string_length(type_name), type_name->start);
}

BOOL vmp_builder_message_instr_requires_prev_instr(struct vmp_builder* b, const char* instr_name)
{
	vm_messages* const m = &b->messages;
	return vm_messages_add(m,
		VMP_MESSAGE_PREFIX,
		VMP_MESSAGE_INSTR_REQUIRES_PREV_INSTR,
		VMP_MESSAGE_INSTR_REQUIRES_PREV_INSTR_STR,
		instr_name);
}

BOOL vmp_builder_message_expected_value_on_stack(struct vmp_builder* b, const vm_string* type_name)
{
	vm_messages* const m = &b->messages;
	return vm_messages_add(m,
		VMP_MESSAGE_PREFIX,
		VMP_MESSAGE_EXPECTED_VALUE_ON_STACK,
		VMP_MESSAGE_EXPECTED_VALUE_ON_STACK_STR,
		vm_string_length(type_name), type_name->start);
}

BOOL vmp_builder_message_expected_const_smaller_than(struct vmp_builder* b, vm_int64 value, vm_int64 max_value)
{
	vm_messages* const m = &b->messages;
	return vm_messages_add(m,
		VMP_MESSAGE_PREFIX,
		VMP_MESSAGE_EXPECTED_CONST_SMALLER_THAN,
		VMP_MESSAGE_EXPECTED_CONST_SMALLER_THAN_STR,
		value, max_value);
}

BOOL vmp_builder_message_expected_const_larger_than(struct vmp_builder* b, vm_int64 value, vm_int64 min_value)
{
	vm_messages* const m = &b->messages;
	return vm_messages_add(m,
		VMP_MESSAGE_PREFIX,
		VMP_MESSAGE_EXPECTED_CONST_LARGER_THAN,
		VMP_MESSAGE_EXPECTED_CONST_LARGER_THAN_STR,
		value, min_value);
}

BOOL vmp_builder_message_not_implemented(struct vmp_builder* b, const char* implementation)
{
	vm_messages* const m = &b->messages;
	return vm_messages_add(m,
		VMP_MESSAGE_PREFIX,
		VMP_MESSAGE_NOT_IMPLEMENTED,
		VMP_MESSAGE_NOT_IMPLEMENTED_STR,
		implementation);
}

BOOL vmp_builder_panic_out_of_memory(struct vmp_builder* b)
{
	vm_message* const m = &b->panic_error_message;
	m->prefix = VMP_MESSAGE_PREFIX;
	m->code = VMP_MESSAGE_PANIC;
	m->next = NULL;
	m->line = m->line_offset = m->offset = 0;
	sprintf(m->message, "out of memory");
	return FALSE;
}
