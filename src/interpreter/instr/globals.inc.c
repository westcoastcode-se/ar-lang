#ifndef _VMI_THREAD_H_
#include "../vmi_thread.h"
#endif

vmi_ip _vmi_thread_ldg(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_ldg* instr = (const vmi_instr_ldg*)ip;
	vm_byte* dest = vmi_stack_push(&t->stack, instr->size);
	const vm_byte* src = instr->addr;
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
	return ip + sizeof(vmi_instr_ldg);
}

vmi_ip _vmi_thread_stg(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_stg* const instr = (const vmi_instr_stg*)ip;
	vm_byte* dest = instr->addr;
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
	return ip + sizeof(vmi_instr_stg);
}

vmi_ip _vmi_thread_ldg_a(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_ldg_a* const instr = (const vmi_instr_ldg_a*)ip;
	vm_byte* const target = instr->addr;
	vm_byte** const stack_value = (vm_byte**)vmi_stack_push(&t->stack, sizeof(vm_byte*));
	*stack_value = target;
	return ip + sizeof(vmi_instr_ldg_a);
}
