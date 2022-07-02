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

	// PANIC!
	VMP_MESSAGE_PANIC
};

#define VMP_MESSAGE_UNKNOWN_INSTR_STR "unknown instruction %d"
#define VMP_MESSAGE_ARG_INDEX_MISSING_STR "argument index %d is missing"
#define VMP_MESSAGE_RETURN_INDEX_MISSING_STR "return index %d is missing"
#define VMP_MESSAGE_LOCAL_INDEX_MISSING_STR "local index %d is missing"
#define VMP_MESSAGE_HEADER_PREDICTION_FAILED_STR "expected header size to be %d but was %d"
#define VMP_MESSAGE_TYPES_NOT_COMPATIBLE_STR "identifier '%.*s' is not compatible with '%.*s'"

// 
// Functions which helps adding messages to the messages container
//

extern BOOL vmp_builder_message_unknown_instr(struct vmp_builder* b, vm_int32 instr_type);
extern BOOL vmp_builder_message_arg_index_missing(struct vmp_builder* b, vm_int32 index);
extern BOOL vmp_builder_message_return_index_missing(struct vmp_builder* b, vm_int32 index);
extern BOOL vmp_builder_message_local_index_missing(struct vmp_builder* b, vm_int32 index);
extern BOOL vmp_builder_message_header_prediction_failed(struct vmp_builder* b, vm_uint32 expected, vm_uint32 was);
extern BOOL vmp_builder_message_types_not_compatible(struct vmp_builder* b, const vm_string* from_name, const vm_string* to_name);
extern BOOL vmp_builder_panic_out_of_memory(struct vmp_builder* b);

#endif
