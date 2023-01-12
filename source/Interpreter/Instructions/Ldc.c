#include "Thread.h"
#include "ThreadStack.h"

const Byte* Ldc(Thread* const thread, const Byte* ip)
{
	ThreadStack& stack = thread->GetStack();
	const InstrLdc* const instr = (const InstrLdc*)ip;
	switch (instr->props1)
	{
	case (I8)PrimitiveType::I8: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("LDC <I8> %d", (I32)(instr->i8));
#endif
		I8* dest = (I8*)stack.Push(sizeof(I8));
		*dest = instr->i8;
		break;
	}
	case (I8)PrimitiveType::U8: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("LDC <U8> %d", (I32)((U8)instr->i8));
#endif
		U8* dest = (U8*)stack.Push(sizeof(U8));
		*dest = (U8)instr->i8;
		break;
	}
	case (I8)PrimitiveType::I16: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("LDC <I16> %d", (I32)((I16)instr->i16));
#endif
		I16* dest = (I16*)stack.Push(sizeof(I16));
		*dest = (I16)instr->i16;
		break;
	}
	case (I8)PrimitiveType::U16: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("LDC <U16> %d", (I32)((U16)instr->i16));
#endif
		U16* dest = (U16*)stack.Push(sizeof(U16));
		*dest = (U16)instr->i16;
		break;
	}
	case (I8)PrimitiveType::I32: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("LDC <I32> %d", (I32)((I32)instr->i32));
#endif
		I32* dest = (I32*)stack.Push(sizeof(I32));
		*dest = (I32)instr->i32;
		break;
	}
	case (I8)PrimitiveType::U32: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("LDC <U32> %ud", (U32)instr->i32);
#endif
		U32* dest = (U32*)stack.Push(sizeof(U32));
		*dest = (U32)instr->i32;
		break;
	}
	case (I8)PrimitiveType::F32: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("LDC <F32> %f", instr->f32);
#endif
		F32* dest = (F32*)stack.Push(sizeof(F32));
		*dest = (F32)instr->f32;
		break;
	}
#ifdef ARLANG_32BIT
	case (I8)PrimitiveType::Ptr: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("LDC <PTR> %p", instr->ptr);
#endif
		void** dest = (void**)stack.Push(sizeof(void*));
		*dest = (void*)instr->ptr;
		break;
	}
#endif
	default:
		return thread->Haltf(ip, ThreadFlag::UnknownInstruction,
			"unknown Ldc_s props (props=[%d,%d,%d])", (I32)instr->header.props1, (I32)instr->header.props2, (I32)instr->header.props3);
	}
	return ip + sizeof(InstrLdc);
}
