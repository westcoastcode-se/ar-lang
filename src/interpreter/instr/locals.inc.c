#ifndef _VMI_THREAD_H_
#include "../vmi_thread.h"
#endif

vmi_ip _vmi_thread_locals(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_locals* instr = (const vmi_instr_locals*)ip;
	t->cf_pos->locals_ptr = t->locals_ptr = vmi_locals_take(&t->locals, instr->size);
	if (t->locals_ptr == NULL)
		return _vmi_thread_locals_out_of_memory(t, ip);
	return ip + sizeof(vmi_instr_locals);
}

vmi_ip _vmi_thread_ldl(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_ldl* instr = (const vmi_instr_ldl*)ip;
	vm_byte* dest = vmi_stack_push(&t->stack, instr->size);
	const vm_byte* src = t->locals_ptr + instr->offset;
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
		if (instr->size >= sizeof(vm_int64) && instr->size % sizeof(vm_int64) == 0)
			vmi_copy_int64((vm_int64*)dest, (vm_int64*)(t->locals_ptr + instr->offset), instr->size / 8);
		else if (instr->size >= sizeof(vm_int32) && instr->size % sizeof(vm_int32) == 0)
			vmi_copy_int32((vm_int32*)dest, (vm_int32*)(t->locals_ptr + instr->offset), instr->size / 4);
		else
			vmi_copy_bytes(dest, t->locals_ptr + instr->offset, instr->size);
		break;
	}
	return ip + sizeof(vmi_instr_ldl);
}

vmi_ip _vmi_thread_stl(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_stl* const instr = (const vmi_instr_stl*)ip;
	vm_byte* dest = (vm_byte*)(t->locals_ptr + instr->offset);
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
		if (instr->size >= sizeof(vm_int64) && instr->size % sizeof(vm_int64) == 0)
			vmi_copy_int64((vm_int64*)dest, (vm_int64*)src, instr->size / 8);
		else if (instr->size >= sizeof(vm_int32) && instr->size % sizeof(vm_int32) == 0)
			vmi_copy_int32((vm_int32*)dest, (vm_int32*)src, instr->size / 4);
		else 
			vmi_copy_bytes(dest, src, instr->size);
		break;
	}
	return ip + sizeof(vmi_instr_stl);
}

vmi_ip _vmi_thread_ldl_a(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_ldl_a* const instr = (const vmi_instr_ldl_a*)ip;
	vm_byte* const target = (vm_byte*)(t->locals_ptr + instr->offset);
	vm_byte** const stack_value = (vm_byte**)vmi_stack_push(&t->stack, sizeof(vm_byte*));
	*stack_value = target;
	return ip + sizeof(vmi_instr_ldl_a);
}
