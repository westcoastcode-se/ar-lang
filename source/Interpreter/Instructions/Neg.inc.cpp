#include "Thread.h"
#include "ThreadStack.h"

template<typename T>
const Byte* Neg_T(ThreadStack* const stack, const Byte* ip)
{
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("Neg <%s>", PrimitiveTypeString<T>::Name);
#endif
	T* const dest = (T*)stack->Top(-(I32)sizeof(T));
	*dest = -*dest;
	return ip + sizeof(const InstrNeg);
}
