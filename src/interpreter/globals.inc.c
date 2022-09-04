#ifndef _arThread_H_
#include "../arThread.h"
#endif

#ifndef _VMI_UTILS_H_
#include "../arUtils.h"
#endif

arIP _arThread_ldg(arThread* t, arIP ip)
{
	const arInstruction_ldg* const instr = (const arInstruction_ldg*)ip;
	arByte* dest = arStack_push(&t->stack, instr->size);
	const arByte* src = instr->addr;
	if (dest == NULL)
		return _arThread_stack_out_of_memory(t, ip);

	switch (instr->size) {
	case 1:
		*dest = *src;
		break;
	case 2:
		*(arInt16*)dest = *(arInt16*)src;
		break;
	case 4:
		*(arInt32*)dest = *(arInt32*)src;
		break;
	case 8:
		*(arInt64*)dest = *(arInt64*)src;
		break;
	case 16:
		*(arInt64*)dest = *(arInt64*)src;
		dest += sizeof(arInt64); src += sizeof(arInt64);
		*(arInt64*)dest = *(arInt64*)src;
		break;
	case 24:
		*(arInt64*)dest = *(arInt64*)src;
		dest += sizeof(arInt64); src += sizeof(arInt64);
		*(arInt64*)dest = *(arInt64*)src;
		dest += sizeof(arInt64); src += sizeof(arInt64);
		*(arInt64*)dest = *(arInt64*)src;
		break;
	case 32:
		*(arInt64*)dest = *(arInt64*)src;
		dest += sizeof(arInt64); src += sizeof(arInt64);
		*(arInt64*)dest = *(arInt64*)src;
		dest += sizeof(arInt64); src += sizeof(arInt64);
		*(arInt64*)dest = *(arInt64*)src;
		dest += sizeof(arInt64); src += sizeof(arInt64);
		*(arInt64*)dest = *(arInt64*)src;
		break;
	default:
		if (instr->size >= sizeof(arInt64) && instr->size % sizeof(arInt64) == 0)
			arMemcpy64((arInt64*)dest, (arInt64*)(t->locals_ptr + instr->offset), instr->size / 8);
		else if (instr->size >= sizeof(arInt32) && instr->size % sizeof(arInt32) == 0)
			arMemcpy32((arInt32*)dest, (arInt32*)(t->locals_ptr + instr->offset), instr->size / 4);
		else
			arMemcpy(dest, t->locals_ptr + instr->offset, instr->size);
		break;
	}
	return ip + sizeof(arInstruction_ldg);
}

arIP _arThread_stg(arThread* t, arIP ip)
{
	const arInstruction_stg* const instr = (const arInstruction_stg*)ip;
	arByte* dest = instr->addr;
	const arByte* src = arStack_pop(&t->stack, instr->size);

	switch (instr->size) {
	case 1:
		*dest = *src;
		break;
	case 2:
		*(arInt16*)dest = *(arInt16*)src;
		break;
	case 4:
		*(arInt32*)dest = *(arInt32*)src;
		break;
	case 8:
		*(arInt64*)dest = *(arInt64*)src;
		break;
	case 16:
		*(arInt64*)dest = *(arInt64*)src;
		dest += sizeof(arInt64); src += sizeof(arInt64);
		*(arInt64*)dest = *(arInt64*)src;
		break;
	case 24:
		*(arInt64*)dest = *(arInt64*)src;
		dest += sizeof(arInt64); src += sizeof(arInt64);
		*(arInt64*)dest = *(arInt64*)src;
		dest += sizeof(arInt64); src += sizeof(arInt64);
		*(arInt64*)dest = *(arInt64*)src;
		break;
	case 32:
		*(arInt64*)dest = *(arInt64*)src;
		dest += sizeof(arInt64); src += sizeof(arInt64);
		*(arInt64*)dest = *(arInt64*)src;
		dest += sizeof(arInt64); src += sizeof(arInt64);
		*(arInt64*)dest = *(arInt64*)src;
		dest += sizeof(arInt64); src += sizeof(arInt64);
		*(arInt64*)dest = *(arInt64*)src;
		break;
	default:
		if (instr->size >= sizeof(arInt64) && instr->size % sizeof(arInt64) == 0)
			arMemcpy64((arInt64*)dest, (arInt64*)src, instr->size / 8);
		else if (instr->size >= sizeof(arInt32) && instr->size % sizeof(arInt32) == 0)
			arMemcpy32((arInt32*)dest, (arInt32*)src, instr->size / 4);
		else 
			arMemcpy(dest, src, instr->size);
		break;
	}
	return ip + sizeof(arInstruction_stg);
}

arIP _arThread_ldg_a(arThread* t, arIP ip)
{
	const arInstruction_ldg_a* const instr = (const arInstruction_ldg_a*)ip;
	arByte* const target = instr->addr;
	arByte** const stack_value = (arByte**)arStack_push(&t->stack, sizeof(arByte*));
	*stack_value = target;
	return ip + sizeof(arInstruction_ldg_a);
}
