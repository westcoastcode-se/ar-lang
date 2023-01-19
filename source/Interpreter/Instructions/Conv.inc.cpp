#include "Thread.h"
#include "ThreadStack.h"

template<typename From, typename To>
const Byte* Conv_From_To(ThreadStack& stack, const Byte* ip)
{
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("Conv <%s> <%s>", PrimitiveTypeString<From>::Name, PrimitiveTypeString<To>::Name);
#endif
	const From from = *(From*)stack.Pop(sizeof(From));
	To* const to = (To*)stack.Push(sizeof(To));
	*to = (To)from;
	return ip + sizeof(InstrConv);
}

template<typename From>
const Byte* Conv_From_ToPtr(ThreadStack& stack, const Byte* ip)
{
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("Conv <%s> <Ptr>", PrimitiveTypeString<From>::Name);
#endif
	const From from = *(From*)stack.Pop(sizeof(From));
	char** const to = (char**)stack.Push(sizeof(char*));
	*to = (char*)(0) + from;
	return ip + sizeof(InstrConv);
}

template<typename From>
const Byte* Conv_From_ToBool(ThreadStack& stack, const Byte* ip)
{
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("Conv <%s> <Bool>", PrimitiveTypeString<From>::Name);
#endif
	const From from = *(From*)stack.Pop(sizeof(From));
	IB* const to = (IB*)stack.Push(sizeof(IB));
	*to = (IB)from;
	return ip + sizeof(InstrConv);
}
