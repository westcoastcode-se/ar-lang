#include "Thread.h"
#include "ThreadStack.h"

const Byte* Conv_I8(Thread* const thread, const Byte* ip)
{
	ThreadStack& stack = thread->GetStack();
	const InstrConv* const instr = (const InstrConv*)ip;

	switch (instr->props2)
	{
	case (I8)PrimitiveType::I16: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("Conv <I16>");
#endif
		const I8 from = *(I8*)stack.Pop(sizeof(I8));
		I16* const to = (I16*)stack.Push(sizeof(I16));
		*to = from;
		break;
	}
	default:
		return thread->Haltf(ip, ThreadFlag::UnknownInstruction,
			"unknown Conv props (props=[%d,%d,%d])", (I32)instr->header.props1, (I32)instr->header.props2, (I32)instr->header.props3);
	}
	return ip + sizeof(InstrConv);
}

const Byte* Conv_I32(Thread* const thread, const Byte* ip)
{
	ThreadStack& stack = thread->GetStack();
	const InstrConv* const instr = (const InstrConv*)ip;

	switch (instr->props2)
	{
	case (I8)PrimitiveType::I8: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("<I8>");
#endif
		const I32 from = *(I32*)stack.Pop(sizeof(I32));
		I8* const to = (I8*)stack.Push(sizeof(I8));
		*to = from;
		break;
	}
	case (U8)PrimitiveType::U8: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("<U8>");
#endif
		const I32 from = *(I32*)stack.Pop(sizeof(I32));
		U8* const to = (U8*)stack.Push(sizeof(U8));
		*to = from;
		break;
	}
	case (U8)PrimitiveType::I16: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("<I16>");
#endif
		const I32 from = *(I32*)stack.Pop(sizeof(I32));
		I16* const to = (I16*)stack.Push(sizeof(I16));
		*to = from;
		break;
	}
	case (U8)PrimitiveType::U16: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("<U16>");
#endif
		const I32 from = *(I32*)stack.Pop(sizeof(I32));
		U16* const to = (U16*)stack.Push(sizeof(U16));
		*to = from;
		break;
	}
	case (U8)PrimitiveType::I32: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("<I32>");
#endif
		break;
	}
	case (U8)PrimitiveType::U32: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("<U32>");
#endif
		const I32 from = *(I32*)stack.Pop(sizeof(I32));
		U32* const to = (U32*)stack.Push(sizeof(U32));
		*to = from;
		break;
	}
	case (U8)PrimitiveType::I64: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("<I64>");
#endif
		const I32 from = *(I32*)stack.Pop(sizeof(I32));
		I64* const to = (I64*)stack.Push(sizeof(I64));
		*to = from;
		break;
	}
	case (U8)PrimitiveType::U64: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("<U64>");
#endif
		const I32 from = *(I32*)stack.Pop(sizeof(I32));
		U64* const to = (U64*)stack.Push(sizeof(U64));
		*to = from;
		break;
	}
	case (U8)PrimitiveType::F32: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("<F32>");
#endif
		const I32 from = *(I32*)stack.Pop(sizeof(I32));
		F32* const to = (F32*)stack.Push(sizeof(F32));
		*to = (F32)from;
		break;
	}
	case (U8)PrimitiveType::F64: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("<F64>");
#endif
		const I32 from = *(I32*)stack.Pop(sizeof(I32));
		F64* const to = (F64*)stack.Push(sizeof(F64));
		*to = (F64)from;
		break;
	}
	case (U8)PrimitiveType::Ptr: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("<PTR>");
#endif
		const I32 from = *(I32*)stack.Pop(sizeof(I32));
		Byte** const to = (Byte**)stack.Push(sizeof(Byte*));
		*to = (Byte*)0 + from;
		break;
	}
	default:
		return thread->Haltf(ip, ThreadFlag::UnknownInstruction,
			"unknown Conv props (props=[%d,%d,%d])", (I32)instr->header.props1, (I32)instr->header.props2, (I32)instr->header.props3);
	}
	return ip + sizeof(InstrConv);
}

const Byte* Conv_I64(Thread* const thread, const Byte* ip)
{
	ThreadStack& stack = thread->GetStack();
	const InstrConv* const instr = (const InstrConv*)ip;

	switch (instr->props2)
	{
	case (I8)PrimitiveType::I8: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("<I8>");
#endif
		const I64 from = *(I64*)stack.Pop(sizeof(I64));
		I8* const to = (I8*)stack.Push(sizeof(I8));
		*to = (I8)from;
		break;
	}
	case (U8)PrimitiveType::U8: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("<U8>");
#endif
		const I64 from = *(I64*)stack.Pop(sizeof(I64));
		U8* const to = (U8*)stack.Push(sizeof(U8));
		*to = (U8)from;
		break;
	}
	case (U8)PrimitiveType::I16: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("<I16>");
#endif
		const I64 from = *(I64*)stack.Pop(sizeof(I64));
		I16* const to = (I16*)stack.Push(sizeof(I16));
		*to = (I16)from;
		break;
	}
	case (U8)PrimitiveType::U16: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("<U16>");
#endif
		const I64 from = *(I64*)stack.Pop(sizeof(I64));
		U16* const to = (U16*)stack.Push(sizeof(U16));
		*to = (U16)from;
		break;
	}
	case (U8)PrimitiveType::I32: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("<I32>");
#endif
		const I64 from = *(I64*)stack.Pop(sizeof(I64));
		I32* const to = (I32*)stack.Push(sizeof(I32));
		*to = (I32)from;
		break;
	}
	case (U8)PrimitiveType::U32: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("<U32>");
#endif
		const I64 from = *(I64*)stack.Pop(sizeof(I64));
		U32* const to = (U32*)stack.Push(sizeof(U32));
		*to = (U32)from;
		break;
	}
	case (U8)PrimitiveType::I64: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("<I64>");
#endif
		break;
	}
	case (U8)PrimitiveType::U64: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("<U64>");
#endif
		const I64 from = *(I64*)stack.Pop(sizeof(I64));
		U64* const to = (U64*)stack.Push(sizeof(U64));
		*to = (U64)from;
		break;
	}
	case (U8)PrimitiveType::F32: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("<F32>");
#endif
		const I64 from = *(I64*)stack.Pop(sizeof(I64));
		F32* const to = (F32*)stack.Push(sizeof(F32));
		*to = (F32)from;
		break;
	}
	case (U8)PrimitiveType::F64: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("<F64>");
#endif
		const I64 from = *(I64*)stack.Pop(sizeof(I64));
		F64* const to = (F64*)stack.Push(sizeof(F64));
		*to = (F64)from;
		break;
	}
	case (U8)PrimitiveType::Ptr: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("<PTR>");
#endif
		const I64 from = *(I64*)stack.Pop(sizeof(I64));
		Byte** const to = (Byte**)stack.Push(sizeof(Byte*));
		*to = (Byte*)0 + from;
		break;
	}
	default:
		return thread->Haltf(ip, ThreadFlag::UnknownInstruction,
			"unknown Conv props (props=[%d,%d,%d])", (I32)instr->header.props1, (I32)instr->header.props2, (I32)instr->header.props3);
	}
	return ip + sizeof(InstrConv);
}

const Byte* Conv_F64(Thread* const thread, const Byte* ip)
{
	ThreadStack& stack = thread->GetStack();
	const InstrConv* const instr = (const InstrConv*)ip;

	switch (instr->props2)
	{
	case (I8)PrimitiveType::I8: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("<I8>");
#endif
		const F64 from = *(F64*)stack.Pop(sizeof(F64));
		I8* const to = (I8*)stack.Push(sizeof(I8));
		*to = (I8)from;
		break;
	}
	case (U8)PrimitiveType::U8: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("<U8>");
#endif
		const F64 from = *(F64*)stack.Pop(sizeof(F64));
		U8* const to = (U8*)stack.Push(sizeof(U8));
		*to = (U8)from;
		break;
	}
	case (U8)PrimitiveType::I16: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("<I16>");
#endif
		const F64 from = *(F64*)stack.Pop(sizeof(F64));
		I16* const to = (I16*)stack.Push(sizeof(I16));
		*to = (I16)from;
		break;
	}
	case (U8)PrimitiveType::U16: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("<U16>");
#endif
		const F64 from = *(F64*)stack.Pop(sizeof(F64));
		U16* const to = (U16*)stack.Push(sizeof(U16));
		*to = (U16)from;
		break;
	}
	case (U8)PrimitiveType::I32: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("<I32>");
#endif
		const F64 from = *(F64*)stack.Pop(sizeof(F64));
		I32* const to = (I32*)stack.Push(sizeof(I32));
		*to = (I32)from;
		break;
	}
	case (U8)PrimitiveType::U32: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("<U32>");
#endif
		const F64 from = *(F64*)stack.Pop(sizeof(F64));
		U32* const to = (U32*)stack.Push(sizeof(U32));
		*to = (U32)from;
		break;
	}
	case (U8)PrimitiveType::I64: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("<I64>");
#endif
		const F64 from = *(F64*)stack.Pop(sizeof(F64));
		I64* const to = (I64*)stack.Push(sizeof(I64));
		*to = (I64)from;
		break;
	}
	case (U8)PrimitiveType::U64: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("<U64>");
#endif
		const F64 from = *(F64*)stack.Pop(sizeof(F64));
		U64* const to = (U64*)stack.Push(sizeof(U64));
		*to = (U64)from;
		break;
	}
	case (U8)PrimitiveType::F32: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("<F32>");
#endif
		const F64 from = *(F64*)stack.Pop(sizeof(F64));
		F32* const to = (F32*)stack.Push(sizeof(F32));
		*to = (F32)from;
		break;
	}
	case (U8)PrimitiveType::F64: {
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("<F64>");
#endif
		break;
	}
	default:
		return thread->Haltf(ip, ThreadFlag::UnknownInstruction,
			"unknown Conv props (props=[%d,%d,%d])", (I32)instr->header.props1, (I32)instr->header.props2, (I32)instr->header.props3);
	}
	return ip + sizeof(InstrConv);
}

const Byte* Conv(Thread* const thread, const Byte* ip)
{
	ThreadStack& stack = thread->GetStack();
	const InstrConv* const instr = (const InstrConv*)ip;

	switch (instr->props1)
	{
	case (I8)PrimitiveType::I8:
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("Conv <I8> ");
#endif
		return Conv_I8(thread, ip);
	case (I8)PrimitiveType::I32:
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("Conv <I32> ");
#endif
		return Conv_I32(thread, ip);
	case (I8)PrimitiveType::I64:
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("Conv <I64> ");
#endif
		return Conv_I64(thread, ip);
	case (I8)PrimitiveType::F64:
#ifdef ARLANG_INSTRUCTION_DEBUG
		printf("Conv <F64> ");
#endif
		return Conv_F64(thread, ip);
	default:
		return thread->Haltf(ip, ThreadFlag::UnknownInstruction,
			"unknown Conv props (props=[%d,%d,%d])", (I32)instr->header.props1, (I32)instr->header.props2, (I32)instr->header.props3);
	}
}
