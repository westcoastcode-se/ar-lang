#ifndef _arThread_H_
#include "../arThread.h"
#endif

#ifndef _VMI_UTILS_H_
#include "../arUtils.h"
#endif

arIP _arThread_allocs(arThread* t, arIP ip)
{
	const vmi_instr_allocs* const instr = (const vmi_instr_allocs*)ip;
	arStack_push(&t->stack, instr->size);
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("ALLOCS size=%d", instr->size);
#endif
	return ip + sizeof(vmi_instr_allocs);
}

arIP _arThread_frees(arThread* t, arIP ip)
{
	const vmi_instr_frees* const instr = (const vmi_instr_frees*)ip;
	arStack_pop(&t->stack, instr->size);
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("FREES size=%d", instr->size);
#endif
	return ip + sizeof(vmi_instr_frees);
}

arIP _arThread_alloch(arThread* t, arIP ip)
{
	const vmi_instr_alloch* const instr = (const vmi_instr_alloch*)ip;
	const arInt32 size = *arStack_pop(&t->stack, sizeof(arInt32));
	if (size <= 0) {
		return _arThread_stack_out_of_memory(t, ip);
	}
	arByte* ptr = (arByte*)malloc(size);
	*(arByte**)arStack_push(&t->stack, sizeof(arByte*)) = ptr;
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("ALLOCH <stack> size=%d ptr=%p", size, ptr);
#endif
	return ip + sizeof(vmi_instr_alloch);
}

arIP _arThread_alloch_c(arThread* t, arIP ip)
{
	const vmi_instr_alloch_c* const instr = (const vmi_instr_alloch_c*)ip;
	arByte* ptr = (arByte*)malloc(instr->size);
	*(arByte**)arStack_push(&t->stack, sizeof(arByte*)) = ptr;
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("ALLOCH <const> size=%d ptr=%p", instr->size, ptr);
#endif
	return ip + sizeof(vmi_instr_alloch_c);
}

arIP _arThread_freeh(arThread* t, arIP ip)
{
	arByte* ptr = *(arByte**)arStack_pop(&t->stack, sizeof(arByte*));
	free(ptr);
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("FREEH ptr=%p", ptr);
#endif
	return ip + sizeof(vmi_instr_freeh);
}
