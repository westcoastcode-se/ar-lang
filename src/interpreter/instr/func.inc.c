#ifndef _VMI_THREAD_H_
#include "../vmi_thread.h"
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
	vmi_stack_pop(&t->stack, instr->pop_locals_size);

#if defined(VM_STACK_DEBUG)
	expected = t->stack.top - instr->expected_ebp_offset;
	// Make sure that we haven't manipulated the stack in a way that caused it to become malformed
	if (t->ebp != expected)
		return _vmi_thread_stack_mismanaged_ret(t, ip, (vm_int32)(expected - t->ebp));
#endif

	// Pop the stack pointer and return return the next instruction pointer to be executed
	t->ebp = *(vm_byte**)vmi_stack_pop(&t->stack, sizeof(vm_byte*));
	next_ip = *(vmi_ip*)vmi_stack_pop(&t->stack, sizeof(vmi_ip));
	// Pop a pre-defined amount of bytes from the stack. The amount is normally the arguments, but
	// not the return values
	vmi_stack_pop(&t->stack, instr->pop_stack_size);

	return next_ip;
}

vmi_ip _vmi_thread_call(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_call* instr = (const vmi_instr_call*)ip;
	// TODO: Is it possible to make address be the actual bytecode address?
	vmi_ip new_ip = (vmi_ip)(t->bytecode + (size_t)instr->addr);
	return vmi_thread_begin_call(t, ip, new_ip, instr->expected_stack_size);
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

vmi_ip _vmi_thread_str(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_str* const instr = (const vmi_instr_str*)ip;
	vm_byte* dest = (vm_byte*)(t->ebp + instr->offset);
	const vm_byte* src = vmi_stack_pop(&t->stack, instr->size);

	switch (instr->size) {
	case 1:
		*dest = *src;
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
			vmi_copy_bytes(dest, src, instr->size);
		break;
	}
	return ip + sizeof(vmi_instr_str);
}
