#include "Thread.h"
#include "ThreadStack.h"

template<typename T>
const Byte* Add_T(ThreadStack* const stack, const Byte* ip)
{
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("Add <%s>", PrimitiveTypeTraits<T>::Name);
#endif
	const T* const rhs = (const T*)stack->Pop(sizeof(T));
	T* const dest = (T*)stack->Top(-(I32)sizeof(T));
	*dest = *dest + *rhs;
	return ip + sizeof(const InstrAdd);
}

const Byte* Add_Ptr(ThreadStack* const stack, const Byte* ip)
{
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("ADD <PTR>");
#endif
	const Byte** const rhs = (const Byte**)stack->Pop(sizeof(Byte*));
	Byte** const dest = (Byte**)stack->Top(-(I32)sizeof(Byte*));
	*dest = *dest + (size_t)*rhs;
	return ip + sizeof(const InstrAdd);
}
