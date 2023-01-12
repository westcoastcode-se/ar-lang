#pragma once

#include "Error.h"

namespace WestCoastCode::Interpreter
{
	enum class ProcessErrorType : int
	{
		FailedToLoad,
		IncompatibleVersion,
		Unsupported
	};

	// Base class for errors raised by a process
	class ProcessError : public InterpreterError
	{
	public:
		ProcessError(ProcessErrorType type)
			: _type(type){}

	private:
		const ProcessErrorType _type;
	};

	// Invalid bytecode header
	class ProcessErrorFailedToLoad : public ProcessError
	{
	public:
		ProcessErrorFailedToLoad();
	};

	// The supplied bytecode doesn't have a compatible version with
	// the current virtual machine
	class ProcessErrorIncompatibleVersion : public ProcessError
	{
	public:
		ProcessErrorIncompatibleVersion(I32 version);
	};

	// We are trying to do an unsupported feature. Might be implemented in the future
	class ProcessErrorUnsupported : public ProcessError
	{
	public:
		ProcessErrorUnsupported(const char* message);
	};
}
