#ifndef _VMI_THREAD_H_
#include "../vmi_thread.h"
#endif

vmi_ip _vmi_thread_jmp(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_jmp* const instr = (const vmi_instr_jmp*)ip;

	// TODO: Is it possible to make address be the actual bytecode address?
	return instr->destination;
}

vmi_ip _vmi_thread_jmp_true(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_jmp* const instr = (const vmi_instr_jmp*)ip;
	vm_int32* result = (vm_int32*)vmi_stack_pop(&t->stack, sizeof(vm_int32));
	if (*result == TRUE) {
		// TODO: Is it possible to make address be the actual bytecode address?
		return instr->destination;
	}
	return ip + sizeof(const vmi_instr_jmp);
}

vmi_ip _vmi_thread_jmp_false(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_jmp* const instr = (const vmi_instr_jmp*)ip;
	vm_int32* result = (vm_int32*)vmi_stack_pop(&t->stack, sizeof(vm_int32));
	if (*result == FALSE) {
		// TODO: Is it possible to make address be the actual bytecode address?
		return instr->destination;
	}
	return ip + sizeof(const vmi_instr_jmp);
}
