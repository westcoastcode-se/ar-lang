#include "const.h"
#include "../interpreter/vmi_ops.h"

vmp_const vmp_const_i1(vm_int8 value)
{
	vmp_const c;
	c.i1 = value;
	c.type = VMI_INSTR_CONST_PROP1_INT8;
	return c;
}

vmp_const vmp_const_ui1(vm_uint8 value)
{
	vmp_const c;
	c.ui1 = value;
	c.type = VMI_INSTR_CONST_PROP1_UINT8;
	return c;
}

vmp_const vmp_const_i2(vm_int16 value)
{
	vmp_const c;
	c.i2 = value;
	c.type = VMI_INSTR_CONST_PROP1_INT16;
	return c;
}

vmp_const vmp_const_ui2(vm_uint16 value)
{
	vmp_const c;
	c.ui2 = value;
	c.type = VMI_INSTR_CONST_PROP1_UINT16;
	return c;
}

vmp_const vmp_const_i4(vm_int32 value)
{
	vmp_const c;
	c.i4 = value;
	c.type = VMI_INSTR_CONST_PROP1_INT32;
	return c;
}

vmp_const vmp_const_ui4(vm_uint32 value)
{
	vmp_const c;
	c.ui4 = value;
	c.type = VMI_INSTR_CONST_PROP1_UINT32;
	return c;
}

vmp_const vmp_const_i8(vm_int64 value)
{
	vmp_const c;
	c.i8 = value;
	c.type = VMI_INSTR_CONST_PROP1_INT64;
	return c;
}

vmp_const vmp_const_ui8(vm_uint64 value)
{
	vmp_const c;
	c.ui8 = value;
	c.type = VMI_INSTR_CONST_PROP1_UINT64;
	return c;
}

vmp_const vmp_const_f4(vm_float32 value)
{
	vmp_const c;
	c.f4 = value;
	c.type = VMI_INSTR_CONST_PROP1_FLOAT32;
	return c;
}

vmp_const vmp_const_f8(vm_float64 value)
{
	vmp_const c;
	c.f8 = value;
	c.type = VMI_INSTR_CONST_PROP1_FLOAT64;
	return c;
}

vmp_const vmp_const_ptr(vm_byte* value)
{
	vmp_const c;
	c.ptr = value;
#ifdef VM_32BIT
	c.type = VMI_INSTR_CONST_PROP1_INT32;
#else
	c.type = VMI_INSTR_CONST_PROP1_INT64;
#endif
	return c;
}

BOOL vmp_const_not_implemented(vmp_const* lhs, const vmp_const* rhs)
{
	assert(false && "Not implemented");
	return FALSE;
}

BOOL vmp_const_not_allowed(vmp_const* lhs, const vmp_const* rhs)
{
	return FALSE;
}

