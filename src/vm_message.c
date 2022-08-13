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

void vm_message_init(vm_message* m)
{
	m->code = 0;
	m->message[0] = 0;
	m->next = NULL;
}

void vm_messages_destroy(vm_messages* m)
{
	vm_message* message = m->first;
	while (message != NULL) {
		vm_message* const next = message->next;
		free(message);
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
	char* buffer;
	va_list argptr;
	vm_message* new_message = (vm_message*)malloc(sizeof(vm_message));
	if (new_message == NULL)
		return FALSE;
	vm_message_init(new_message);

	buffer = new_message->message;
	buffer += sprintf(buffer, "%c%.3d: ", prefix, error_code);

	new_message->prefix = prefix;
	new_message->code = error_code;
	new_message->line = new_message->line_offset = new_message->offset = 0;

	va_start(argptr, format);
	vsprintf(buffer, format, argptr);
	va_end(argptr);

	vm_messages_append(m, new_message);
	return FALSE;
}

void vm_messages_append(vm_messages* m, vm_message* msg)
{
	if (m->last == NULL)
		m->last = m->first = msg;
	else {
		m->last->next = msg;
		m->last = msg;
	}
}
