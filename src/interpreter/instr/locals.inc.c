#ifndef _VMI_THREAD_H_
#include "../vmi_thread.h"
#endif

vmi_ip _vmi_thread_locals(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_locals* instr = (const vmi_instr_locals*)ip;
	vmi_stack_push(&t->stack, instr->size);
	return ip + sizeof(vmi_instr_locals);
}

vmi_ip _vmi_thread_load_l(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_load_l* instr = (const vmi_instr_load_l*)ip;
	char* const target = vmi_stack_push(&t->stack, instr->size);
	if (target == NULL)
		return _vmi_thread_stack_out_of_memory(t, ip);

	// TODO: Add support for different sizes
	switch (instr->size) {
	case 4:
		*(vm_int32*)target = *(vm_int32*)(t->ebp + instr->offset);
		break;
	case 1:
	case 2:
		*(vm_int16*)target = *(vm_int16*)(t->ebp + instr->offset);
		break;
	case 8:
	default:
		return _vmi_thread_not_implemented(t, ip);
	}
	return ip + sizeof(vmi_instr_load_l);
}

vmi_ip _vmi_thread_save_l(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_save_l* const instr = (const vmi_instr_save_l*)ip;
	vm_byte* const target = (vm_byte*)(t->ebp + instr->offset);

	// TODO: Add support for different sizes
	switch (instr->size) {
	case 4: {
		vm_int32* const value_on_stack = (vm_int32*)vmi_stack_pop(&t->stack, sizeof(vm_int32));
		*((vm_int32*)target) = *value_on_stack;
		break;
	}
	case 1:
	case 2: {
		vm_int16* const value_on_stack = (vm_int16*)vmi_stack_pop(&t->stack, sizeof(vm_int16));
		*((vm_int16*)target) = *value_on_stack;
		break;
	}
	case 8:
	default:
		return _vmi_thread_not_implemented(t, ip);
	}
	return ip + sizeof(vmi_instr_save_l);
}

vmi_ip _vmi_thread_ldl_a(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_ldl_a* const instr = (const vmi_instr_ldl_a*)ip;
	vm_byte* const target = (vm_byte*)(t->ebp + instr->offset);
	vm_byte** const stack_value = (vm_byte**)vmi_stack_push(&t->stack, sizeof(vm_byte*));
	*stack_value = target;
	return ip + sizeof(vmi_instr_ldl_a);
}

vmi_ip _vmi_thread_sunref_int32(vmi_thread* t, vmi_ip ip)
{
	// it is assumed that the value on top is of the same type as the pointer in this case
	const vm_int32 value = *(vm_int32*)vmi_stack_pop(&t->stack, sizeof(vm_int32));
	vm_int32* const ptr = *(vm_int32**)vmi_stack_pop(&t->stack, sizeof(vm_int32*));
	*ptr = value;
	return ip + sizeof(vmi_instr_single_instruction);
}
