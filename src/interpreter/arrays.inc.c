#ifndef _arThread_H_
#include "../arThread.h"
#endif

#ifndef _VMI_UTILS_H_
#include "../arUtils.h"
#endif

arIP _arThread_stelem(arThread* t, arIP ip)
{
	const vmi_instr_stelem* const instr = (const vmi_instr_stelem*)ip;
	const arByte* src = arStack_pop(&t->stack, instr->size_per_element);
	const arInt32 index = *(arInt32*)arStack_pop(&t->stack, sizeof(arInt32));
	arByte* dest = *(arByte**)arStack_pop(&t->stack, sizeof(arByte*));

	switch (instr->size_per_element) {
	case 1:
		*(dest + index) = *src;
		break;
	case 2:
		*((arInt16*)dest + index) = *(arInt16*)(src);
		break;
	case 4:
		*((arInt32*)dest + index) = *(arInt32*)(src);
		break;
	case 8:
		*((arInt64*)dest +index) = *(arInt64*)(src);
		break;
	default:
		arMemcpy(dest + (index * instr->size_per_element), src, instr->size_per_element);
		break;
	}
	return ip + sizeof(vmi_instr_stelem);
}

arIP _arThread_ldelem(arThread* t, arIP ip)
{
	const vmi_instr_ldelem* const instr = (const vmi_instr_ldelem*)ip;
	const arInt32 index = *(arInt32*)arStack_pop(&t->stack, sizeof(arInt32));
	const arByte* src = *(arByte**)arStack_pop(&t->stack, sizeof(arByte*));

	arByte* const dest = arStack_push(&t->stack, instr->size_per_element);
	switch (instr->size_per_element) {
	case 1:
		*dest = *(src + index);
		break;
	case 2:
		*(arInt16*)dest = *((arInt16*)src + index);
		break;
	case 4:
		*(arInt32*)dest = *((arInt32*)src + index);
		break;
	case 8:
		*(arInt64*)dest = *((arInt64*)src + index);
		break;
	default:
		arMemcpy(dest + (index * instr->size_per_element), src, instr->size_per_element);
		break;
	}
	return ip + sizeof(vmi_instr_ldelem);
}

arIP _arThread_stelem_s_i1(arThread* t, arIP ip)
{
	const arInt8* src = arStack_pop(&t->stack, sizeof(arInt8));
	const arInt32 index = *(arInt32*)arStack_pop(&t->stack, sizeof(arInt32));
	arInt8* dest = *(arInt8**)arStack_pop(&t->stack, sizeof(arInt8*));

	*(dest + index) = *src;
	return ip + sizeof(vmi_instr_stelem_s);
}

arIP _arThread_ldelem_s_i1(arThread* t, arIP ip)
{
	const arInt32 index = *(arInt32*)arStack_pop(&t->stack, sizeof(arInt32));
	arByte* src = *(arByte**)arStack_pop(&t->stack, sizeof(arByte*));

	arByte* const dest = arStack_push(&t->stack, sizeof(arInt8));
	*dest = *(src + index);
	return ip + sizeof(vmi_instr_ldelem_s);
}

arIP _arThread_stelem_s_i2(arThread* t, arIP ip)
{
	const arInt16* src = (const arInt16*)arStack_pop(&t->stack, sizeof(arInt16));
	const arInt32 index = *(arInt32*)arStack_pop(&t->stack, sizeof(arInt32));
	arInt16* dest = *(arInt16**)arStack_pop(&t->stack, sizeof(arInt16*));

	*(dest + index) = *src;
	return ip + sizeof(vmi_instr_stelem_s);
}

arIP _arThread_ldelem_s_i2(arThread* t, arIP ip)
{
	const arInt32 index = *(arInt32*)arStack_pop(&t->stack, sizeof(arInt32));
	arInt16* memory = *(arInt16**)arStack_pop(&t->stack, sizeof(arInt16*));

	arInt16* const dest = (arInt16*)arStack_push(&t->stack, sizeof(arInt16));
	*dest = *(memory + index);
	return ip + sizeof(vmi_instr_ldelem_s);
}

arIP _arThread_stelem_s_i4(arThread* t, arIP ip)
{
	const arInt32* src = (const arInt32*)arStack_pop(&t->stack, sizeof(arInt32));
	const arInt32 index = *(arInt32*)arStack_pop(&t->stack, sizeof(arInt32));
	arInt32* dest = *(arInt32**)arStack_pop(&t->stack, sizeof(arInt32*));

	*(dest + index) = *src;
	return ip + sizeof(vmi_instr_stelem_s);
}

arIP _arThread_ldelem_s_i4(arThread* t, arIP ip)
{
	const arInt32 index = *(arInt32*)arStack_pop(&t->stack, sizeof(arInt32));
	arInt32* memory = *(arInt32**)arStack_pop(&t->stack, sizeof(arInt32*));

	arInt32* const dest = (arInt32*)arStack_push(&t->stack, sizeof(arInt32));
	*dest = *(memory + index);
	return ip + sizeof(vmi_instr_ldelem_s);
}

arIP _arThread_stelem_s_i8(arThread* t, arIP ip)
{
	const arInt64* src = (const arInt64*)arStack_pop(&t->stack, sizeof(arInt64));
	const arInt32 index = *(arInt32*)arStack_pop(&t->stack, sizeof(arInt32));
	arInt64* dest = *(arInt64**)arStack_pop(&t->stack, sizeof(arInt64*));

	*(dest + index) = *src;
	return ip + sizeof(vmi_instr_stelem_s);
}

arIP _arThread_ldelem_s_i8(arThread* t, arIP ip)
{
	const arInt32 index = *(arInt32*)arStack_pop(&t->stack, sizeof(arInt32));
	arInt64* memory = *(arInt64**)arStack_pop(&t->stack, sizeof(arInt64*));

	arInt64* const dest = (arInt64*)arStack_push(&t->stack, sizeof(arInt64));
	*dest = *(memory + index);
	return ip + sizeof(vmi_instr_ldelem_s);
}

arIP _arThread_stelem_s(arThread* t, arIP ip)
{
	const vmi_instr_stelem_s* instr = (const vmi_instr_stelem_s*)ip;
	const arByte* src = arStack_pop(&t->stack, instr->size);
	const arInt32 index = *(arInt32*)arStack_pop(&t->stack, sizeof(arInt32));
	arByte* dest = *(arByte**)arStack_pop(&t->stack, sizeof(arByte*));

	switch (instr->size) {
	case 1:
		*(dest + index) = *src;
		break;
	case 2:
		*(arInt16*)(dest + (index * sizeof(arInt16))) = *(arInt16*)(src);
		break;
	case 4:
		*(arInt32*)(dest + (index * sizeof(arInt32))) = *(arInt32*)(src);
		break;
	case 8:
		*(arInt64*)(dest + (index * sizeof(arInt64))) = *(arInt64*)(src);
		break;
	default:
		arMemcpy(dest + (index * instr->size), src, instr->size);
		break;
	}
	return ip + sizeof(vmi_instr_stelem_s);
}


arIP _arThread_ldelem_s(arThread* t, arIP ip)
{
	const vmi_instr_ldelem_s* instr = (const vmi_instr_ldelem_s*)ip;
	const arInt32 index = *(arInt32*)arStack_pop(&t->stack, sizeof(arInt32));
	const arByte* memory = *(arByte**)arStack_pop(&t->stack, sizeof(arByte*));

	arByte* const dest = arStack_push(&t->stack, instr->size);
	switch (instr->size) {
	case 1:
		*dest = *(memory + index);
		break;
	case 2:
		*(arInt16*)dest = (arInt16)(*(memory + (index * sizeof(arInt16))));
		break;
	case 4:
		*(arInt32*)dest = (arInt32)(*(memory + (index * sizeof(arInt32))));
		break;
	case 8:
		*(arInt64*)dest = (arInt64)(*(memory + (index * sizeof(arInt64))));
		break;
	default:
		arMemcpy(dest + (index * instr->size), memory, instr->size);
		break;
	}
	return ip + sizeof(vmi_instr_ldelem_s);
}
