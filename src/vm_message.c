#include "vm_message.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

BOOL vm_message_set(vm_message* msg, char prefix, int error_code, const char* format, ...)
{
	msg->prefix = prefix;
	msg->code = error_code;
	msg->next = NULL;
	msg->line = msg->line_offset = msg->offset = 0;
	va_list argptr;
	va_start(argptr, format);
	vsprintf(msg->message, format, argptr);
	va_end(argptr);
	return FALSE;
}

void vm_messages_init(vm_messages* m)
{
	m->first = m->last = NULL;
}

void vm_messages_destroy(vm_messages* m)
{
	vm_message* message = m->first;
	while (message != NULL) {
		vm_message* const next = message->next;
		free(m);
		message = next;
	}
	m->first = m->last = NULL;
}

void vm_messages_move(vm_messages* src, vm_messages* dest)
{
	vm_message* first;
	vm_message* last;
	if (!vm_messages_has_messages(src))
		return;

	first = src->first;
	last = src->last;
	src->first = src->last = NULL;

	if (dest->last == NULL) {
		dest->first = first;
		dest->last = last;
	}
	else {
		dest->last->next = first;
		dest->last = last;
	}
}

BOOL vm_messages_add(vm_messages* m, char prefix, int error_code, const char* format, ...)
{
	va_list argptr;
	vm_message* new_message = (vm_message*)malloc(sizeof(vm_message));
	if (new_message == NULL)
		return FALSE;

	new_message->prefix = prefix;
	new_message->code = error_code;
	new_message->next = NULL;
	new_message->line = new_message->line_offset = new_message->offset = 0;

	va_start(argptr, format);
	vsprintf(new_message->message, format, argptr);
	va_end(argptr);

	if (m->last == NULL)
		m->last = m->first = new_message;
	else {
		m->last->next = new_message;
		m->last = new_message;
	}
	return FALSE;
}
