#include "Thread.h"
#include "ThreadStack.h"

const Byte* Add_F64(ThreadStack* const stack, const Byte* ip)
{
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("ADD <F64> ");
#endif
	const F64* const rhs = (const F64*)stack->Pop(sizeof(F64));
	F64* const dest = (F64*)stack->Top(-(I32)sizeof(F64));
	*dest = *dest + *rhs;
	return ip + sizeof(const InstrAdd);
}

const Byte* Add_F32(ThreadStack* const stack, const Byte* ip)
{
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("ADD <F32> ");
#endif
	const F32* const rhs = (const F32*)stack->Pop(sizeof(F32));
	F32* const dest = (F32*)stack->Top(-(I32)sizeof(F32));
	*dest = *dest + *rhs;
	return ip + sizeof(const InstrAdd);
}

const Byte* Add_I64(ThreadStack* const stack, const Byte* ip)
{
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("ADD <I64> ");
#endif
	const I64* const rhs = (const I64*)stack->Pop(sizeof(I64));
	I64* const dest = (I64*)stack->Top(-(I32)sizeof(I64));
	*dest = *dest + *rhs;
	return ip + sizeof(const InstrAdd);
}

const Byte* Add_I32(ThreadStack* const stack, const Byte* ip)
{
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("ADD <I32> ");
#endif
	const I32* const rhs = (const I32*)stack->Pop(sizeof(I32));
	I32* const dest = (I32*)stack->Top(-(I32)sizeof(I32));
	*dest = *dest + *rhs;
	return ip + sizeof(const InstrAdd);
}

const Byte* Add_I16(ThreadStack* const stack, const Byte* ip)
{
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("ADD <I16> ");
#endif
	const I16* const rhs = (const I16*)stack->Pop(sizeof(I16));
	I16* const dest = (I16*)stack->Top(-(I32)sizeof(I16));
	*dest = *dest + *rhs;
	return ip + sizeof(const InstrAdd);
}

const Byte* Add_I8(ThreadStack* const stack, const Byte* ip)
{
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("ADD <I8> ");
#endif
	const I8* const rhs = (const I8*)stack->Pop(sizeof(I8));
	I8* const dest = (I8*)stack->Top(-(I32)sizeof(I8));
	*dest = *dest + *rhs;
	return ip + sizeof(const InstrAdd);
}
