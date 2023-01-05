#include "messages.h"
#include "syntax_tree.h"
#include "../arCompiler.h"

#define arC_MESSAGE_PREFIX 'C'
#define arC_MESSAGE_NOT_IMPLEMENTED_STR "not implemented '%.*s' at %d:%d. %s"
#define arC_MESSAGE_EXPECTED_PACKAGE_STR "expected package but was '%.*s' at %d:%d"
#define arC_MESSAGE_EXPECTED_IDENTIFIER_STR "expected identifier but was '%.*s' at %d:%d"
#define arC_MESSAGE_SYNTAX_ERROR_STR "%s but was '%.*s' at %d:%d"
#define arC_MESSAGE_ALREADY_DEFINED_STR "'%.*s' is already defined at %d:%d"
#define arC_MESSAGE_NOT_DEFINED_STR "'%.*s' is not defined but used at %d:%d"
#define arC_MESSAGE_SYMBOL_UNRESOLVED_STR "'%.*s' is unresolved"
#define arC_MESSAGE_TYPE_UNRESOLVED_STR "'%.*s' is not a type"
#define arC_MESSAGE_TYPE_RECURSIVE_DEPENDENCY_STR "recursive dependency chain detected %s"
#define arC_MESSAGE_TYPE_FEATURE_MISSING_STR "%s"

BOOL arC_message_panic(const arC_state* s, const char* str, arC_message_code code)
{
	arMessage* const m = &s->compiler->panic;
	m->prefix = arC_MESSAGE_PREFIX;
	m->code = code;
	m->next = NULL;
	m->line = m->line_offset = m->offset = 0;
	sprintf(m->message, str);
	return FALSE;
}

BOOL arC_message_out_of_memory(const arC_state* s)
{
	return arC_message_panic(s, "out of memory", arC_MESSAGE_OUT_OF_MEMORY);
}

BOOL arC_message_not_implemented(const arC_state* s, const char* extra)
{
	const arC_token* const t = s->token;
	arMessages* const m = &s->compiler->messages;

	int line, line_offset, offset;
	arC_token_get_metadata(t, &line, &line_offset, &offset);
	arMessage* const message = arMessages_add(m,
		arC_MESSAGE_PREFIX,
		arC_MESSAGE_NOT_IMPLEMENTED,
		arC_MESSAGE_NOT_IMPLEMENTED_STR,
		arString_length(&t->string), t->string.start, line, line_offset, extra);
	message->line = line;
	message->line_offset = line_offset;
	message->offset = offset;
	return FALSE;
}

BOOL arC_message_expected_package(arCompiler* c, struct arC_token* t)
{
	arMessages* const m = &c->messages;

	int line, line_offset, offset;
	arC_token_get_metadata(t, &line, &line_offset, &offset);
	arMessage* const message = arMessages_add(m,
		arC_MESSAGE_PREFIX,
		arC_MESSAGE_EXPECTED_PACKAGE,
		arC_MESSAGE_EXPECTED_PACKAGE_STR,
		arString_length(&t->string), t->string.start, line, line_offset);
	message->line = line;
	message->line_offset = line_offset;
	message->offset = offset;
	return FALSE;
}

BOOL arC_message_expected_identifier(const arC_state* s)
{
	const arC_token* const t = s->token;
	arMessages* const m = &s->compiler->messages;

	int line, line_offset, offset;
	arC_token_get_metadata(t, &line, &line_offset, &offset);
	arMessage* const message = arMessages_add(m,
		arC_MESSAGE_PREFIX,
		arC_MESSAGE_EXPECTED_IDENTIFIER,
		arC_MESSAGE_EXPECTED_IDENTIFIER_STR,
		arString_length(&t->string), t->string.start, line, line_offset);
	message->line = line;
	message->line_offset = line_offset;
	message->offset = offset;
	return FALSE;
}

BOOL arC_message_syntax_error(const arC_state* s, const char* msg)
{
	const arC_token* const t = s->token;
	arMessages* const m = &s->compiler->messages;

	int line, line_offset, offset;
	arC_token_get_metadata(t, &line, &line_offset, &offset);
	arMessage* const message = arMessages_add(m,
		arC_MESSAGE_PREFIX,
		arC_MESSAGE_SYNTAX_ERROR,
		arC_MESSAGE_SYNTAX_ERROR_STR,
		msg, arString_length(&t->string), t->string.start, line, line_offset);
	message->line = line;
	message->line_offset = line_offset;
	message->offset = offset;
	return FALSE;
}

ARLANG_API BOOL arC_message_already_defined(const struct arC_state* s, const arString* name)
{
	const arC_token* const t = s->token;
	arMessages* const m = &s->compiler->messages;

	int line, line_offset, offset;
	arC_token_get_metadata(t, &line, &line_offset, &offset);
	arMessage* const message = arMessages_add(m,
		arC_MESSAGE_PREFIX,
		arC_MESSAGE_ALREADY_DEFINED,
		arC_MESSAGE_ALREADY_DEFINED_STR,
		arString_length(name), name->start, line, line_offset);
	message->line = line;
	message->line_offset = line_offset;
	message->offset = offset;
	return FALSE;
}

BOOL arC_message_not_defined(const arC_state* s, const arString* name)
{
	const arC_token* const t = s->token;
	arMessages* const m = &s->compiler->messages;

	int line, line_offset, offset;
	arC_token_get_metadata(t, &line, &line_offset, &offset);
	arMessage* const message = arMessages_add(m,
		arC_MESSAGE_PREFIX,
		arC_MESSAGE_NOT_DEFINED,
		arC_MESSAGE_NOT_DEFINED_STR,
		arString_length(name), name->start, line, line_offset);
	message->line = line;
	message->line_offset = line_offset;
	message->offset = offset;
	return FALSE;
}

BOOL arC_message_symbol_unresolved(const struct arC_state* s, const arString* name)
{
	arMessages* const m = &s->compiler->messages;

	arMessage* const message = arMessages_add(m,
		arC_MESSAGE_PREFIX,
		arC_MESSAGE_SYMBOL_UNRESOLVED,
		arC_MESSAGE_SYMBOL_UNRESOLVED_STR,
		arString_length(name), name->start);
	message->line = 0;
	message->line_offset = 0;
	message->offset = 0;
	return FALSE;
}

BOOL arC_message_type_unresolved(const struct arC_state* s, const arString* name)
{
	arMessages* const m = &s->compiler->messages;

	arMessage* const message = arMessages_add(m,
		arC_MESSAGE_PREFIX,
		arC_MESSAGE_TYPE_UNRESOLVED,
		arC_MESSAGE_TYPE_UNRESOLVED_STR,
		arString_length(name), name->start);
	message->line = 0;
	message->line_offset = 0;
	message->offset = 0;
	return FALSE;
}

BOOL arC_message_type_recursive_dependency(const struct arC_state* s, const char* relationships)
{
	arMessages* const m = &s->compiler->messages;

	arMessage* const message = arMessages_add(m,
		arC_MESSAGE_PREFIX,
		arC_MESSAGE_TYPE_RECURSIVE_DEPENDENCY,
		arC_MESSAGE_TYPE_RECURSIVE_DEPENDENCY_STR,
		relationships);
	message->line = 0;
	message->line_offset = 0;
	message->offset = 0;
	return FALSE;
}

BOOL arC_message_feature_missing(const struct arC_state* s, const char* relationships)
{
	arMessages* const m = &s->compiler->messages;

	arMessage* const message = arMessages_add(m,
		arC_MESSAGE_PREFIX,
		arC_MESSAGE_TYPE_FEATURE_MISSING,
		arC_MESSAGE_TYPE_FEATURE_MISSING_STR,
		relationships);
	message->line = 0;
	message->line_offset = 0;
	message->offset = 0;
	return FALSE;
}
