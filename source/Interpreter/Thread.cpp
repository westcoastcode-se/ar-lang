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

#include "Instructions/Ldc.inc.cpp"
#include "Instructions/Ldc_s.inc.cpp"
#include "Instructions/Ldc_l.inc.cpp"
#include "Instructions/Conv.inc.cpp"
#include "Instructions/Ret.inc.cpp"
#include "Instructions/Add.inc.cpp"
#include "Instructions/Neg.inc.cpp"
#include "Instructions/BitNot.inc.cpp"

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
		printf("\n%p] ", ip);
#endif
#if defined(VM_STACK_DEBUG)
		printf("[stack=%05d] ", _stack.GetSize());
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

		case (I32)Opcodes::Conv_I8_Bool:
			ip = Conv_From_ToBool<I8>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I8_I8:
			ip = Conv_From_To<I8, I8>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I8_U8:
			ip = Conv_From_To<I8, U8>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I8_I16:
			ip = Conv_From_To<I8, I16>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I8_U16:
			ip = Conv_From_To<I8, U16>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I8_I32:
			ip = Conv_From_To<I8, I32>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I8_U32:
			ip = Conv_From_To<I8, U32>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I8_I64:
			ip = Conv_From_To<I8, I64>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I8_U64:
			ip = Conv_From_To<I8, U64>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I8_F32:
			ip = Conv_From_To<I8, F32>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I8_F64:
			ip = Conv_From_To<I8, F64>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I8_Ptr:
			ip = Conv_From_ToPtr<I8>(_stack, ip);
			continue;

		case (I32)Opcodes::Conv_U8_Bool:
			ip = Conv_From_ToBool<U8>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U8_I8:
			ip = Conv_From_To<U8, I8>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U8_U8:
			ip = Conv_From_To<U8, U8>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U8_I16:
			ip = Conv_From_To<U8, I16>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U8_U16:
			ip = Conv_From_To<U8, U16>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U8_I32:
			ip = Conv_From_To<U8, I32>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U8_U32:
			ip = Conv_From_To<U8, U32>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U8_I64:
			ip = Conv_From_To<U8, I64>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U8_U64:
			ip = Conv_From_To<U8, U64>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U8_F32:
			ip = Conv_From_To<U8, F32>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U8_F64:
			ip = Conv_From_To<U8, F64>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U8_Ptr:
			ip = Conv_From_ToPtr<U8>(_stack, ip);
			continue;

		case (I32)Opcodes::Conv_I16_Bool:
			ip = Conv_From_ToBool<I16>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I16_I8:
			ip = Conv_From_To<I16, I8>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I16_U8:
			ip = Conv_From_To<I16, I8>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I16_I16:
			ip = Conv_From_To<I16, I16>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I16_U16:
			ip = Conv_From_To<I16, U16>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I16_I32:
			ip = Conv_From_To<I16, I32>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I16_U32:
			ip = Conv_From_To<I16, U32>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I16_I64:
			ip = Conv_From_To<I16, I64>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I16_U64:
			ip = Conv_From_To<I16, U64>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I16_F32:
			ip = Conv_From_To<I16, F32>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I16_F64:
			ip = Conv_From_To<I16, F64>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I16_Ptr:
			ip = Conv_From_ToPtr<I16>(_stack, ip);
			continue;

		case (I32)Opcodes::Conv_U16_Bool:
			ip = Conv_From_ToBool<U16>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U16_I8:
			ip = Conv_From_To<U16, I8>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U16_U8:
			ip = Conv_From_To<U16, I8>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U16_I16:
			ip = Conv_From_To<U16, I16>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U16_U16:
			ip = Conv_From_To<U16, U16>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U16_I32:
			ip = Conv_From_To<U16, I32>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U16_U32:
			ip = Conv_From_To<U16, U32>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U16_I64:
			ip = Conv_From_To<U16, I64>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U16_U64:
			ip = Conv_From_To<U16, U64>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U16_F32:
			ip = Conv_From_To<U16, F32>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U16_F64:
			ip = Conv_From_To<U16, F64>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U16_Ptr:
			ip = Conv_From_ToPtr<U16>(_stack, ip);
			continue;

		case (I32)Opcodes::Conv_I32_Bool:
			ip = Conv_From_ToBool<I32>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I32_I8:
			ip = Conv_From_To<I32, I8>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I32_U8:
			ip = Conv_From_To<I32, I8>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I32_I16:
			ip = Conv_From_To<I32, I16>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I32_U16:
			ip = Conv_From_To<I32, U16>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I32_I32:
			ip = Conv_From_To<I32, I32>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I32_U32:
			ip = Conv_From_To<I32, U32>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I32_I64:
			ip = Conv_From_To<I32, I64>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I32_U64:
			ip = Conv_From_To<I32, U64>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I32_F32:
			ip = Conv_From_To<I32, F32>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I32_F64:
			ip = Conv_From_To<I32, F64>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I32_Ptr:
			ip = Conv_From_ToPtr<I32>(_stack, ip);
			continue;

		case (I32)Opcodes::Conv_U32_Bool:
			ip = Conv_From_ToBool<U32>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U32_I8:
			ip = Conv_From_To<U32, I8>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U32_U8:
			ip = Conv_From_To<U32, I8>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U32_I16:
			ip = Conv_From_To<U32, I16>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U32_U16:
			ip = Conv_From_To<U32, U16>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U32_I32:
			ip = Conv_From_To<U32, I32>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U32_U32:
			ip = Conv_From_To<U32, U32>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U32_I64:
			ip = Conv_From_To<U32, I64>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U32_U64:
			ip = Conv_From_To<U32, U64>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U32_F32:
			ip = Conv_From_To<U32, F32>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U32_F64:
			ip = Conv_From_To<U32, F64>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U32_Ptr:
			ip = Conv_From_ToPtr<U32>(_stack, ip);
			continue;

		case (I32)Opcodes::Conv_I64_Bool:
			ip = Conv_From_ToBool<I64>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I64_I8:
			ip = Conv_From_To<I64, I8>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I64_U8:
			ip = Conv_From_To<I64, I8>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I64_I16:
			ip = Conv_From_To<I64, I16>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I64_U16:
			ip = Conv_From_To<I64, U16>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I64_I32:
			ip = Conv_From_To<I64, I32>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I64_U32:
			ip = Conv_From_To<I64, U32>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I64_I64:
			ip = Conv_From_To<I64, I64>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I64_U64:
			ip = Conv_From_To<I64, U64>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I64_F32:
			ip = Conv_From_To<I64, F32>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I64_F64:
			ip = Conv_From_To<I64, F64>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_I64_Ptr:
			ip = Conv_From_ToPtr<I64>(_stack, ip);
			continue;

		case (I32)Opcodes::Conv_U64_Bool:
			ip = Conv_From_ToBool<U64>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U64_I8:
			ip = Conv_From_To<U64, I8>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U64_U8:
			ip = Conv_From_To<U64, I8>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U64_I16:
			ip = Conv_From_To<U64, I16>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U64_U16:
			ip = Conv_From_To<U64, U16>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U64_I32:
			ip = Conv_From_To<U64, I32>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U64_U32:
			ip = Conv_From_To<U64, U32>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U64_I64:
			ip = Conv_From_To<U64, I64>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U64_U64:
			ip = Conv_From_To<U64, U64>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U64_F32:
			ip = Conv_From_To<U64, F32>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U64_F64:
			ip = Conv_From_To<U64, F64>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_U64_Ptr:
			ip = Conv_From_ToPtr<U64>(_stack, ip);
			continue;

		case (I32)Opcodes::Conv_F32_I8:
			ip = Conv_From_To<F32, I8>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_F32_U8:
			ip = Conv_From_To<F32, I8>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_F32_I16:
			ip = Conv_From_To<F32, I16>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_F32_U16:
			ip = Conv_From_To<F32, U16>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_F32_I32:
			ip = Conv_From_To<F32, I32>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_F32_U32:
			ip = Conv_From_To<F32, U32>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_F32_I64:
			ip = Conv_From_To<F32, I64>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_F32_U64:
			ip = Conv_From_To<F32, U64>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_F32_F32:
			ip = Conv_From_To<F32, F32>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_F32_F64:
			ip = Conv_From_To<F32, F64>(_stack, ip);
			continue;

		case (I32)Opcodes::Conv_F64_I8:
			ip = Conv_From_To<F64, I8>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_F64_U8:
			ip = Conv_From_To<F64, I8>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_F64_I16:
			ip = Conv_From_To<F64, I16>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_F64_U16:
			ip = Conv_From_To<F64, U16>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_F64_I32:
			ip = Conv_From_To<F64, I32>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_F64_U32:
			ip = Conv_From_To<F64, U32>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_F64_I64:
			ip = Conv_From_To<F64, I64>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_F64_U64:
			ip = Conv_From_To<F64, U64>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_F64_F32:
			ip = Conv_From_To<F64, F32>(_stack, ip);
			continue;
		case (I32)Opcodes::Conv_F64_F64:
			ip = Conv_From_To<F64, F64>(_stack, ip);
			continue;

		case (I32)Opcodes::Add_I8:
			ip = Add_I8(&_stack, ip);
			continue;
		case (I32)Opcodes::Add_I16:
			ip = Add_I16(&_stack, ip);
			continue;
		case (I32)Opcodes::Add_I32:
			ip = Add_I32(&_stack, ip);
			continue;
		case (I32)Opcodes::Add_I64:
			ip = Add_I64(&_stack, ip);
			continue;
		case (I32)Opcodes::Add_F32:
			ip = Add_F32(&_stack, ip);
			continue;
		case (I32)Opcodes::Add_F64:
			ip = Add_F64(&_stack, ip);
			continue;

		case (I32)Opcodes::Neg_I8:
			ip = Neg_I8(&_stack, ip);
			continue;
		case (I32)Opcodes::Neg_I16:
			ip = Neg_I16(&_stack, ip);
			continue;
		case (I32)Opcodes::Neg_I32:
			ip = Neg_I32(&_stack, ip);
			continue;
		case (I32)Opcodes::Neg_I64:
			ip = Neg_I64(&_stack, ip);
			continue;
		case (I32)Opcodes::Neg_F32:
			ip = Neg_F32(&_stack, ip);
			continue;
		case (I32)Opcodes::Neg_F64:
			ip = Neg_F64(&_stack, ip);
			continue;

		case (I32)Opcodes::BitNot_I8:
			ip = BitNot_I8(&_stack, ip);
			continue;
		case (I32)Opcodes::BitNot_U8:
			ip = BitNot_U8(&_stack, ip);
			continue;
		case (I32)Opcodes::BitNot_I16:
			ip = BitNot_I16(&_stack, ip);
			continue;
		case (I32)Opcodes::BitNot_U16:
			ip = BitNot_U16(&_stack, ip);
			continue;
		case (I32)Opcodes::BitNot_I32:
			ip = BitNot_I32(&_stack, ip);
			continue;
		case (I32)Opcodes::BitNot_U32:
			ip = BitNot_U32(&_stack, ip);
			continue;
		case (I32)Opcodes::BitNot_I64:
			ip = BitNot_I64(&_stack, ip);
			continue;
		case (I32)Opcodes::BitNot_U64:
			ip = BitNot_U64(&_stack, ip);
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
