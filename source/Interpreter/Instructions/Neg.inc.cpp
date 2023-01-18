#include "Thread.h"
#include "ThreadStack.h"

const Byte* Neg_I8(ThreadStack* const stack, const Byte* ip)
{
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("NEG <I8>");
#endif
	I8* const dest = (I8*)stack->Top(-(I32)sizeof(I8));
	*dest = -*dest;
	return ip + sizeof(const InstrNeg);
}

const Byte* Neg_I16(ThreadStack* const stack, const Byte* ip)
{
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("NEG <I16>");
#endif
	I16* const dest = (I16*)stack->Top(-(I32)sizeof(I16));
	*dest = -*dest;
	return ip + sizeof(const InstrNeg);
}

const Byte* Neg_I32(ThreadStack* const stack, const Byte* ip)
{
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("NEG <I32>");
#endif
	I32* const dest = (I32*)stack->Top(-(I32)sizeof(I32));
	*dest = -*dest;
	return ip + sizeof(const InstrNeg);
}

const Byte* Neg_I64(ThreadStack* const stack, const Byte* ip)
{
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("NEG <I64>");
#endif
	I64* const dest = (I64*)stack->Top(-(I32)sizeof(I64));
	*dest = -*dest;
	return ip + sizeof(const InstrNeg);
}

const Byte* Neg_F32(ThreadStack* const stack, const Byte* ip)
{
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("NEG <F32>");
#endif
	F32* const dest = (F32*)stack->Top(-(I32)sizeof(F32));
	*dest = -*dest;
	return ip + sizeof(const InstrNeg);
}

const Byte* Neg_F64(ThreadStack* const stack, const Byte* ip)
{
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("NEG <F64>");
#endif
	F64* const dest = (F64*)stack->Top(-(I32)sizeof(F64));
	*dest = -*dest;
	return ip + sizeof(const InstrNeg);
}
