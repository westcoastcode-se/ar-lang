#ifndef _VMI_THREAD_H_
#include "../thread.h"
#endif

// Add the two top-most values from the stack. Assume that they are 1 byte integers
vmi_ip _vmi_thread_addi1(vmi_thread* t, vmi_ip ip)
{
	const vm_int8 rhs = *(const vm_int8*)vmi_stack_pop(&t->stack, sizeof(vm_int8));
	vm_int8* lhs = (vm_int8*)(t->stack.top - sizeof(vm_int8));
	*lhs = *lhs + rhs;
	return ip + sizeof(vmi_instr_single_instruction);
}

// Add the two top-most values from the stack. Assume that they are 1 byte integers
vmi_ip _vmi_thread_addui1(vmi_thread* t, vmi_ip ip)
{
	const vm_uint8 rhs = *(const vm_uint8*)vmi_stack_pop(&t->stack, sizeof(vm_uint8));
	vm_uint8* lhs = (vm_uint8*)(t->stack.top - sizeof(vm_uint8));
	*lhs = *lhs + rhs;
	return ip + sizeof(vmi_instr_single_instruction);
}

// Add the two top-most values from the stack. Assume that they are 2 byte integers
vmi_ip _vmi_thread_addi2(vmi_thread* t, vmi_ip ip)
{
	const vm_int16 rhs = *(const vm_int16*)vmi_stack_pop(&t->stack, sizeof(vm_int16));
	vm_int16* lhs = (vm_int16*)(t->stack.top - sizeof(vm_int16));
	*lhs = *lhs + rhs;
	return ip + sizeof(vmi_instr_single_instruction);
}

// Add the two top-most values from the stack. Assume that they are 2 byte integers
vmi_ip _vmi_thread_addui2(vmi_thread* t, vmi_ip ip)
{
	const vm_uint16 rhs = *(const vm_uint16*)vmi_stack_pop(&t->stack, sizeof(vm_uint16));
	vm_uint16* lhs = (vm_uint16*)(t->stack.top - sizeof(vm_uint16));
	*lhs = *lhs + rhs;
	return ip + sizeof(vmi_instr_single_instruction);
}

// Add the two top-most values from the stack. Assume that they are 4 byte integers
vmi_ip _vmi_thread_addi4(vmi_thread* t, vmi_ip ip)
{
	const vm_int32 rhs = *(const vm_int32*)vmi_stack_pop(&t->stack, sizeof(vm_int32));
	vm_int32* lhs = (vm_int32*)(t->stack.top - sizeof(vm_int32));
	*lhs = *lhs + rhs;
	return ip + sizeof(vmi_instr_single_instruction);
}

// Add the two top-most values from the stack. Assume that they are 4 byte integers
vmi_ip _vmi_thread_addui4(vmi_thread* t, vmi_ip ip)
{
	const vm_uint32 rhs = *(const vm_uint32*)vmi_stack_pop(&t->stack, sizeof(vm_uint32));
	vm_uint32* lhs = (vm_uint32*)(t->stack.top - sizeof(vm_uint32));
	*lhs = *lhs + rhs;
	return ip + sizeof(vmi_instr_single_instruction);
}

// Add the two top-most values from the stack. Assume that they are 8 byte integers
vmi_ip _vmi_thread_addi8(vmi_thread* t, vmi_ip ip)
{
	const vm_int64 rhs = *(const vm_int64*)vmi_stack_pop(&t->stack, sizeof(vm_int64));
	vm_int64* lhs = (vm_int64*)(t->stack.top - sizeof(vm_int64));
	*lhs = *lhs + rhs;
	return ip + sizeof(vmi_instr_single_instruction);
}

// Add the two top-most values from the stack. Assume that they are 8 byte integers
vmi_ip _vmi_thread_addui8(vmi_thread* t, vmi_ip ip)
{
	const vm_uint64 rhs = *(const vm_uint64*)vmi_stack_pop(&t->stack, sizeof(vm_uint64));
	vm_uint64* lhs = (vm_uint64*)(t->stack.top - sizeof(vm_uint64));
	*lhs = *lhs + rhs;
	return ip + sizeof(vmi_instr_single_instruction);
}

// Add the two top-most values from the stack. Assume that they are 4 byte floats
vmi_ip _vmi_thread_addf4(vmi_thread* t, vmi_ip ip)
{
	const vm_float32 rhs = *(const vm_float32*)vmi_stack_pop(&t->stack, sizeof(vm_float32));
	vm_float32* lhs = (vm_float32*)(t->stack.top - sizeof(vm_float32));
	*lhs = *lhs + rhs;
	return ip + sizeof(vmi_instr_single_instruction);
}

// Add the two top-most values from the stack. Assume that they are 8 byte floats
vmi_ip _vmi_thread_addf8(vmi_thread* t, vmi_ip ip)
{
	const vm_float64 rhs = *(const vm_float64*)vmi_stack_pop(&t->stack, sizeof(vm_float64));
	vm_float64* lhs = (vm_float64*)(t->stack.top - sizeof(vm_float64));
	*lhs = *lhs + rhs;
	return ip + sizeof(vmi_instr_single_instruction);
}
