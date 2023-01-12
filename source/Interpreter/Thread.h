#pragma once

#include "ThreadError.h"
#include "ThreadStack.h"

namespace WestCoastCode::Interpreter
{
	class Process;
	class Function;

	enum class ThreadFlag
	{
		UnknownInstruction = 1 << 0,
		OutOfMemory = 1 << 1,
		InvalidLocal = 1 << 2,
		StackMismanaged = 1 << 3,
		LocalsMismanaged = 1 << 4,
		NotImplemented = 1 << 5,
		ManualHalt = 1 << 6,
		OutsideExecutionMemory = 1 << 7
	};
	typedef I32 ThreadFlags;

	// Call frame
	struct ThreadCallFrame
	{
		// Where arguments can be found
		Byte* ebp;

		// The next instruction to be called when returning
		const Byte* ret;
	};

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

		// Execute instructions
		void Exec(const Byte* ip);

		// Execute instructions
		void Exec0(const Byte* ip) noexcept;

	private:
		Process* const _process;		
		Byte* const _bytecode;
		ThreadStack _stack;
		const Range<const Byte*> _executionRange;

		const Byte* _ip;

		// Available callframes
		ThreadCallFrame _callFrames[VM_RECURSIVE_FN_CALLFRAMES];

		// The current call frame
		ThreadCallFrame* _cf;

		// Error management
		ThreadFlags _flags;
		Char _halt_message[4096];
	};
}
