#ifndef _VMI_THREAD_H_
#include "../vmi_thread.h"
#endif

vmi_ip _vmi_thread_stelem(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_stelem* const instr = (const vmi_instr_stelem*)ip;
	const vm_byte* src = vmi_stack_pop(&t->stack, instr->size_per_element);
	const vm_int32 index = *(vm_int32*)vmi_stack_pop(&t->stack, sizeof(vm_int32));
	vm_byte* dest = *(vm_byte**)vmi_stack_pop(&t->stack, sizeof(vm_byte*));

	switch (instr->size_per_element) {
	case 1:
		*(dest + index) = *src;
		break;
	case 2:
		*((vm_int16*)dest + index) = *(vm_int16*)(src);
		break;
	case 4:
		*((vm_int32*)dest + index) = *(vm_int32*)(src);
		break;
	case 8:
		*((vm_int64*)dest +index) = *(vm_int64*)(src);
		break;
	default:
		vmi_copy_bytes(dest + (index * instr->size_per_element), src, instr->size_per_element);
		break;
	}
	return ip + sizeof(vmi_instr_stelem);
}

vmi_ip _vmi_thread_ldelem(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_ldelem* const instr = (const vmi_instr_ldelem*)ip;
	const vm_int32 index = *(vm_int32*)vmi_stack_pop(&t->stack, sizeof(vm_int32));
	const vm_byte* src = *(vm_byte**)vmi_stack_pop(&t->stack, sizeof(vm_byte*));

	vm_byte* const dest = vmi_stack_push(&t->stack, instr->size_per_element);
	switch (instr->size_per_element) {
	case 1:
		*dest = *(src + index);
		break;
	case 2:
		*(vm_int16*)dest = *((vm_int16*)src + index);
		break;
	case 4:
		*(vm_int32*)dest = *((vm_int32*)src + index);
		break;
	case 8:
		*(vm_int64*)dest = *((vm_int64*)src + index);
		break;
	default:
		vmi_copy_bytes(dest + (index * instr->size_per_element), src, instr->size_per_element);
		break;
	}
	return ip + sizeof(vmi_instr_ldelem);
}

vmi_ip _vmi_thread_stelem_s_i1(vmi_thread* t, vmi_ip ip)
{
	const vm_int8* src = vmi_stack_pop(&t->stack, sizeof(vm_int8));
	const vm_int32 index = *(vm_int32*)vmi_stack_pop(&t->stack, sizeof(vm_int32));
	vm_int8* dest = *(vm_int8**)vmi_stack_pop(&t->stack, sizeof(vm_int8*));

	*(dest + index) = *src;
	return ip + sizeof(vmi_instr_stelem_s);
}

vmi_ip _vmi_thread_ldelem_s_i1(vmi_thread* t, vmi_ip ip)
{
	const vm_int32 index = *(vm_int32*)vmi_stack_pop(&t->stack, sizeof(vm_int32));
	vm_byte* src = *(vm_byte**)vmi_stack_pop(&t->stack, sizeof(vm_byte*));

	vm_byte* const dest = vmi_stack_push(&t->stack, sizeof(vm_int8));
	*dest = *(src + index);
	return ip + sizeof(vmi_instr_ldelem_s);
}

vmi_ip _vmi_thread_stelem_s_i2(vmi_thread* t, vmi_ip ip)
{
	const vm_int16* src = (const vm_int16*)vmi_stack_pop(&t->stack, sizeof(vm_int16));
	const vm_int32 index = *(vm_int32*)vmi_stack_pop(&t->stack, sizeof(vm_int32));
	vm_int16* dest = *(vm_int16**)vmi_stack_pop(&t->stack, sizeof(vm_int16*));

	*(dest + index) = *src;
	return ip + sizeof(vmi_instr_stelem_s);
}

vmi_ip _vmi_thread_ldelem_s_i2(vmi_thread* t, vmi_ip ip)
{
	const vm_int32 index = *(vm_int32*)vmi_stack_pop(&t->stack, sizeof(vm_int32));
	vm_int16* memory = *(vm_int16**)vmi_stack_pop(&t->stack, sizeof(vm_int16*));

	vm_int16* const dest = (vm_int16*)vmi_stack_push(&t->stack, sizeof(vm_int16));
	*dest = *(memory + index);
	return ip + sizeof(vmi_instr_ldelem_s);
}

vmi_ip _vmi_thread_stelem_s_i4(vmi_thread* t, vmi_ip ip)
{
	const vm_int32* src = (const vm_int32*)vmi_stack_pop(&t->stack, sizeof(vm_int16));
	const vm_int32 index = *(vm_int32*)vmi_stack_pop(&t->stack, sizeof(vm_int32));
	vm_int32* dest = *(vm_int32**)vmi_stack_pop(&t->stack, sizeof(vm_int32*));

	*(dest + index) = *src;
	return ip + sizeof(vmi_instr_stelem_s);
}

vmi_ip _vmi_thread_ldelem_s_i4(vmi_thread* t, vmi_ip ip)
{
	const vm_int32 index = *(vm_int32*)vmi_stack_pop(&t->stack, sizeof(vm_int32));
	vm_int32* memory = *(vm_int32**)vmi_stack_pop(&t->stack, sizeof(vm_int32*));

	vm_int32* const dest = (vm_int32*)vmi_stack_push(&t->stack, sizeof(vm_int32));
	*dest = *(memory + index);
	return ip + sizeof(vmi_instr_ldelem_s);
}

vmi_ip _vmi_thread_stelem_s_i8(vmi_thread* t, vmi_ip ip)
{
	const vm_int64* src = (const vm_int64*)vmi_stack_pop(&t->stack, sizeof(vm_int16));
	const vm_int32 index = *(vm_int32*)vmi_stack_pop(&t->stack, sizeof(vm_int32));
	vm_int64* dest = *(vm_int64**)vmi_stack_pop(&t->stack, sizeof(vm_int64*));

	*(dest + index) = *src;
	return ip + sizeof(vmi_instr_stelem_s);
}

vmi_ip _vmi_thread_ldelem_s_i8(vmi_thread* t, vmi_ip ip)
{
	const vm_int32 index = *(vm_int32*)vmi_stack_pop(&t->stack, sizeof(vm_int32));
	vm_int64* memory = *(vm_int64**)vmi_stack_pop(&t->stack, sizeof(vm_int64*));

	vm_int64* const dest = (vm_int64*)vmi_stack_push(&t->stack, sizeof(vm_int64));
	*dest = *(memory + index);
	return ip + sizeof(vmi_instr_ldelem_s);
}

vmi_ip _vmi_thread_stelem_s(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_stelem_s* instr = (const vmi_instr_stelem_s*)ip;
	const vm_byte* src = vmi_stack_pop(&t->stack, instr->size);
	const vm_int32 index = *(vm_int32*)vmi_stack_pop(&t->stack, sizeof(vm_int32));
	vm_byte* dest = *(vm_byte**)vmi_stack_pop(&t->stack, sizeof(vm_byte*));

	switch (instr->size) {
	case 1:
		*(dest + index) = *src;
		break;
	case 2:
		*(vm_int16*)(dest + (index * sizeof(vm_int16))) = *(vm_int16*)(src);
		break;
	case 4:
		*(vm_int32*)(dest + (index * sizeof(vm_int32))) = *(vm_int32*)(src);
		break;
	case 8:
		*(vm_int64*)(dest + (index * sizeof(vm_int64))) = *(vm_int64*)(src);
		break;
	default:
		vmi_copy_bytes(dest + (index * instr->size), src, instr->size);
		break;
	}
	return ip + sizeof(vmi_instr_stelem_s);
}


vmi_ip _vmi_thread_ldelem_s(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_ldelem_s* instr = (const vmi_instr_ldelem_s*)ip;
	const vm_int32 index = *(vm_int32*)vmi_stack_pop(&t->stack, sizeof(vm_int32));
	const vm_byte* memory = *(vm_byte**)vmi_stack_pop(&t->stack, sizeof(vm_byte*));

	vm_byte* const dest = vmi_stack_push(&t->stack, instr->size);
	switch (instr->size) {
	case 1:
		*dest = *(memory + index);
		break;
	case 2:
		*(vm_int16*)dest = (vm_int16)(*(memory + (index * sizeof(vm_int16))));
		break;
	case 4:
		*(vm_int32*)dest = (vm_int32)(*(memory + (index * sizeof(vm_int32))));
		break;
	case 8:
		*(vm_int64*)dest = (vm_int64)(*(memory + (index * sizeof(vm_int64))));
		break;
	default:
		vmi_copy_bytes(dest + (index * instr->size), memory, instr->size);
		break;
	}
	return ip + sizeof(vmi_instr_ldelem_s);
}
