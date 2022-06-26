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
	vm_byte* dest = vmi_stack_push(&t->stack, instr->size);
	const vm_byte* src = t->ebp + instr->offset;
	if (dest == NULL)
		return _vmi_thread_stack_out_of_memory(t, ip);

	switch (instr->size) {
	case 1:
		*dest = *src;
		break;
	case 2:
		*(vm_int16*)dest = *(vm_int16*)src;
		break;
	case 4:
		*(vm_int32*)dest = *(vm_int32*)src;
		break;
	case 8:
		*(vm_int64*)dest = *(vm_int64*)src;
		break;
	case 16:
		*(vm_int64*)dest = *(vm_int64*)src;
		dest += sizeof(vm_int64); src += sizeof(vm_int64);
		*(vm_int64*)dest = *(vm_int64*)src;
		break;
	case 24:
		*(vm_int64*)dest = *(vm_int64*)src;
		dest += sizeof(vm_int64); src += sizeof(vm_int64);
		*(vm_int64*)dest = *(vm_int64*)src;
		dest += sizeof(vm_int64); src += sizeof(vm_int64);
		*(vm_int64*)dest = *(vm_int64*)src;
		break;
	case 32:
		*(vm_int64*)dest = *(vm_int64*)src;
		dest += sizeof(vm_int64); src += sizeof(vm_int64);
		*(vm_int64*)dest = *(vm_int64*)src;
		dest += sizeof(vm_int64); src += sizeof(vm_int64);
		*(vm_int64*)dest = *(vm_int64*)src;
		dest += sizeof(vm_int64); src += sizeof(vm_int64);
		*(vm_int64*)dest = *(vm_int64*)src;
		break;
	default:
		if (instr->size % sizeof(vm_int32) == 0)
			vmi_copy_int32((vm_int32*)dest, (vm_int32*)(t->ebp + instr->offset), instr->size / 4);
		else
			vmi_copy_bytes(dest, t->ebp + instr->offset, instr->size);
		break;
	}
	return ip + sizeof(vmi_instr_load_l);
}

vmi_ip _vmi_thread_save_l(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_save_l* const instr = (const vmi_instr_save_l*)ip;
	vm_byte* dest = (vm_byte*)(t->ebp + instr->offset);
	const vm_byte* src = vmi_stack_pop(&t->stack, instr->size);

	switch (instr->size) {
	case 1:
		*dest = *src;
		break;
	case 2:
		*(vm_int16*)dest = *(vm_int16*)src;
		break;
	case 4:
		*(vm_int32*)dest = *(vm_int32*)src;
		break;
	case 8:
		*(vm_int64*)dest = *(vm_int64*)src;
		break;
	case 16:
		*(vm_int64*)dest = *(vm_int64*)src;
		dest += sizeof(vm_int64); src += sizeof(vm_int64);
		*(vm_int64*)dest = *(vm_int64*)src;
		break;
	case 24:
		*(vm_int64*)dest = *(vm_int64*)src;
		dest += sizeof(vm_int64); src += sizeof(vm_int64);
		*(vm_int64*)dest = *(vm_int64*)src;
		dest += sizeof(vm_int64); src += sizeof(vm_int64);
		*(vm_int64*)dest = *(vm_int64*)src;
		break;
	case 32:
		*(vm_int64*)dest = *(vm_int64*)src;
		dest += sizeof(vm_int64); src += sizeof(vm_int64);
		*(vm_int64*)dest = *(vm_int64*)src;
		dest += sizeof(vm_int64); src += sizeof(vm_int64);
		*(vm_int64*)dest = *(vm_int64*)src;
		dest += sizeof(vm_int64); src += sizeof(vm_int64);
		*(vm_int64*)dest = *(vm_int64*)src;
		break;
	default:
		if (instr->size % sizeof(vm_int32) == 0)
			vmi_copy_int32((vm_int32*)dest, (vm_int32*)src, instr->size / 4);
		else 
			vmi_copy_bytes(dest, src, instr->size);
		break;
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
