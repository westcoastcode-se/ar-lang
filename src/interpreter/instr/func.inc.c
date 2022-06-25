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
	char* const target = vmi_stack_push(&t->stack, instr->size);
	if (target == NULL)
		return _vmi_thread_stack_out_of_memory(t, ip);

	// TODO: Add support for different sizes
	switch (instr->size) {
	case 4:
		*(vm_int32*)target = *(vm_int32*)(t->ebp + instr->offset);
		break;
	case 1:
		*(vm_int8*)target = *(vm_int8*)(t->ebp + instr->offset);
		break;
	case 2:
		*(vm_int16*)target = *(vm_int16*)(t->ebp + instr->offset);
		break;
	case 8:
		*(vm_int64*)target = *(vm_int64*)(t->ebp + instr->offset);
		break;
	default:
		return _vmi_thread_not_implemented(t, ip);
	}
	return ip + sizeof(vmi_instr_load_a);
}

vmi_ip _vmi_thread_save_r(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_save_r* const instr = (const vmi_instr_save_r*)ip;
	vm_byte* const target = (vm_byte*)(t->ebp + instr->offset);

	// TODO: Add support for different sizes
	switch (instr->size) {
	case 4: {
		vm_int32* const value_on_stack = (vm_int32*)vmi_stack_pop(&t->stack, sizeof(vm_int32));
		*((vm_int32*)target) = *value_on_stack;
		break;
	}
	case 1:
	case 2: {
		vm_int16* const value_on_stack = (vm_int16*)vmi_stack_pop(&t->stack, sizeof(vm_int16));
		*((vm_int16*)target) = *value_on_stack;
		break;
	}
	case 8:
	default:
		return _vmi_thread_not_implemented(t, ip);
	}
	return ip + sizeof(vmi_instr_save_r);
}
