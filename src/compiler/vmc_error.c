#include "vmc_error.h"
#include <stdarg.h>

BOOL vmc_error_set(vmc_error* err, int error_code, const char* format, ...)
{
	err->code = error_code;
	va_list argptr;
	va_start(argptr, format);
	vsprintf(err->message, format, argptr);
	va_end(argptr);
	return FALSE;
}
