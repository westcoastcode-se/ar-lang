#include "vm_message.h"
#include <stdarg.h>

BOOL vm_message_set(vm_message* msg, int error_code, const char* format, ...)
{
	msg->code = error_code;
	va_list argptr;
	va_start(argptr, format);
	vsprintf(msg->message, format, argptr);
	va_end(argptr);
	return FALSE;
}
