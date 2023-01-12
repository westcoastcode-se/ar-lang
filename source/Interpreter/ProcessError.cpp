#include "ProcessError.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Interpreter;

void ProcessError::SetErrorf(const char* fmt, ...)
{
	_error = fmt;
}

void ProcessError::SetError(const String& error)
{
	_error = error;
}

ProcessErrorFailedToLoad::ProcessErrorFailedToLoad()
	: ProcessError(ProcessErrorType::FailedToLoad)
{
	StringStream s;
	s << "failed to load bytecode";
	SetError(s.str());
}

ProcessErrorIncompatibleVersion::ProcessErrorIncompatibleVersion(I32 version)
	: ProcessError(ProcessErrorType::IncompatibleVersion)
{
	StringStream s;
	s << "incompatible version: " << version;
	SetError(s.str());
}

ProcessErrorUnsupported::ProcessErrorUnsupported(const char* message)
	: ProcessError(ProcessErrorType::Unsupported)
{
	StringStream s;
	s << "unsupported: '" << message << "'";
	SetError(s.str());
}
