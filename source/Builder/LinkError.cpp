#include "LinkError.h"
using namespace WestCoastCode::Builder;

char const* LinkError::what() const
{
	return _error.c_str();
}

void LinkError::SetErrorf(const char* fmt, ...)
{
	_error = fmt;
}

void LinkError::SetError(const String& error)
{
	_error = error;
}
