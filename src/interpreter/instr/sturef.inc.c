#ifndef _VMI_THREAD_H_
#include "../vmi_thread.h"
#endif

#ifndef _VMI_UTILS_H_
#include "../vmi_utils.h"
#endif

vmi_ip _vmi_thread_sturef_s_i1(vmi_thread* t, vmi_ip ip)
{
	const vm_int8 value = *(vm_int8*)vmi_stack_pop(&t->stack, sizeof(vm_int8));
	vm_int8* const ptr = *(vm_int8**)vmi_stack_pop(&t->stack, sizeof(vm_int8*));
	*ptr = value;
	return ip + sizeof(vmi_instr_sturef_s);
}

vmi_ip _vmi_thread_sturef_s_i2(vmi_thread* t, vmi_ip ip)
{
	const vm_int16 value = *(vm_int16*)vmi_stack_pop(&t->stack, sizeof(vm_int16));
	vm_int16* const ptr = *(vm_int16**)vmi_stack_pop(&t->stack, sizeof(vm_int16*));
	*ptr = value;
	return ip + sizeof(vmi_instr_sturef_s);
}

vmi_ip _vmi_thread_sturef_s_i4(vmi_thread* t, vmi_ip ip)
{
	const vm_int32 value = *(vm_int32*)vmi_stack_pop(&t->stack, sizeof(vm_int32));
	vm_int32* const ptr = *(vm_int32**)vmi_stack_pop(&t->stack, sizeof(vm_int32*));
	*ptr = value;
	return ip + sizeof(vmi_instr_sturef_s);
}

vmi_ip _vmi_thread_sturef_s_i8(vmi_thread* t, vmi_ip ip)
{
	const vm_int64 value = *(vm_int64*)vmi_stack_pop(&t->stack, sizeof(vm_int64));
	vm_int64* const ptr = *(vm_int64**)vmi_stack_pop(&t->stack, sizeof(vm_int64*));
	*ptr = value;
	return ip + sizeof(vmi_instr_sturef_s);
}

vmi_ip _vmi_thread_sturef_s(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_sturef_s* const instr = (const vmi_instr_sturef_s*)ip;
	const vm_byte* src = vmi_stack_pop(&t->stack, instr->size);
	vm_byte** const dest = (vm_byte**)vmi_stack_pop(&t->stack, sizeof(vm_byte*));
	if (instr->size >= 8 && instr->size % 8) {
		vmi_copy_int64(*(vm_int64**)dest, (vm_int64*)src, instr->size / 8);
	}
	else if (instr->size >= 4 && instr->size % 4) {
		vmi_copy_int32(*(vm_int32**)dest, (vm_int32*)src, instr->size / 4);
	}
	else {
		vmi_copy_bytes(*dest, src, instr->size);
	}
	return ip + sizeof(vmi_instr_sturef_s);
}

vmi_ip _vmi_thread_sturef(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_sturef* const instr = (const vmi_instr_sturef*)ip;
	const vm_byte* src = vmi_stack_pop(&t->stack, instr->size);
	vm_byte** const dest = (vm_byte**)vmi_stack_pop(&t->stack, sizeof(vm_byte*));
	if (instr->size >= 8 && instr->size % 8) {
		vmi_copy_int64(*(vm_int64**)dest, (vm_int64*)src, instr->size / 8);
	}
	else if (instr->size >= 4 && instr->size % 4) {
		vmi_copy_int32(*(vm_int32**)dest, (vm_int32*)src, instr->size / 4);
	}
	else {
		vmi_copy_bytes(*dest, src, instr->size);
	}
	return ip + sizeof(vmi_instr_sturef);
}
