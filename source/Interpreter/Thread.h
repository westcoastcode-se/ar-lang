#pragma once

#include "ThreadError.h"
#include "ThreadStack.h"

namespace WestCoastCode::Interpreter
{
	class Process;
	class Function;

	/// @brief Helper struct managing range validation
	struct Range
	{
		const Byte* start;
		const Byte* end;

		Range(const Byte* start, const Byte* end) : start(start), end(end) {}
		Range(const Range& rhs) : start(rhs.start), end(rhs.end) {}

		/// @brief Check to see if the supplied value is inside range
		/// @param val 
		/// @return true if the supplied address is within the memory range
		bool Inside(const Byte* val) const noexcept {
			return (size_t)val >= (size_t)start &&
				(size_t)val <= (size_t)end;
		}
	};

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
	struct ThreadStackFrame
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

		ThreadStack& GetStack() { return _stack; }

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

		// Get the current callframe
		const ThreadStackFrame* GetCallFrame() const noexcept { return _sf; }

		// Get the current callframe
		ThreadStackFrame* GetCallFrame() noexcept { return _sf; }

		// Halt using the supplied message
		const Byte* Halt(const Byte* address, ThreadFlags flags, const char* message) noexcept;

		// Halt using the supplied message
		const Byte* Halt(const Byte* address, ThreadFlag flag, const char* message) noexcept;

		// Halt using the supplied message
		const Byte* Haltf(const Byte* address, ThreadFlags flags, const char* format, ...) noexcept;

		// Halt using the supplied message
		const Byte* Haltf(const Byte* address, ThreadFlag flag, const char* format, ...) noexcept;

		// Return to the one calling a function
		const Byte* ReturnToCaller() noexcept;

	private:
		/// @brief 
		/// @param ip 
		/// @return 
		const Byte* Call(const Byte* ip) noexcept;

		/// @brief Prepare a call statement
		/// @param ip 
		/// @param expectedStackSize 
		/// @return 
		const Byte* BeginCall(const Byte* ip, const Byte* returnIp, const Byte* nextIp, I32 expectedStackSize) noexcept;

	private:
		Process* const _process;		
		Byte* const _bytecode;
		ThreadStack _stack;
		const Range _executionRange;

		const Byte* _ip;

		// Available callframes
		ThreadStackFrame _stackFrames[VM_RECURSIVE_FN_CALLFRAMES];
		// The current call frame
		ThreadStackFrame* _sf;

		// Error management
		ThreadFlags _haltFlags;
		// Message associated with the halt
		Char _haltMessage[4096];
		// Address where the halt originated from
		const Byte* _haltAddress;
	};
}
