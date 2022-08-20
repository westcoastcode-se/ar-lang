#ifndef _VMI_THREAD_H_
#include "../thread.h"
#endif

// Return to the caller
vmi_ip _vmi_thread_ret(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_ret* const instr = (const vmi_instr_ret*)ip;
	vmi_ip next_ip;
#if defined(VM_STACK_DEBUG)
	const vm_byte* expected;
#endif

	// Remove memory for all local variables
	if (vmi_locals_give_back(&t->locals, instr->pop_locals_size) == FALSE) {
		return _vmi_thread_locals_mismanaged_ret(t, ip);
	}

#if defined(VM_STACK_DEBUG)
	expected = t->stack.top - instr->expected_ebp_offset;
	// Make sure that we haven't manipulated the stack in a way that caused it to become malformed
	if (t->ebp != expected)
		return _vmi_thread_stack_mismanaged_ret(t, ip, (vm_int32)(expected - t->ebp));
#endif

	next_ip = t->cf.ret;
	t->cf_pos--;
	t->cf = *t->cf_pos;
	return next_ip;
}

vmi_ip _vmi_thread_call(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_call* instr = (const vmi_instr_call*)ip;
	return vmi_thread_begin_call(t, ip, instr->addr, instr->expected_stack_size);
}

vmi_ip _vmi_thread_callnative(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_callnative* instr = (const vmi_instr_callnative*)ip;

#if defined(VM_STACK_DEBUG)
	// Verify that we've pushed the bare-minimum data on the stack for the function to work
	// 
	// We must've pushed at least "expected_stack_size" in bytes (we also ignore the required function call stack values)
	const vm_byte* expected = t->stack.top - instr->expected_stack_size;
	if (t->ebp > expected) {
		const vm_int32 stack_size = (vm_int32)(t->stack.top - t->ebp);
		return _vmi_thread_stack_mismanaged_begin(t, ip, instr->expected_stack_size, stack_size);
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

vmi_ip _vmi_thread_calluref(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_calluref* instr = (const vmi_instr_calluref*)ip;
	vmi_ip addr = *(const vm_byte**)vmi_stack_pop(&t->stack, sizeof(vmi_ip));
	return vmi_thread_begin_call(t, ip, addr, instr->expected_stack_size);
}

vmi_ip _vmi_thread_lda(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_lda* instr = (const vmi_instr_lda*)ip;
	const vm_byte* src = t->ebp + instr->offset;
	vm_byte* dest = vmi_stack_push(&t->stack, instr->size);
	if (dest == NULL)
		return _vmi_thread_stack_out_of_memory(t, ip);

	switch (instr->size) {
	case 1:
		*(vm_int8*)dest = *(vm_int8*)src;
		break;
	case 2:
		*(vm_int16*)dest = *(vm_int16*)src;
		break;
	case 4:
		*(vm_int32*)dest = *(vm_int32*)src;
		break;
	case 8:
		*(vm_int64*)dest = *(vm_int64*)src;
		break;
	case 16:
		*(vm_int64*)dest = *(vm_int64*)src;
		dest += sizeof(vm_int64); src += sizeof(vm_int64);
		*(vm_int64*)dest = *(vm_int64*)src;
		break;
	case 24:
		*(vm_int64*)dest = *(vm_int64*)src;
		dest += sizeof(vm_int64); src += sizeof(vm_int64);
		*(vm_int64*)dest = *(vm_int64*)src;
		dest += sizeof(vm_int64); src += sizeof(vm_int64);
		*(vm_int64*)dest = *(vm_int64*)src;
		break;
	case 32:
		*(vm_int64*)dest = *(vm_int64*)src;
		dest += sizeof(vm_int64); src += sizeof(vm_int64);
		*(vm_int64*)dest = *(vm_int64*)src;
		dest += sizeof(vm_int64); src += sizeof(vm_int64);
		*(vm_int64*)dest = *(vm_int64*)src;
		dest += sizeof(vm_int64); src += sizeof(vm_int64);
		*(vm_int64*)dest = *(vm_int64*)src;
		break;
	default:
		if (instr->size >= sizeof(vm_int64) && instr->size % sizeof(vm_int64) == 0)
			vmi_copy_int64((vm_int64*)dest, (vm_int64*)src, instr->size / 8);
		else if (instr->size >= sizeof(vm_int32) && instr->size % sizeof(vm_int32) == 0)
			vmi_copy_int32((vm_int32*)dest, (vm_int32*)src, instr->size / 4);
		else
			vmi_copy_bytes(dest, t->ebp + instr->offset, instr->size);
		break;
	}
	return ip + sizeof(vmi_instr_lda);
}

vmi_ip _vmi_thread_lda_a(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_lda_a* instr = (const vmi_instr_lda_a*)ip;
	vm_byte* src = t->ebp + instr->offset;
	vm_byte** dest = (vm_byte**)vmi_stack_push(&t->stack, sizeof(vm_byte*));
	if (dest == NULL)
		return _vmi_thread_stack_out_of_memory(t, ip);
	*dest = src;
	return ip + sizeof(vmi_instr_lda_a);
}


vmi_ip _vmi_thread_ldf(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_ldf* instr = (const vmi_instr_ldf*)ip;
	const vm_byte** dest = (const vm_byte**)vmi_stack_push(&t->stack, sizeof(const vm_byte*));
	*dest = instr->addr;
	return ip + sizeof(vmi_instr_ldf);
}
