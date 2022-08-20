#ifndef _VMI_THREAD_H_
#include "../thread.h"
#endif

vmi_ip _vmi_thread_ldc_i1(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_ldc_i32* instr = (const vmi_instr_ldc_i32*)ip;
	vm_int8* result = (vm_int8*)vmi_stack_push(&t->stack, sizeof(vm_int8));
	*result = instr->i8;
	return ip + sizeof(vmi_instr_ldc_i32);
}

vmi_ip _vmi_thread_ldc_i2(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_ldc_i32* instr = (const vmi_instr_ldc_i32*)ip;
	vm_int16* result = (vm_int16*)vmi_stack_push(&t->stack, sizeof(vm_int16));
	*result = instr->i16;
	return ip + sizeof(vmi_instr_ldc_i32);
}

vmi_ip _vmi_thread_ldc_i4(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_ldc_i32* instr = (const vmi_instr_ldc_i32*)ip;
	vm_int32* result = (vm_int32*)vmi_stack_push(&t->stack, sizeof(vm_int32));
	*result = instr->i32;
	return ip + sizeof(vmi_instr_ldc_i32);
}

vmi_ip _vmi_thread_ldc_i8(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_ldc_i64* instr = (const vmi_instr_ldc_i64*)ip;
	vm_int64* result = (vm_int64*)vmi_stack_push(&t->stack, sizeof(vm_int64));
	*result = instr->i64;
	return ip + sizeof(vmi_instr_ldc_i64);
}

vmi_ip _vmi_thread_ldc_f4(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_ldc_i32* instr = (const vmi_instr_ldc_i32*)ip;
	vm_float32* result = (vm_float32*)vmi_stack_push(&t->stack, sizeof(vm_float32));
	*result = instr->f32;
	return ip + sizeof(vmi_instr_ldc_i32);
}

vmi_ip _vmi_thread_ldc_f8(vmi_thread* t, vmi_ip ip)
{
	const vmi_instr_ldc_i64* instr = (const vmi_instr_ldc_i64*)ip;
	vm_float64* result = (vm_float64*)vmi_stack_push(&t->stack, sizeof(vm_float64));
	*result = instr->f64;
	return ip + sizeof(vmi_instr_ldc_i64);
}

vmi_ip _vmi_thread_ldc_s_i1(vmi_thread* t, vmi_ip ip, vm_int8 value)
{
	const vmi_instr_ldc_s* instr = (const vmi_instr_ldc_s*)ip;
	vm_int8* result = (vm_int8*)vmi_stack_push(&t->stack, sizeof(vm_int8));
	*result = value;
	return ip + sizeof(vmi_instr_ldc_s);
}

vmi_ip _vmi_thread_ldc_s_i2(vmi_thread* t, vmi_ip ip, vm_int16 value)
{
	const vmi_instr_ldc_s* instr = (const vmi_instr_ldc_s*)ip;
	vm_int16* result = (vm_int16*)vmi_stack_push(&t->stack, sizeof(vm_int16));
	*result = value;
	return ip + sizeof(vmi_instr_ldc_s);
}

vmi_ip _vmi_thread_ldc_s_i4(vmi_thread* t, vmi_ip ip, vm_int32 value)
{
	const vmi_instr_ldc_s* instr = (const vmi_instr_ldc_s*)ip;
	vm_int32* result = (vm_int32*)vmi_stack_push(&t->stack, sizeof(vm_int32));
	*result = value;
	return ip + sizeof(vmi_instr_ldc_s);
}

vmi_ip _vmi_thread_ldc_s_i8(vmi_thread* t, vmi_ip ip, vm_int64 value)
{
	const vmi_instr_ldc_s* instr = (const vmi_instr_ldc_s*)ip;
	vm_int64* result = (vm_int64*)vmi_stack_push(&t->stack, sizeof(vm_int64));
	*result = value;
	return ip + sizeof(vmi_instr_ldc_s);
}

vmi_ip _vmi_thread_ldc_s_f4(vmi_thread* t, vmi_ip ip, vm_float32 value)
{
	const vmi_instr_ldc_s* instr = (const vmi_instr_ldc_s*)ip;
	vm_float32* result = (vm_float32*)vmi_stack_push(&t->stack, sizeof(vm_float32));
	*result = value;
	return ip + sizeof(vmi_instr_ldc_s);
}

vmi_ip _vmi_thread_ldc_s_f8(vmi_thread* t, vmi_ip ip, vm_float64 value)
{
	const vmi_instr_ldc_s* instr = (const vmi_instr_ldc_s*)ip;
	vm_float64* result = (vm_float64*)vmi_stack_push(&t->stack, sizeof(vm_float64));
	*result = value;
	return ip + sizeof(vmi_instr_ldc_s);
}
