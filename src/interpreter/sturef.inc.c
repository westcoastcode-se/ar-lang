#ifndef _arThread_H_
#include "../arThread.h"
#endif

#ifndef _VMI_UTILS_H_
#include "../arUtils.h"
#endif

arIP _arThread_sturef_s_i1(arThread* t, arIP ip)
{
	const arInt8 value = *(arInt8*)arStack_pop(&t->stack, sizeof(arInt8));
	arInt8* const ptr = *(arInt8**)arStack_pop(&t->stack, sizeof(arInt8*));
	*ptr = value;
	return ip + sizeof(vmi_instr_sturef_s);
}

arIP _arThread_sturef_s_i2(arThread* t, arIP ip)
{
	const arInt16 value = *(arInt16*)arStack_pop(&t->stack, sizeof(arInt16));
	arInt16* const ptr = *(arInt16**)arStack_pop(&t->stack, sizeof(arInt16*));
	*ptr = value;
	return ip + sizeof(vmi_instr_sturef_s);
}

arIP _arThread_sturef_s_i4(arThread* t, arIP ip)
{
	const arInt32 value = *(arInt32*)arStack_pop(&t->stack, sizeof(arInt32));
	arInt32* const ptr = *(arInt32**)arStack_pop(&t->stack, sizeof(arInt32*));
	*ptr = value;
	return ip + sizeof(vmi_instr_sturef_s);
}

arIP _arThread_sturef_s_i8(arThread* t, arIP ip)
{
	const arInt64 value = *(arInt64*)arStack_pop(&t->stack, sizeof(arInt64));
	arInt64* const ptr = *(arInt64**)arStack_pop(&t->stack, sizeof(arInt64*));
	*ptr = value;
	return ip + sizeof(vmi_instr_sturef_s);
}

arIP _arThread_sturef_s(arThread* t, arIP ip)
{
	const vmi_instr_sturef_s* const instr = (const vmi_instr_sturef_s*)ip;
	const arByte* src = arStack_pop(&t->stack, instr->size);
	arByte** const dest = (arByte**)arStack_pop(&t->stack, sizeof(arByte*));
	if (instr->size >= 8 && instr->size % 8) {
		arMemcpy64(*(arInt64**)dest, (arInt64*)src, instr->size / 8);
	}
	else if (instr->size >= 4 && instr->size % 4) {
		arMemcpy32(*(arInt32**)dest, (arInt32*)src, instr->size / 4);
	}
	else {
		arMemcpy(*dest, src, instr->size);
	}
	return ip + sizeof(vmi_instr_sturef_s);
}

arIP _arThread_sturef(arThread* t, arIP ip)
{
	const vmi_instr_sturef* const instr = (const vmi_instr_sturef*)ip;
	const arByte* src = arStack_pop(&t->stack, instr->size);
	arByte** const dest = (arByte**)arStack_pop(&t->stack, sizeof(arByte*));
	if (instr->size >= 8 && instr->size % 8) {
		arMemcpy64(*(arInt64**)dest, (arInt64*)src, instr->size / 8);
	}
	else if (instr->size >= 4 && instr->size % 4) {
		arMemcpy32(*(arInt32**)dest, (arInt32*)src, instr->size / 4);
	}
	else {
		arMemcpy(*dest, src, instr->size);
	}
	return ip + sizeof(vmi_instr_sturef);
}
