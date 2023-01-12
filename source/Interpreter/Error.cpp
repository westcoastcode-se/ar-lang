#include "Error.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Interpreter;

InterpreterError::InterpreterError()
{
}

char const* InterpreterError::what() const
{
	return _error.c_str();
}

void InterpreterError::SetErrorf(const char* fmt, ...)
{
	_error = fmt;
}

void InterpreterError::SetError(const String& error)
{
	_error = error;
}
