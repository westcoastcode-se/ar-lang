#include "Thread.h"
#include "ThreadStack.h"

const Byte* Ldc_l(Thread* const thread, const Byte* ip)
{
	ThreadStack& stack = thread->GetStack();
	const InstrLdc_l* const instr = (const InstrLdc_l*)ip;
	switch (instr->props1)
	{
	case (I8)PrimitiveType::F64: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("LDC_L <F64> %f", instr->f64);
#endif
		F64* dest = (F64*)stack.Push(sizeof(F64));
		*dest = (F64)instr->f64;
		break;
	}
	case (I8)PrimitiveType::I64: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("LDC_L <I64> %lld", (I64)instr->i64);
#endif
		I64* dest = (I64*)stack.Push(sizeof(I64));
		*dest = (I64)instr->i64;
		break;
	}
	case (I8)PrimitiveType::U64: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("LDC_L <U64> %llud", (U64)instr->i64);
#endif
		U64* dest = (U64*)stack.Push(sizeof(U64));
		*dest = (U64)instr->i64;
		break;
	}
#ifdef ARLANG_64BIT
	case (I8)PrimitiveType::Ptr: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("LDC_L <PTR> %p", instr->ptr);
#endif
		void** dest = (void**)stack.Push(sizeof(void*));
		*dest = (void*)instr->ptr;
		break;
	}
#endif
	default:
		return thread->Haltf(ip, ThreadFlag::UnknownInstruction,
			"unknown Ldc_l props (props=[%d,%d,%d])", (I32)instr->header.props1, (I32)instr->header.props2, (I32)instr->header.props3);
	}
	return ip + sizeof(InstrLdc_l);
}
