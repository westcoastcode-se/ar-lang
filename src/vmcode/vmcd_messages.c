#include "vmcd_messages.h"
#include "vmcode.h"

BOOL vmcd_message_panic(const struct vmcd_scope* s, const char* str)
{
	vm_message* const m = &s->vmcd->panic_error_message;
	m->prefix = VMCD_MESSAGE_PREFIX;
	m->code = VMCD_MESSAGE_PANIC;
	m->next = NULL;
	m->line = m->line_offset = m->offset = 0;
	sprintf(m->message, str);
	return FALSE;
}

BOOL vmcd_message_out_of_memory(const struct vmcd_scope* s)
{
	return vmcd_message_panic(s, "out of memory");
}

BOOL vmcd_message_unknown_token(const struct vmcd_scope* s)
{
	const vmcd_token* const t = s->token;
	vm_messages* const m = &s->vmcd->messages;

	int line, line_offset, _;
	vmcd_token_get_metadata(t, &line, &line_offset, &_);
	return vm_messages_add(m,
		VMCD_MESSAGE_PREFIX,
		VMC_MESSAGE_UNKNOWN_TOKEN,
		VMC_MESSAGE_UNKNOWN_TOKEN_STR,
		vm_string_length(&t->string), t->string.start, line, line_offset);
}

BOOL vmcd_message_package_not_found(const struct vmcd_scope* s)
{
	const vmcd_token* const t = s->token;
	vm_messages* const m = &s->vmcd->messages;

	int line, line_offset, _;
	vmcd_token_get_metadata(t, &line, &line_offset, &_);
	return vm_messages_add(m,
		VMCD_MESSAGE_PREFIX,
		VMC_MESSAGE_PACKAGE_NOT_FOUND,
		VMC_MESSAGE_PACKAGE_NOT_FOUND_STR,
		vm_string_length(&t->string), t->string.start, line, line_offset);
}

BOOL vmcd_message_local_not_found(const struct vmcd_scope* s, const vm_string* local)
{
	const vmcd_token* const t = s->token;
	vm_messages* const m = &s->vmcd->messages;

	int line, line_offset, _;
	vmcd_token_get_metadata(t, &line, &line_offset, &_);
	return vm_messages_add(m,
		VMCD_MESSAGE_PREFIX,
		VMC_MESSAGE_LOCAL_NOT_FOUND,
		VMC_MESSAGE_LOCAL_NOT_FOUND_STR,
		vm_string_length(local), local->start, line, line_offset);
}

BOOL vmcd_message_type_not_found(const struct vmcd_scope* s, const vm_string* type)
{
	const vmcd_token* const t = s->token;
	vm_messages* const m = &s->vmcd->messages;

	int line, line_offset, _;
	vmcd_token_get_metadata(t, &line, &line_offset, &_);
	return vm_messages_add(m,
		VMCD_MESSAGE_PREFIX,
		VMC_MESSAGE_TYPE_NOT_FOUND,
		VMC_MESSAGE_TYPE_NOT_FOUND_STR,
		vm_string_length(type), type->start, line, line_offset);
}

BOOL vmcd_message_expected_identifier(const struct vmcd_scope* s)
{
	const vmcd_token* const t = s->token;
	vm_messages* const m = &s->vmcd->messages;

	int line, line_offset, _; 
	vmcd_token_get_metadata(t, &line, &line_offset, &_);
	return vm_messages_add(m,
		VMCD_MESSAGE_PREFIX,
		VMCD_MESSAGE_EXPECTED_IDENTIFIER,
		VMCD_MESSAGE_EXPECTED_IDENTIFIER_STR,
		vm_string_length(&t->string), t->string.start, line, line_offset);
}

BOOL vmcd_message_syntax_error(const struct vmcd_scope* s, char c)
{
	const vmcd_token* const t = s->token;
	vm_messages* const m = &s->vmcd->messages;

	int line, line_offset, _;
	vmcd_token_get_metadata(t, &line, &line_offset, &_);
	return vm_messages_add(m,
		VMCD_MESSAGE_PREFIX,
		VMCD_MESSAGE_EXPECTED_TOKEN,
		VMCD_MESSAGE_EXPECTED_TOKEN_STR,
		c, vm_string_length(&t->string), t->string.start, line, line_offset);
}

BOOL vmcd_message_not_implemented(const struct vmcd_scope* s)
{
	const vmcd_token* const t = s->token;
	vm_messages* const m = &s->vmcd->messages;

	int line, line_offset, _;
	vmcd_token_get_metadata(t, &line, &line_offset, &_);
	return vm_messages_add(m,
		VMCD_MESSAGE_PREFIX,
		VMCD_MESSAGE_NOT_IMPLEMENTED,
		VMCD_MESSAGE_NOT_IMPLEMENTED_STR,
		vm_string_length(&t->string), t->string.start, line, line_offset);
}

BOOL vmcd_message_duplicated_declaration(const struct vmcd_scope* s, const vm_string* decl)
{
	const vmcd_token* const t = s->token;
	vm_messages* const m = &s->vmcd->messages;

	int line, line_offset, _;
	vmcd_token_get_metadata(t, &line, &line_offset, &_);
	return vm_messages_add(m,
		VMCD_MESSAGE_PREFIX,
		VMCD_MESSAGE_DUPLICATED_DECLARATION,
		VMCD_MESSAGE_DUPLICATED_DECLARATION_STR,
		vm_string_length(decl), decl->start, line, line_offset);
}