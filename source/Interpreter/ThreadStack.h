#pragma once

#include "ProcessError.h"
#include "Primitive.h"

namespace WestCoastCode::Interpreter
{
	// Process running the virtual machine
	struct ThreadStack
	{
	public:
		ThreadStack();

		// Move the stack pointer forward and return a pointer to where we can store the number of blocks required
		// Returns NULL if there is no more free memory on the stack
		Byte* Push(I32 size);

		// Move the stack backwards. This will return a pointer to where you can read
		// the poped value from. Will return NULL if you've popped to much memory
		Byte* Pop(I32 size);

		// Get the size of the stack
		I32 GetSize() const { return 0; }

	private:
		Byte _blocks[VM_STACK_DEFAULT_SIZE];
		Byte* _top;
		const Byte* _end;
	};
}
