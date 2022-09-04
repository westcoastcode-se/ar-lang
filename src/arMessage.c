#include "arMessage.h"
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

BOOL arMessage_set(arMessage* msg, char prefix, int error_code, const char* format, ...)
{
	arMessage_init(msg);

	msg->prefix = prefix;
	msg->code = error_code;
	msg->line = msg->line_offset = msg->offset = 0;

	char* buffer = msg->message;
	buffer += sprintf(buffer, "%c%.3d: ", prefix, error_code);

	va_list argptr;
	va_start(argptr, format);
	vsprintf(buffer, format, argptr);
	va_end(argptr);

	return TRUE;
}

void arMessages_init(arMessages* m)
{
	m->first = m->last = NULL;
}

void arMessage_init(arMessage* m)
{
	m->code = 0;
	m->message[0] = 0;
	m->next = NULL;
}

void arMessages_destroy(arMessages* m)
{
	arMessage* message = m->first;
	while (message != NULL) {
		arMessage* const next = message->next;
		free(message);
		message = next;
	}
	m->first = m->last = NULL;

}

BOOL arMessages_has_messages(arMessages* m)
{
	return m->first != NULL;
}

void arMessages_move(arMessages* src, arMessages* dest)
{
	arMessage* first;
	arMessage* last;
	if (!arMessages_has_messages(src))
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

arMessage* arMessages_add(arMessages* m, char prefix, int error_code, const char* format, ...)
{
	char* buffer;
	va_list argptr;
	arMessage* new_message = (arMessage*)malloc(sizeof(arMessage));
	if (new_message == NULL)
		return NULL;
	arMessage_init(new_message);

	buffer = new_message->message;
	buffer += sprintf(buffer, "%c%.3d: ", prefix, error_code);

	new_message->prefix = prefix;
	new_message->code = error_code;
	new_message->line = new_message->line_offset = new_message->offset = 0;

	va_start(argptr, format);
	vsprintf(buffer, format, argptr);
	va_end(argptr);

	arMessages_append(m, new_message);
	return new_message;
}

void arMessages_append(arMessages* m, arMessage* msg)
{
	if (m->last == NULL)
		m->last = m->first = msg;
	else {
		m->last->next = msg;
		m->last = msg;
	}
}
