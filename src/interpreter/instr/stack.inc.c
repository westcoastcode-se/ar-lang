#ifndef _VMI_THREAD_H_
#include "../vmi_thread.h"
#endif

vmi_ip _vmi_thread_alloc_s(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_alloc_s* instr = (const vmi_instr_alloc_s*)ip;
	vmi_stack_push(&t->stack, instr->size);
	return ip + sizeof(vmi_instr_alloc_s);
}

vmi_ip _vmi_thread_free_s(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_free_s* instr = (const vmi_instr_free_s*)ip;
	vmi_stack_pop(&t->stack, instr->size);
	return ip + sizeof(vmi_instr_free_s);
}
