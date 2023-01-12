#include "Thread.h"
#include "Process.h"
#include "Instructions.h"
#include <cstdarg>

using namespace WestCoastCode;
using namespace WestCoastCode::Interpreter;

namespace
{
	const InstrEoe eoe = { (I8)Incode::Eoe };
}

Thread::Thread(Process* process)
	: _process(process), _bytecode(process->GetBytecode()), _executionRange(process->GetExecutionRange()), _ip(nullptr), 
	_haltFlags(0), _haltMessage{0}, _haltAddress(nullptr)
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
#include "Instructions/Ldc_s.c"
#include "Instructions/Ldc_l.c"
#include "Instructions/Ret.c"

void Thread::Exec(const Byte* ip)
{
	Exec0(ip);
	if (_haltFlags != 0)
		throw ThreadErrorHaltedExecution(_haltFlags, _haltMessage);
}

void Thread::Exec0(const Byte* ip) noexcept
{
	while (true)
	{
		// Verify that we are inside execution memory
		if (!_executionRange.Inside(ip)) {
			if (ip != (const Byte*)&eoe) {
				Halt(ip, (ThreadFlags)ThreadFlag::OutsideExecutionMemory, "outside execution memory");
				return;
			}
		}

		// Stop executing of a flag is raised
		if (_haltFlags != 0) return;

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
		case (I32)Opcodes::Ldc_s_I8_0:
			ip = Ldc_s_I8(&_stack, ip, 0);
			continue;
		case (I32)Opcodes::Ldc_s_I8_1:
			ip = Ldc_s_I8(&_stack, ip, 1);
			continue;
		case (I32)Opcodes::Ldc_s_U8_0:
			ip = Ldc_s_U8(&_stack, ip, 0);
			continue;
		case (I32)Opcodes::Ldc_s_U8_1:
			ip = Ldc_s_U8(&_stack, ip, 1);
			continue;
		case (I32)Opcodes::Ldc_s_I16_0:
			ip = Ldc_s_I16(&_stack, ip, 0);
			continue;
		case (I32)Opcodes::Ldc_s_I16_1:
			ip = Ldc_s_I16(&_stack, ip, 1);
			continue;
		case (I32)Opcodes::Ldc_s_U16_0:
			ip = Ldc_s_U16(&_stack, ip, 0);
			continue;
		case (I32)Opcodes::Ldc_s_U16_1:
			ip = Ldc_s_U16(&_stack, ip, 1);
			continue;
		default:
			break;
		}

		// Perform more "generic" instruction types
		switch (header->incode)
		{
		case Incode::Ldc:
			ip = Ldc(this, ip);
			continue;
		case Incode::Ldc_s:
			ip = Ldc_s(this, ip);
			continue;
		case Incode::Ldc_l:
			ip = Ldc_l(this, ip);
			continue;
		case Incode::Ret:
			ip = Ret(this, ip);
			continue;
		case Incode::Eoe:
			ip = (const Byte*)&eoe;
#ifdef ARLANG_INSTRUCTION_DEBUG
			printf("EOE\n");
#endif
			return;
		default:
			ip = Haltf(ip, ThreadFlag::UnknownInstruction, 
				"unknown instruction(opcode=%d, incode=%d, props=[%d,%d,%d])", header->opcode,
				(I32)header->incode, (I32)header->props1, (I32)header->props2, (I32)header->props3);
			break;
		}
	}
}

const Byte* Thread::Halt(const Byte* address, ThreadFlags flags, const char* message) noexcept
{
	_haltAddress = address;
	_haltFlags |= flags;
	sprintf(_haltMessage, message);
	return (const Byte*)&eoe;
}

const Byte* Thread::Halt(const Byte* address, ThreadFlag flags, const char* message) noexcept
{
	return Halt(address, (ThreadFlags)flags, message);
}

const Byte* Thread::Haltf(const Byte* address, ThreadFlags flags, const char* format, ...) noexcept
{
	va_list argptr;

	_haltAddress = address;
	_haltFlags |= flags;

	va_start(argptr, format);
	vsprintf(_haltMessage, format, argptr);
	va_end(argptr);

	return (const Byte*)&eoe;
}

const Byte* Thread::Haltf(const Byte* address, ThreadFlag flags, const char* format, ...) noexcept
{
	va_list argptr;

	_haltAddress = address;
	_haltFlags |= (ThreadFlags)flags;

	va_start(argptr, format);
	vsprintf(_haltMessage, format, argptr);
	va_end(argptr);

	return (const Byte*)&eoe;
}

const Byte* Thread::ReturnToCaller() noexcept
{
	const Byte* nextIP = _cf->ret;
	_cf--;
#ifdef ARLANG_INSTRUCTION_DEBUG
	if (nextIP == (const Byte*)&eoe)
		printf("EOE");
	else
		printf("%p", nextIP);
#endif
	return nextIP;
}
