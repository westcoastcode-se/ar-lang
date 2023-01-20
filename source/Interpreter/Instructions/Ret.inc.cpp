#include "Thread.h"
#include "ThreadStack.h"

const Byte* Ret(Thread* t, const Byte* ip)
{
	const InstrRet* const instr = (const InstrRet*)ip;
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("Ret ");
#endif
	// Remove memory for all local variables
	// // TODO: This should be done using scope-in and scope-out statements
	//if (arLocals_give_back(&t->locals, instr->pop_locals_size) == FALSE) {
	//	return _arThread_locals_mismanaged_ret(t, ip);
	//}

#if defined(VM_STACK_DEBUG)
	// Make sure that we haven't manipulated the stack in a way that caused it to become malformed
	const Byte* const expected = t->GetStack().Top(-instr->expected_ebp_offset);
	if (expected != t->GetCallFrame()->ebp)
		return t->Halt(ip, (I32)ThreadFlag::StackMismanaged, "invalid stack when returning");
#endif
	return t->ReturnToCaller();
}

