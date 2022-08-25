#include "const.h"
#include "../interpreter/vmi_ops.h"

vmp_const vmp_const_i1(vm_int8 value)
{
	vmp_const c;
	c.i1 = value;
	c.type = VMI_INSTR_PROP_I1;
	return c;
}

vmp_const vmp_const_ui1(vm_uint8 value)
{
	vmp_const c;
	c.ui1 = value;
	c.type = VMI_INSTR_PROP_UI1;
	return c;
}

vmp_const vmp_const_i2(vm_int16 value)
{
	vmp_const c;
	c.i2 = value;
	c.type = VMI_INSTR_PROP_I2;
	return c;
}

vmp_const vmp_const_ui2(vm_uint16 value)
{
	vmp_const c;
	c.ui2 = value;
	c.type = VMI_INSTR_PROP_UI2;
	return c;
}

vmp_const vmp_const_i4(vm_int32 value)
{
	vmp_const c;
	c.i4 = value;
	c.type = VMI_INSTR_PROP_I4;
	return c;
}

vmp_const vmp_const_ui4(vm_uint32 value)
{
	vmp_const c;
	c.ui4 = value;
	c.type = VMI_INSTR_PROP_UI4;
	return c;
}

vmp_const vmp_const_i8(vm_int64 value)
{
	vmp_const c;
	c.i8 = value;
	c.type = VMI_INSTR_PROP_I8;
	return c;
}

vmp_const vmp_const_ui8(vm_uint64 value)
{
	vmp_const c;
	c.ui8 = value;
	c.type = VMI_INSTR_PROP_UI8;
	return c;
}

vmp_const vmp_const_f4(vm_float32 value)
{
	vmp_const c;
	c.f4 = value;
	c.type = VMI_INSTR_PROP_F4;
	return c;
}

vmp_const vmp_const_f8(vm_float64 value)
{
	vmp_const c;
	c.f8 = value;
	c.type = VMI_INSTR_PROP_F8;
	return c;
}

vmp_const vmp_const_ptr(vm_byte* value)
{
	vmp_const c;
	c.ptr = value;
#ifdef VM_32BIT
	c.type = VMI_INSTR_PROP_I4;
#else
	c.type = VMI_INSTR_PROP_I8;
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

vm_int32 vmp_const_datatype(vm_int32 lhs, vm_int32 rhs)
{
	static const types[VMI_INSTR_PROP_DATA_TYPE_COUNT][VMI_INSTR_PROP_DATA_TYPE_COUNT] = {
		// UNKNOWN
		{ 
			0 
		},
		// BOOL
		{ 
			0, VMI_INSTR_PROP_BOOL
		},
		// INT8
		{ 
			0, VMI_INSTR_PROP_I4, VMI_INSTR_PROP_I4, VMI_INSTR_PROP_I4, VMI_INSTR_PROP_I4, VMI_INSTR_PROP_I4, VMI_INSTR_PROP_I4,
			VMI_INSTR_PROP_UI4, VMI_INSTR_PROP_I8, VMI_INSTR_PROP_UI8, VMI_INSTR_PROP_F4, VMI_INSTR_PROP_F8, VMI_INSTR_PROP_PTR
		},
		// UINT8
		{
			0, VMI_INSTR_PROP_I4, VMI_INSTR_PROP_I4, VMI_INSTR_PROP_I4, VMI_INSTR_PROP_I4, VMI_INSTR_PROP_I4, VMI_INSTR_PROP_I4,
			VMI_INSTR_PROP_UI4, VMI_INSTR_PROP_I8, VMI_INSTR_PROP_UI8, VMI_INSTR_PROP_F4, VMI_INSTR_PROP_F8, VMI_INSTR_PROP_PTR
		},
		// INT16
		{
			0, VMI_INSTR_PROP_I4, VMI_INSTR_PROP_I4, VMI_INSTR_PROP_I4, VMI_INSTR_PROP_I4, VMI_INSTR_PROP_I4, VMI_INSTR_PROP_I4,
			VMI_INSTR_PROP_UI4, VMI_INSTR_PROP_I8, VMI_INSTR_PROP_UI8, VMI_INSTR_PROP_F4, VMI_INSTR_PROP_F8, VMI_INSTR_PROP_PTR
		},
		// UINT16
		{
			0, VMI_INSTR_PROP_I4, VMI_INSTR_PROP_I4, VMI_INSTR_PROP_I4, VMI_INSTR_PROP_I4, VMI_INSTR_PROP_I4, VMI_INSTR_PROP_I4,
			VMI_INSTR_PROP_UI4, VMI_INSTR_PROP_I8, VMI_INSTR_PROP_UI8, VMI_INSTR_PROP_F4, VMI_INSTR_PROP_F8, VMI_INSTR_PROP_PTR
		},
		// INT32
		{
			0, VMI_INSTR_PROP_I4, VMI_INSTR_PROP_I4, VMI_INSTR_PROP_I4, VMI_INSTR_PROP_I4, VMI_INSTR_PROP_I4, VMI_INSTR_PROP_I4,
			VMI_INSTR_PROP_UI4, VMI_INSTR_PROP_I8, VMI_INSTR_PROP_UI8, VMI_INSTR_PROP_F4, VMI_INSTR_PROP_F8, VMI_INSTR_PROP_PTR
		},
		// UINT32
		{
			0, VMI_INSTR_PROP_UI4, VMI_INSTR_PROP_UI4, VMI_INSTR_PROP_UI4, VMI_INSTR_PROP_UI4, VMI_INSTR_PROP_UI4, VMI_INSTR_PROP_UI4,
			VMI_INSTR_PROP_UI4, VMI_INSTR_PROP_I8, VMI_INSTR_PROP_UI8, VMI_INSTR_PROP_F4, VMI_INSTR_PROP_F8, VMI_INSTR_PROP_PTR
		},
		// INT64
		{
			0, VMI_INSTR_PROP_I8, VMI_INSTR_PROP_I8, VMI_INSTR_PROP_I8, VMI_INSTR_PROP_I8, VMI_INSTR_PROP_I8, VMI_INSTR_PROP_I8,
			VMI_INSTR_PROP_I8, VMI_INSTR_PROP_I8, VMI_INSTR_PROP_UI8, VMI_INSTR_PROP_F4, VMI_INSTR_PROP_F8, VMI_INSTR_PROP_PTR
		},
		// UINT64
		{
			0, VMI_INSTR_PROP_UI8, VMI_INSTR_PROP_UI8, VMI_INSTR_PROP_UI8, VMI_INSTR_PROP_UI8, VMI_INSTR_PROP_UI8, VMI_INSTR_PROP_UI8,
			VMI_INSTR_PROP_UI8, VMI_INSTR_PROP_UI8, VMI_INSTR_PROP_UI8, VMI_INSTR_PROP_F4, VMI_INSTR_PROP_F8, VMI_INSTR_PROP_PTR
		},
		// FLOAT32
		{
			0, VMI_INSTR_PROP_F4, VMI_INSTR_PROP_F4, VMI_INSTR_PROP_F4, VMI_INSTR_PROP_F4, VMI_INSTR_PROP_F4, VMI_INSTR_PROP_F4,
			VMI_INSTR_PROP_F4, VMI_INSTR_PROP_F4, VMI_INSTR_PROP_F4, VMI_INSTR_PROP_F4, VMI_INSTR_PROP_F8, VMI_INSTR_PROP_PTR
		},
		// FLOAT64
		{
			0, VMI_INSTR_PROP_F8, VMI_INSTR_PROP_F8, VMI_INSTR_PROP_F8, VMI_INSTR_PROP_F8, VMI_INSTR_PROP_F8, VMI_INSTR_PROP_F8,
			VMI_INSTR_PROP_F8, VMI_INSTR_PROP_F8, VMI_INSTR_PROP_F8, VMI_INSTR_PROP_F8, VMI_INSTR_PROP_F8, VMI_INSTR_PROP_PTR
		},
		// PTR
		{
			0, VMI_INSTR_PROP_PTR, VMI_INSTR_PROP_PTR, VMI_INSTR_PROP_PTR, VMI_INSTR_PROP_PTR, VMI_INSTR_PROP_PTR, VMI_INSTR_PROP_PTR,
			VMI_INSTR_PROP_PTR, VMI_INSTR_PROP_PTR, VMI_INSTR_PROP_PTR, 0, 0, VMI_INSTR_PROP_PTR
		}
	};

	if (lhs <= VMI_INSTR_PROP_UNKNOWN || lhs >= VMI_INSTR_PROP_DATA_TYPE_COUNT)
		return 0;
	if (rhs <= VMI_INSTR_PROP_UNKNOWN || rhs  >= VMI_INSTR_PROP_DATA_TYPE_COUNT)
		return 0;
	return types[lhs][rhs];
}
