#include "Thread.h"
#include "ThreadStack.h"

template<typename T>
const Byte* Mult_T(ThreadStack* const stack, const Byte* ip)
{
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("Mult <%s>", PrimitiveTypeTraits<T>::Name);
#endif
	const T* const rhs = (const T*)stack->Pop(sizeof(T));
	T* const dest = (T*)stack->Top(-(I32)sizeof(T));
	*dest = *dest * *rhs;
	return ip + sizeof(const InstrMult);
}
