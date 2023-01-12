#pragma once

#include "Error.h"

namespace WestCoastCode::Interpreter
{
	enum class ThreadErrorType : int
	{
		StackMismanaged,
		HaltedExecution,
		ExecutionOutsideMemory,
	};

	// Base class for errors raised by a process
	class ThreadError : public InterpreterError
	{
	public:
		ThreadError(ThreadErrorType type)
			: _type(type) {}

	private:
		const ThreadErrorType _type;
	};

	// Error raised when the stack is mismanaged
	class ThreadErrorStackMismanaged : public ThreadError
	{
	public:
		ThreadErrorStackMismanaged(I32 expected, I32 current_size);
	};

	// Error raised when the stack is mismanaged
	class ThreadErrorHaltedExecution : public ThreadError
	{
	public:
		ThreadErrorHaltedExecution(I32 flags, const Char* message);
	};

	// Error raised the thread tries to execute bytecode outside executable
	// memory
	class ThreadErrorExecutionOutsideMemory : public ThreadError
	{
	public:
		ThreadErrorExecutionOutsideMemory();
	};
}
