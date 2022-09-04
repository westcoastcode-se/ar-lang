#ifndef _arThread_H_
#include "../arThread.h"
#endif

#ifndef _VMI_UTILS_H_
#include "../arUtils.h"
#endif

// Return to the caller
arIP _arThread_ret(arThread* t, arIP ip)
{
	const vmi_instr_ret* const instr = (const vmi_instr_ret*)ip;
	arIP next_ip;
#if defined(VM_STACK_DEBUG)
	const arByte* expected;
#endif

	// Remove memory for all local variables
	if (arLocals_give_back(&t->locals, instr->pop_locals_size) == FALSE) {
		return _arThread_locals_mismanaged_ret(t, ip);
	}

#if defined(VM_STACK_DEBUG)
	expected = t->stack.top - instr->expected_ebp_offset;
	// Make sure that we haven't manipulated the stack in a way that caused it to become malformed
	if (t->ebp != expected)
		return _arThread_stack_mismanaged_ret(t, ip, (arInt32)(expected - t->ebp));
#endif

	next_ip = t->cf.ret;
	t->cf_pos--;
	t->cf = *t->cf_pos;

#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("RET to=%p", next_ip);
#endif
	return next_ip;
}

arIP _arThread_call(arThread* t, arIP ip)
{
	const vmi_instr_call* instr = (const vmi_instr_call*)ip;
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("CALL to=%p", instr->addr);
#endif
	return arThread_beginCall(t, ip, instr->addr, instr->expected_stack_size);
}

arIP _arThread_callnative(arThread* t, arIP ip)
{
	const vmi_instr_callnative* instr = (const vmi_instr_callnative*)ip;

#if defined(VM_STACK_DEBUG)
	// Verify that we've pushed the bare-minimum data on the stack for the function to work
	// 
	// We must've pushed at least "expected_stack_size" in bytes (we also ignore the required function call stack values)
	const arByte* expected = t->stack.top - instr->expected_stack_size;
	if (t->ebp > expected) {
		const arInt32 stack_size = (arInt32)(t->stack.top - t->ebp);
		return _arThread_stack_mismanaged_begin(t, ip, instr->expected_stack_size, stack_size);
	}
#endif
	t->ip = ip;
	switch (instr->func_ptr(t)) {
	default:
		break;
	}
	// If a halt happened then make sure to call the next IP
	if (t->halt_pos != NULL) {
		return t->ip;
	}
	return ip + sizeof(vmi_instr_callnative);
}

arIP _arThread_calluref(arThread* t, arIP ip)
{
	const vmi_instr_calluref* instr = (const vmi_instr_calluref*)ip;
	arIP addr = *(const arByte**)arStack_pop(&t->stack, sizeof(arIP));
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("CALLUREF to=%p", addr);
#endif
	return arThread_beginCall(t, ip, addr, instr->expected_stack_size);
}

arIP _arThread_lda(arThread* t, arIP ip)
{
	const arInstruction_lda* const instr = (const arInstruction_lda*)ip;
	const arByte* src = t->ebp + instr->offset;
	arByte* dest = arStack_push(&t->stack, instr->size);
	if (dest == NULL)
		return _arThread_stack_out_of_memory(t, ip);

	switch (instr->size) {
	case 1:
		*(arInt8*)dest = *(arInt8*)src;
		break;
	case 2:
		*(arInt16*)dest = *(arInt16*)src;
		break;
	case 4:
		*(arInt32*)dest = *(arInt32*)src;
		break;
	case 8:
		*(arInt64*)dest = *(arInt64*)src;
		break;
	case 16:
		*(arInt64*)dest = *(arInt64*)src;
		dest += sizeof(arInt64); src += sizeof(arInt64);
		*(arInt64*)dest = *(arInt64*)src;
		break;
	case 24:
		*(arInt64*)dest = *(arInt64*)src;
		dest += sizeof(arInt64); src += sizeof(arInt64);
		*(arInt64*)dest = *(arInt64*)src;
		dest += sizeof(arInt64); src += sizeof(arInt64);
		*(arInt64*)dest = *(arInt64*)src;
		break;
	case 32:
		*(arInt64*)dest = *(arInt64*)src;
		dest += sizeof(arInt64); src += sizeof(arInt64);
		*(arInt64*)dest = *(arInt64*)src;
		dest += sizeof(arInt64); src += sizeof(arInt64);
		*(arInt64*)dest = *(arInt64*)src;
		dest += sizeof(arInt64); src += sizeof(arInt64);
		*(arInt64*)dest = *(arInt64*)src;
		break;
	default:
		if (instr->size >= sizeof(arInt64) && instr->size % sizeof(arInt64) == 0)
			arMemcpy64((arInt64*)dest, (arInt64*)src, instr->size / 8);
		else if (instr->size >= sizeof(arInt32) && instr->size % sizeof(arInt32) == 0)
			arMemcpy32((arInt32*)dest, (arInt32*)src, instr->size / 4);
		else
			arMemcpy(dest, t->ebp + instr->offset, instr->size);
		break;
	}

#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("LDA offset=%d size=%d", instr->offset, instr->size);
#endif
	return ip + sizeof(arInstruction_lda);
}

arIP _arThread_lda_a(arThread* t, arIP ip)
{
	const arInstruction_lda_a* instr = (const arInstruction_lda_a*)ip;
	arByte* src = t->ebp + instr->offset;
	arByte** dest = (arByte**)arStack_push(&t->stack, sizeof(arByte*));
	if (dest == NULL)
		return _arThread_stack_out_of_memory(t, ip);
	*dest = src;
	return ip + sizeof(arInstruction_lda_a);
}


arIP _arThread_ldf(arThread* t, arIP ip)
{
	const vmi_instr_ldf* instr = (const vmi_instr_ldf*)ip;
	const arByte** dest = (const arByte**)arStack_push(&t->stack, sizeof(const arByte*));
	*dest = instr->addr;
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("LDF to=%p", instr->addr);
#endif
	return ip + sizeof(vmi_instr_ldf);
}
