#ifndef _VMI_THREAD_H_
#include "../vmi_thread.h"
#endif

// Add the two top-most values from the stack. Assume that they are 2 byte integers
vmi_ip _vmi_thread_addi16(vmi_thread* t, vmi_ip ip)
{
	const vm_int16 rhs = *(const vm_int16*)vmi_stack_pop(&t->stack, sizeof(vm_int16));
	vm_int16* lhs = (vm_int16*)(t->stack.top - sizeof(vm_int16));
	*lhs = *lhs + rhs;
	return ip + sizeof(vmi_instr_single_instruction);
}


// Add the two top-most values from the stack. Assume that they are 4 byte integers
vmi_ip _vmi_thread_addi32(vmi_thread* t, vmi_ip ip)
{
	const vm_int32 rhs = *(const vm_int32*)vmi_stack_pop(&t->stack, sizeof(vm_int32));
	vm_int32* lhs = (vm_int32*)(t->stack.top - sizeof(vm_int32));
	*lhs = *lhs + rhs;
	return ip + sizeof(vmi_instr_single_instruction);
}
