#ifndef _VMI_THREAD_H_
#include "../vmi_thread.h"
#endif

// Convert an int16 to int32
vmi_ip _vmi_thread_conv_i16_i32(vmi_thread* t, vmi_ip ip)
{
	const vm_int16 rhs = *(const vm_int16*)vmi_stack_pop(&t->stack, sizeof(vm_int16));
	vm_int32* result = (vm_int32*)vmi_stack_push(&t->stack, sizeof(vm_int32));
	*result = (vm_int32)rhs;
	return ip + sizeof(vmi_instr_conv);
}
