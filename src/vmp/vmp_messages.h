#ifndef _VMC_LEXER_MESSAGES_H_
#define _VMC_LEXER_MESSAGES_H_

#include "../vm_message.h"

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

	// PANIC!
	VMP_MESSAGE_PANIC,
};

#define VMP_MESSAGE_UNKNOWN_INSTR_STR "unknown instruction %d"
#define VMP_MESSAGE_ARG_INDEX_MISSING_STR "argument index %d is missing"
#define VMP_MESSAGE_RETURN_INDEX_MISSING_STR "return index %d is missing"
#define VMP_MESSAGE_LOCAL_INDEX_MISSING_STR "local index %d is missing"

// 
// Functions which helps adding messages to the messages container
//

extern BOOL vmp_builder_message_unknown_instr(struct vmp_builder* b, vm_int32 instr_type);
extern BOOL vmp_builder_message_arg_index_missing(struct vmp_builder* b, vm_int32 index);
extern BOOL vmp_builder_message_return_index_missing(struct vmp_builder* b, vm_int32 index);
extern BOOL vmp_builder_message_local_index_missing(struct vmp_builder* b, vm_int32 index);
extern BOOL vmp_builder_panic_out_of_memory(struct vmp_builder* b);

#endif
