#include "Thread.h"
#include "Process.h"
#include "Instructions.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Interpreter;

namespace
{
	const InstrEoe eoe = { (I8)Incode::Eoe };
}

Thread::Thread(Process* process)
	: _process(process), _bytecode(process->GetBytecode()), _executionRange(process->GetExecutionRange()), _ip(nullptr), _flags(0), _halt_message{0}
{
}

Thread::~Thread()
{
}

void Thread::ExecEntrypoint(const Function* entrypoint)
{
	_ip = entrypoint->GetAddress();

	// Verify that we've pushed the data on the stack for the entry-point function to work
	if (_stack.GetSize() != entrypoint->GetStackSize()) {
		throw ThreadErrorStackMismanaged(entrypoint->GetStackSize(), _stack.GetSize());
	}

	// Prepare callframe information
	_cf = _callFrames;
	_cf->ret = (Byte*)&eoe;
	_cf->ebp = _stack.Top(-entrypoint->GetStackSize());
	return Exec(_ip);
}

#include "Instructions/Ldc.c"
#include "Instructions/Ret.c"

void Thread::Exec(const Byte* ip)
{
	Exec0(ip);
	if (_flags != 0)
		throw ThreadErrorHaltedExecution(_flags, _halt_message);
}

void Thread::Exec0(const Byte* ip) noexcept
{
	while (true)
	{
		// Verify that we are inside execution memory
		if (!_executionRange.Inside(_ip)) {
			if (ip == (const Byte*)&eoe)
				return;
			_flags |= (ThreadFlags)ThreadFlag::OutsideExecutionMemory;
			sprintf(_halt_message, "outside execution memory");
			return;
		}

#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("\n");
#endif
		// Stop executing of a flag is raised
		if (_flags != 0) return;

#ifdef ARLANG_INSTRUCTION_DEBUG
#	ifdef VM_STACK_DEBUG
		printf("\n%p] [stack=%05d] ", ip, _stack.GetSize());
#	else
		printf("\n%p] ", ip);
#	endif
#endif
		const OpcodeHeader* const header = (const OpcodeHeader*)ip;

		// Perform special-case (most common instruction types)
		switch (header->opcode) {
		case (I32)Opcodes::Ldc_I8:
			ip = Ldc_I8(&_stack, ip);
			continue;
		case (I32)Opcodes::Ldc_s_I8:
			ip = Ldc_s_I8(&_stack, ip);
			continue;
		default:
			break;
		}

		// Perform more "generic" instruction types
		switch (header->incode)
		{
		case Incode::Ret:
			ip = Ret(this, ip);
			break;
		case Incode::Eoe:
			ip = (const Byte*)&eoe;
#ifdef ARLANG_INSTRUCTION_DEBUG
			printf("EOE\n");
#endif
			return;
		default:
			_flags |= (ThreadFlags)ThreadFlag::UnknownInstruction;
			sprintf(_halt_message, "unknown instruction(opcode=%d, incode=%d, props=[%d,%d,%d])", header->opcode,
				(I32)header->incode, (I32)header->props1, (I32)header->props2, (I32)header->props3);
			break;
		}
	}
}
