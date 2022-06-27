#ifndef _VMI_THREAD_H_
#include "../vmi_thread.h"
#endif

vmi_ip _vmi_thread_allocs(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_allocs* instr = (const vmi_instr_allocs*)ip;
	vmi_stack_push(&t->stack, instr->size);
	return ip + sizeof(vmi_instr_allocs);
}

vmi_ip _vmi_thread_frees(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_frees* instr = (const vmi_instr_frees*)ip;
	vmi_stack_pop(&t->stack, instr->size);
	return ip + sizeof(vmi_instr_frees);
}

vmi_ip _vmi_thread_alloch(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_alloch* instr = (const vmi_instr_alloch*)ip;
	vm_byte* ptr = (vm_byte*)malloc(instr->size);
	*(vm_byte**)vmi_stack_push(&t->stack, sizeof(vm_byte*)) = ptr;
	return ip + sizeof(vmi_instr_alloch);
}

vmi_ip _vmi_thread_freeh(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_freeh* instr = (const vmi_instr_freeh*)ip;
	vm_byte* ptr = *(vm_byte**)vmi_stack_pop(&t->stack, sizeof(vm_byte*));
	return ip + sizeof(vmi_instr_freeh);
}
