#pragma once

#include "ThreadError.h"
#include "ThreadStack.h"

namespace WestCoastCode::Interpreter
{
	class Process;
	class Function;

	// A thread running the bytecode
	class Thread
	{
	public:
		Thread(Process* p);

		~Thread();

		const ThreadStack& GetStack() const { return _stack; }

		// Pop the stack with the supplied number of bytes
		Byte* PopStack(I32 size) {
			return _stack.Pop(size);
		}

		// Execute the supplied entrypoint
		void ExecEntrypoint(const Function* entrypoint);

	private:
		Process* const _process;
		
		Byte* const _bytecode;
		ThreadStack _stack;

	};
}
