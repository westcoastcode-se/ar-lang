#include "Thread.h"
#include "ThreadStack.h"

const Byte* Ret(Thread* t, const Byte* ip)
{
	const InstrRet* const instr = (const InstrRet*)ip;
	return ip + sizeof(InstrLdc);
}

