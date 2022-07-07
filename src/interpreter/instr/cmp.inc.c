#ifndef _VMI_THREAD_H_
#include "../vmi_thread.h"
#endif

// Compare the two top-most values on the stack
vmi_ip _vmi_thread_cmp_lt(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_cmp* const instr = (const vmi_instr_cmp*)ip;
	vm_int32 rhs = *(vm_int32*)vmi_stack_pop(&t->stack, sizeof(vm_int32));
	vm_int32 lhs = *(vm_int32*)vmi_stack_pop(&t->stack, sizeof(vm_int32));
	*(vm_int32*)vmi_stack_push(&t->stack, sizeof(vm_int32)) = lhs < rhs ? 1 : 0;
	return ip + sizeof(vmi_instr_cmp);
}

// Compare the two top-most values on the stack
vmi_ip _vmi_thread_cmp_gt(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_cmp* const instr = (const vmi_instr_cmp*)ip;
	vm_int32 rhs = *(vm_int32*)vmi_stack_pop(&t->stack, sizeof(vm_int32));
	vm_int32 lhs = *(vm_int32*)vmi_stack_pop(&t->stack, sizeof(vm_int32));
	*(vm_int32*)vmi_stack_push(&t->stack, sizeof(vm_int32)) = lhs > rhs ? 1 : 0;
	return ip + sizeof(vmi_instr_cmp);
}
