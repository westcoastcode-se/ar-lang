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

	// Push the address where the application should continue executing when the function returns
	*(vmi_ip*)vmi_stack_push(&t->stack, sizeof(vmi_ip)) = (ip + sizeof(vmi_instr_call));
	// Return the functions start position.
	// TODO: Is it possible to make address be the actual bytecode address?
	return (vmi_ip)(t->bytecode + (size_t)instr->addr);
}

vmi_ip _vmi_thread_begin(vmi_thread* t, vmi_ip ip)
{
	// TODO: The problem is that call_frame points to the wrong frame when poping from the stack
	const vmi_instr_begin* const instr = (const vmi_instr_begin*)ip;

#if defined(VM_STACK_DEBUG)
	// Verify that we've pushed the bare-minimum data on the stack for the 
	// function to work
	// 
	// we must've pushed at least "expected_stack_size" in bytes
	const vm_byte* expected = t->stack.top - instr->expected_stack_size - sizeof(vmi_ip) - sizeof(vm_byte*);
	if (t->ebp + instr->expected_stack_size > expected)
		return _vmi_thread_stack_mismanaged_begin(t, ip, instr->expected_stack_size);
#endif

	// Push the previous stack pointer and set where arguments and 
	// return value slots are located on the stack
	* (vm_byte**)vmi_stack_push(&t->stack, sizeof(vm_byte*)) = t->ebp;

	// New ebp is where the arguments and the return values can be found.
	// It is on before the previous ebp, the return value address and at the start of all (assumed)
	// allocated memory on the stack
	t->ebp = t->stack.top - sizeof(vmi_ip) - sizeof(vm_byte*) - instr->expected_stack_size;

	return ip + sizeof(vmi_instr_begin);
}

vmi_ip _vmi_thread_load_a(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_load_a* instr = (const vmi_instr_load_a*)ip;
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
		if (instr->size % sizeof(vm_int32) == 0)
			vmi_copy_int32((vm_int32*)dest, (vm_int32*)src, instr->size / 4);
		else
			vmi_copy_bytes(dest, t->ebp + instr->offset, instr->size);
		break;
	}
	return ip + sizeof(vmi_instr_load_a);
}

vmi_ip _vmi_thread_lda_a(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_lda_a* instr = (const vmi_instr_lda_a*)ip;
	const vm_byte* src = t->ebp + instr->offset;
	vm_byte** dest = (vm_byte**)vmi_stack_push(&t->stack, sizeof(vm_byte*));
	if (dest == NULL)
		return _vmi_thread_stack_out_of_memory(t, ip);
	*dest = src;
	return ip + sizeof(vmi_instr_lda_a);
}

vmi_ip _vmi_thread_save_r(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_save_r* const instr = (const vmi_instr_save_r*)ip;
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
		if (instr->size % sizeof(vm_int32) == 0)
			vmi_copy_int32((vm_int32*)dest, (vm_int32*)src, instr->size / 4);
		else
			vmi_copy_bytes(dest, src, instr->size);
		break;
	}
	return ip + sizeof(vmi_instr_save_r);
}
