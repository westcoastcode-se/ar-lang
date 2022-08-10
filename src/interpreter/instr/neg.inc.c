#ifndef _VMI_THREAD_H_
#include "../vmi_thread.h"
#endif

// Negate 1 byte of memory on the stack
vmi_ip _vmi_thread_negi1(vmi_thread* t, vmi_ip ip)
{
	vm_int8* value = (vm_int8*)(t->stack.top - sizeof(vm_int8));
	*value = -(*value);
	return ip + sizeof(vmi_instr_single_instruction);
}

// Negate 1 byte of memory on the stack
vmi_ip _vmi_thread_negui1(vmi_thread* t, vmi_ip ip)
{
	vm_uint8* value = (vm_uint8*)(t->stack.top - sizeof(vm_uint8));
	*value = -(*value);
	return ip + sizeof(vmi_instr_single_instruction);
}

// Negate 2 byte of memory on the stack
vmi_ip _vmi_thread_negi2(vmi_thread* t, vmi_ip ip)
{
	vm_int16* value = (vm_int16*)(t->stack.top - sizeof(vm_int16));
	*value = -(*value);
	return ip + sizeof(vmi_instr_single_instruction);
}

// Negate 2 byte of memory on the stack
vmi_ip _vmi_thread_negui2(vmi_thread* t, vmi_ip ip)
{
	vm_uint16* value = (vm_uint16*)(t->stack.top - sizeof(vm_uint16));
	*value = -(*value);
	return ip + sizeof(vmi_instr_single_instruction);
}

// Negate 4 byte of memory on the stack
vmi_ip _vmi_thread_negi4(vmi_thread* t, vmi_ip ip)
{
	vm_int32* value = (vm_int32*)(t->stack.top - sizeof(vm_int32));
	*value = -(*value);
	return ip + sizeof(vmi_instr_single_instruction);
}

// Negate 4 byte of memory on the stack
vmi_ip _vmi_thread_negui4(vmi_thread* t, vmi_ip ip)
{
	vm_uint32* value = (vm_uint32*)(t->stack.top - sizeof(vm_uint32));
	*value = -(*value);
	return ip + sizeof(vmi_instr_single_instruction);
}

// Negate 8 byte of memory on the stack
vmi_ip _vmi_thread_negi8(vmi_thread* t, vmi_ip ip)
{
	vm_int64* value = (vm_int64*)(t->stack.top - sizeof(vm_int64));
	*value = -(*value);
	return ip + sizeof(vmi_instr_single_instruction);
}

// Negate 8 byte of memory on the stack
vmi_ip _vmi_thread_negui8(vmi_thread* t, vmi_ip ip)
{
	vm_uint64* value = (vm_uint64*)(t->stack.top - sizeof(vm_uint64));
	*value = -(*value);
	return ip + sizeof(vmi_instr_single_instruction);
}

// Negate 4 byte of memory on the stack
vmi_ip _vmi_thread_negf4(vmi_thread* t, vmi_ip ip)
{
	vm_float32* value = (vm_float32*)(t->stack.top - sizeof(vm_float32));
	*value = -(*value);
	return ip + sizeof(vmi_instr_single_instruction);
}

// Negate 8 byte of memory on the stack
vmi_ip _vmi_thread_negf8(vmi_thread* t, vmi_ip ip)
{
	vm_float64* value = (vm_float64*)(t->stack.top - sizeof(vm_float64));
	*value = -(*value);
	return ip + sizeof(vmi_instr_single_instruction);
}