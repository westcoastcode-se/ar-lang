#ifndef _arThread_H_
#include "../arThread.h"
#endif

#ifndef _VMI_UTILS_H_
#include "../arUtils.h"
#endif

// Compare the two top-most values on the stack
arIP _arThread_cmp_lt(arThread* t, arIP ip)
{
	const vmi_instr_cmp* const instr = (const vmi_instr_cmp*)ip;
	arInt32 rhs = *(arInt32*)arStack_pop(&t->stack, sizeof(arInt32));
	arInt32 lhs = *(arInt32*)arStack_pop(&t->stack, sizeof(arInt32));
	*(arInt32*)arStack_push(&t->stack, sizeof(arInt32)) = lhs < rhs ? 1 : 0;
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("CMP <lt>");
#endif
	return ip + sizeof(vmi_instr_cmp);
}

// Compare the two top-most values on the stack
arIP _arThread_cmp_gt(arThread* t, arIP ip)
{
	const vmi_instr_cmp* const instr = (const vmi_instr_cmp*)ip;
	arInt32 rhs = *(arInt32*)arStack_pop(&t->stack, sizeof(arInt32));
	arInt32 lhs = *(arInt32*)arStack_pop(&t->stack, sizeof(arInt32));
	*(arInt32*)arStack_push(&t->stack, sizeof(arInt32)) = lhs > rhs ? 1 : 0;
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("CMP <gt>");
#endif
	return ip + sizeof(vmi_instr_cmp);
}
