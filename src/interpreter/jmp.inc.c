#ifndef _arThread_H_
#include "../arThread.h"
#endif

#ifndef _VMI_UTILS_H_
#include "../arUtils.h"
#endif

arIP _arThread_jmp(arThread* t, arIP ip)
{
	const vmi_instr_jmp* const instr = (const vmi_instr_jmp*)ip;
	return instr->destination;
}

arIP _arThread_jmp_true(arThread* t, arIP ip)
{
	const vmi_instr_jmp* const instr = (const vmi_instr_jmp*)ip;
	arInt32* result = (arInt32*)arStack_pop(&t->stack, sizeof(arInt32));
	if (*result == TRUE) {
		return instr->destination;
	}
	return ip + sizeof(const vmi_instr_jmp);
}

arIP _arThread_jmp_false(arThread* t, arIP ip)
{
	const vmi_instr_jmp* const instr = (const vmi_instr_jmp*)ip;
	arInt32* result = (arInt32*)arStack_pop(&t->stack, sizeof(arInt32));
	if (*result == FALSE) {
		return instr->destination;
	}
	return ip + sizeof(const vmi_instr_jmp);
}
