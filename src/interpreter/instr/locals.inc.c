#ifndef _VMI_THREAD_H_
#include "../vmi_thread.h"
#endif

vmi_ip _vmi_thread_locals(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_locals* instr = (const vmi_instr_locals*)ip;
	vmi_stack_push(&t->stack, instr->size);
	return ip + sizeof(vmi_instr_locals);
}
