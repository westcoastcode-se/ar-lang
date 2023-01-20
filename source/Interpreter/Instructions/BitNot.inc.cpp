#include "Thread.h"
#include "ThreadStack.h"

template<typename T>
const Byte* BitNot_T(ThreadStack* const stack, const Byte* ip)
{
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("BitNot <%s>", PrimitiveTypeTraits<T>::Name);
#endif
	T* const dest = (T*)stack->Top(-(I32)sizeof(T));
	*dest = ~*dest;
	return ip + sizeof(const InstrBitNot);
}
