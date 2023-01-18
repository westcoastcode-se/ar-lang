#include "Thread.h"
#include "ThreadStack.h"

const Byte* BitNot_I8(ThreadStack* const stack, const Byte* ip)
{
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("BitNot <I8>");
#endif
	I8* const dest = (I8*)stack->Top(-(I32)sizeof(I8));
	*dest = ~*dest;
	return ip + sizeof(const InstrBitNot);
}

const Byte* BitNot_U8(ThreadStack* const stack, const Byte* ip)
{
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("BitNot <U8>");
#endif
	U8* const dest = (U8*)stack->Top(-(I32)sizeof(U8));
	*dest = ~*dest;
	return ip + sizeof(const InstrBitNot);
}

const Byte* BitNot_I16(ThreadStack* const stack, const Byte* ip)
{
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("BitNot <I16>");
#endif
	I16* const dest = (I16*)stack->Top(-(I32)sizeof(I16));
	*dest = ~*dest;
	return ip + sizeof(const InstrBitNot);
}

const Byte* BitNot_U16(ThreadStack* const stack, const Byte* ip)
{
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("BitNot <U16>");
#endif
	U16* const dest = (U16*)stack->Top(-(I32)sizeof(U16));
	*dest = ~*dest;
	return ip + sizeof(const InstrBitNot);
}

const Byte* BitNot_I32(ThreadStack* const stack, const Byte* ip)
{
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("BitNot <I32>");
#endif
	I32* const dest = (I32*)stack->Top(-(I32)sizeof(I32));
	*dest = ~*dest;
	return ip + sizeof(const InstrBitNot);
}

const Byte* BitNot_U32(ThreadStack* const stack, const Byte* ip)
{
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("BitNot <U32>");
#endif
	U32* const dest = (U32*)stack->Top(-(I32)sizeof(U32));
	*dest = ~*dest;
	return ip + sizeof(const InstrBitNot);
}

const Byte* BitNot_I64(ThreadStack* const stack, const Byte* ip)
{
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("BitNot <I64>");
#endif
	I64* const dest = (I64*)stack->Top(-(I32)sizeof(I64));
	*dest = ~*dest;
	return ip + sizeof(const InstrBitNot);
}

const Byte* BitNot_U64(ThreadStack* const stack, const Byte* ip)
{
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("BitNot <U64>");
#endif
	U64* const dest = (U64*)stack->Top(-(I32)sizeof(U64));
	*dest = ~*dest;
	return ip + sizeof(const InstrBitNot);
}
