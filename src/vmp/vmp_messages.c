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

extern BOOL vmp_builder_panic_out_of_memory(struct vmp_builder* b)
{
	vm_message* const m = &b->panic_error_message;
	m->prefix = VMP_MESSAGE_PREFIX;
	m->code = VMP_MESSAGE_PANIC;
	m->next = NULL;
	m->line = m->line_offset = m->offset = 0;
	sprintf(m->message, "out of memory");
	return FALSE;
}
