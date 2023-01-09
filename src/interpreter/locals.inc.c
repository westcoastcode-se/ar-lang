#ifndef _arThread_H_
#include "../arThread.h"
#endif

#ifndef _VMI_UTILS_H_
#include "../arUtils.h"
#endif

arIP _arThread_locals(arThread* t, arIP ip)
{
	const vmi_instr_locals* const instr = (const vmi_instr_locals*)ip;
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("LOCALS size=%d", instr->size);
#endif
	t->cf_pos->locals_ptr = t->locals_ptr = arLocals_take(&t->locals, instr->size);
	if (t->locals_ptr == NULL)
		return _arThread_locals_out_of_memory(t, ip);
	return ip + sizeof(vmi_instr_locals);
}

arIP _arThread_ldl(arThread* t, arIP ip)
{
	const arInstruction_ldl* const instr = (const arInstruction_ldl*)ip;
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("LDL offset=%d size=%d", instr->offset, instr->size);
#endif
	arByte* dest = arStack_push(&t->stack, instr->size);
	const arByte* src = t->locals_ptr + instr->offset;
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
	return ip + sizeof(arInstruction_ldl);
}

arIP _arThread_stl(arThread* t, arIP ip)
{
	const arInstruction_stl* const instr = (const arInstruction_stl*)ip;
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("STL offset=%d size=%d", instr->offset, instr->size);
#endif
	arByte* dest = (arByte*)(t->locals_ptr + instr->offset);
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
	return ip + sizeof(arInstruction_stl);
}

arIP _arThread_ldl_a(arThread* t, arIP ip)
{
	const arInstruction_ldl_a* const instr = (const arInstruction_ldl_a*)ip;
#ifdef ARLANG_INSTRUCTION_DEBUG
	printf("LDL_A offset=%d size=%d", instr->offset, instr->size);
#endif
	arByte* const target = (arByte*)(t->locals_ptr + instr->offset);
	arByte** const stack_value = (arByte**)arStack_push(&t->stack, sizeof(arByte*));
	*stack_value = target;
	return ip + sizeof(arInstruction_ldl_a);
}
