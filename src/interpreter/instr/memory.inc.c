#ifndef _VMI_THREAD_H_
#include "../vmi_thread.h"
#endif

vmi_ip _vmi_thread_allocs(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_allocs* const instr = (const vmi_instr_allocs*)ip;
	vmi_stack_push(&t->stack, instr->size);
	return ip + sizeof(vmi_instr_allocs);
}

vmi_ip _vmi_thread_frees(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_frees* const instr = (const vmi_instr_frees*)ip;
	vmi_stack_pop(&t->stack, instr->size);
	return ip + sizeof(vmi_instr_frees);
}

vmi_ip _vmi_thread_alloch(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_alloch* const instr = (const vmi_instr_alloch*)ip;
	const vm_int32 size = *vmi_stack_pop(&t->stack, sizeof(vm_int32));
	if (size <= 0) {
		return _vmi_thread_stack_out_of_memory(t, ip);
	}
	vm_byte* ptr = (vm_byte*)malloc(size);
	*(vm_byte**)vmi_stack_push(&t->stack, sizeof(vm_byte*)) = ptr;
	return ip + sizeof(vmi_instr_alloch);
}

vmi_ip _vmi_thread_alloch_c(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_alloch_c* const instr = (const vmi_instr_alloch_c*)ip;
	vm_byte* ptr = (vm_byte*)malloc(instr->size);
	*(vm_byte**)vmi_stack_push(&t->stack, sizeof(vm_byte*)) = ptr;
	return ip + sizeof(vmi_instr_alloch_c);
}

vmi_ip _vmi_thread_freeh(vmi_thread* t, vmi_ip ip)
{
	vm_byte* ptr = *(vm_byte**)vmi_stack_pop(&t->stack, sizeof(vm_byte*));
	free(ptr);
	return ip + sizeof(vmi_instr_freeh);
}
