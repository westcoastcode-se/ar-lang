#include "Error.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Compilation;

CompilationError::CompilationError()
{
}

char const* CompilationError::what() const
{
	return _error.c_str();
}

void CompilationError::SetErrorf(const char* fmt, ...)
{
	_error = fmt;
}

void CompilationError::SetError(const String& error)
{
	_error = error;
}
