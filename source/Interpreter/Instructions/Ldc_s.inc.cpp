#include "Thread.h"
#include "ThreadStack.h"

const Byte* Ldc_s(Thread* const thread, const Byte* ip)
{
	ThreadStack& stack = thread->GetStack();
	const InstrLdc_s* const instr = (const InstrLdc_s*)ip;
	switch (instr->props1)
	{
	case (I8)PrimitiveType::I8: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("Ldc_s <I8> %d", (I32)(instr->i8));
#endif
		I8* dest = (I8*)stack.Push(sizeof(I8));
		*dest = instr->i8;
		break;
	}
	case (I8)PrimitiveType::U8: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("Ldc_s <U8> %d", (I32)((U8)instr->i8));
#endif
		U8* dest = (U8*)stack.Push(sizeof(U8));
		*dest = (U8)instr->i8;
		break;
	}
	case (I8)PrimitiveType::I16: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("Ldc_s <I16> %d", (I32)((I16)instr->i16));
#endif
		I16* dest = (I16*)stack.Push(sizeof(I16));
		*dest = (I16)instr->i16;
		break;
	}
	case (I8)PrimitiveType::U16: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("Ldc_s <U16> %d", (I32)((U16)instr->i16));
#endif
		U16* dest = (U16*)stack.Push(sizeof(U16));
		*dest = (U16)instr->i16;
		break;
	}
	default:
		return thread->Haltf(ip, ThreadFlag::UnknownInstruction,
			"unknown Ldc_s props (props=[%d,%d,%d])", (I32)instr->header.props1, (I32)instr->header.props2, (I32)instr->header.props3);
	}
	return ip + sizeof(InstrLdc_s);
}

const Byte* Ldc_s_I8(ThreadStack* const stack, const Byte* ip, I8 val)
{
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("Ldc_s <I8> %d", (I32)val);
#endif
	I8* dest = (I8*)stack->Push(sizeof(I8));
	*dest = val;
	return ip + sizeof(const InstrLdc_s);
}

const Byte* Ldc_s_U8(ThreadStack* const stack, const Byte* ip, U8 val)
{
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("Ldc_s <U8> %d", (I32)val);
#endif
	U8* dest = (U8*)stack->Push(sizeof(U8));
	*dest = val;
	return ip + sizeof(const InstrLdc_s);
}

const Byte* Ldc_s_I16(ThreadStack* const stack, const Byte* ip, I16 val)
{
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("Ldc_s <I16> %d", (I32)val);
#endif
	I16* dest = (I16*)stack->Push(sizeof(I16));
	*dest = val;
	return ip + sizeof(const InstrLdc_s);
}

const Byte* Ldc_s_U16(ThreadStack* const stack, const Byte* ip, U16 val)
{
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("Ldc_s <U16> %d", (I32)val);
#endif
	U16* dest = (U16*)stack->Push(sizeof(U16));
	*dest = val;
	return ip + sizeof(const InstrLdc_s);
}
