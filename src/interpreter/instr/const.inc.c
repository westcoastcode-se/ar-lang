#ifndef _VMI_THREAD_H_
#include "../vmi_thread.h"
#endif

vmi_ip _vmi_thread_const_int16(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_const_int32* instr = (const vmi_instr_const_int32*)ip;
	vm_int16* result = (vm_int16*)vmi_stack_push(&t->stack, sizeof(vm_int16));
	*result = instr->i16;
	return ip + sizeof(vmi_instr_const_int32);
}

vmi_ip _vmi_thread_const_int32(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_const_int32* instr = (const vmi_instr_const_int32*)ip;
	vm_int32* result = (vm_int32*)vmi_stack_push(&t->stack, sizeof(vm_int32));
	*result = instr->value;
	return ip + sizeof(vmi_instr_const_int32);
}
