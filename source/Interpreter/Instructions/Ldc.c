#include "Thread.h"
#include "ThreadStack.h"

const Byte* Ldc_I8(ThreadStack* const stack, const Byte* ip)
{
	const InstrLdc* const instr = (const InstrLdc*)ip;
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("LDC <I8>");
#endif
	I8* dest = (I8*)stack->Push(sizeof(I8));
	*dest = instr->i8;
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("%d", instr->i8);
#endif
	return ip + sizeof(InstrLdc);
}

const Byte* Ldc_s_I8(ThreadStack* const stack, const Byte* ip)
{
	const InstrLdc_s* const instr = (const InstrLdc_s*)ip;
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("LDC_S <I8>");
#endif
	I8* dest = (I8*)stack->Push(sizeof(I8));
	*dest = instr->i8;
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("%d", instr->i8);
#endif
	return ip + sizeof(InstrLdc_s);
}
