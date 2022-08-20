#ifndef _VMI_THREAD_H_
#include "../thread.h"
#endif

vmi_ip _vmi_thread_copy_s_int32(vmi_thread* t, vmi_ip ip)
{
	const vm_int32* const value = (const vm_int32*)vmi_stack_peek(&t->stack, sizeof(vm_int32));
	vm_int32* target = (vm_int32*)vmi_stack_push(&t->stack, sizeof(vm_int32));
	*target = *value;
	return ip + sizeof(vmi_instr_single_instruction);
}
