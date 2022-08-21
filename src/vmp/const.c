#include "const.h"
#include "../interpreter/vmi_ops.h"

vmp_constant vmp_const_i1(vm_int8 value)
{
	vmp_constant c;
	c.i1 = value;
	c.type = VMI_INSTR_CONST_PROP1_INT8;
	return c;
}

vmp_constant vmp_const_ui1(vm_uint8 value)
{
	vmp_constant c;
	c.ui1 = value;
	c.type = VMI_INSTR_CONST_PROP1_UINT8;
	return c;
}

vmp_constant vmp_const_i2(vm_int16 value)
{
	vmp_constant c;
	c.i2 = value;
	c.type = VMI_INSTR_CONST_PROP1_INT16;
	return c;
}

vmp_constant vmp_const_ui2(vm_uint16 value)
{
	vmp_constant c;
	c.ui2 = value;
	c.type = VMI_INSTR_CONST_PROP1_UINT16;
	return c;
}

vmp_constant vmp_const_i4(vm_int32 value)
{
	vmp_constant c;
	c.i4 = value;
	c.type = VMI_INSTR_CONST_PROP1_INT32;
	return c;
}

vmp_constant vmp_const_ui4(vm_uint32 value)
{
	vmp_constant c;
	c.ui4 = value;
	c.type = VMI_INSTR_CONST_PROP1_UINT32;
	return c;
}

vmp_constant vmp_const_i8(vm_int64 value)
{
	vmp_constant c;
	c.i8 = value;
	c.type = VMI_INSTR_CONST_PROP1_INT64;
	return c;
}

vmp_constant vmp_const_ui8(vm_uint64 value)
{
	vmp_constant c;
	c.ui8 = value;
	c.type = VMI_INSTR_CONST_PROP1_UINT64;
	return c;
}

vmp_constant vmp_const_f4(vm_float32 value)
{
	vmp_constant c;
	c.f4 = value;
	c.type = VMI_INSTR_CONST_PROP1_FLOAT32;
	return c;
}

vmp_constant vmp_const_f8(vm_float64 value)
{
	vmp_constant c;
	c.f8 = value;
	c.type = VMI_INSTR_CONST_PROP1_FLOAT64;
	return c;
}

vmp_constant vmp_const_ptr(vm_byte* value)
{
	vmp_constant c;
	c.ptr = value;
#ifdef VM_32BIT
	c.type = VMI_INSTR_CONST_PROP1_INT32;
#else
	c.type = VMI_INSTR_CONST_PROP1_INT64;
#endif
	return c;
}

void vmp_const_add_not_implemented(const vmp_constant* lhs, const vmp_constant* rhs, vmp_constant* result)
{
	assert(false && "Not implemented");
}

void vmp_const_add_i1_bool(const vmp_constant* lhs, const vmp_constant* rhs, vmp_constant* result)
{
	result->i1 = lhs->i1 + (vm_int8)rhs->i4;
}

void vmp_const_add_i1_i1(const vmp_constant* lhs, const vmp_constant* rhs, vmp_constant* result)
{
	result->i1 = lhs->i1 + rhs->i1;
}

void vmp_const_add_i1_ui1(const vmp_constant* lhs, const vmp_constant* rhs, vmp_constant* result)
{
	result->i1 = lhs->i1 + rhs->i1;
	result->type = rhs->type;
}

void vmp_const_add_i1_i2(const vmp_constant* lhs, const vmp_constant* rhs, vmp_constant* result)
{
	result->i2 = lhs->i1 + rhs->i2;
	result->type = rhs->type;
}

void vmp_const_add_i1_ui2(const vmp_constant* lhs, const vmp_constant* rhs, vmp_constant* result)
{
	result->i2 = lhs->i1 + rhs->ui2;
	result->type = rhs->type;
}

void vmp_const_add_i1_i4(const vmp_constant* lhs, const vmp_constant* rhs, vmp_constant* result)
{
	result->i4 = lhs->i1 + rhs->i4;
	result->type = rhs->type;
}

void vmp_const_add_i1_ui4(const vmp_constant* lhs, const vmp_constant* rhs, vmp_constant* result)
{
	result->i4 = lhs->i1 + rhs->ui4;
	result->type = rhs->type;
}

void vmp_const_add_i1_i8(const vmp_constant* lhs, const vmp_constant* rhs, vmp_constant* result)
{
	result->i8 = lhs->i1 + rhs->i8;
	result->type = rhs->type;
}

void vmp_const_add_i1_ui8(const vmp_constant* lhs, const vmp_constant* rhs, vmp_constant* result)
{
	result->i8 = lhs->i1 + rhs->i8;
	result->type = rhs->type;
}

void vmp_const_add_i1_f4(const vmp_constant* lhs, const vmp_constant* rhs, vmp_constant* result)
{
	result->f4 = lhs->i1 + rhs->f4;
	result->type = rhs->type;
}

void vmp_const_add_i1_f8(const vmp_constant* lhs, const vmp_constant* rhs, vmp_constant* result)
{
	result->f8 = lhs->i1 + rhs->f8;
	result->type = rhs->type;
}

void vmp_const_add_i1_ptr(const vmp_constant* lhs, const vmp_constant* rhs, vmp_constant* result)
{
	result->ptr = (size_t)lhs->i1 + rhs->ptr;
	result->type = rhs->type;
}

const vmp_const_combine_fn vmp_const_add_functions[VMI_INSTR_PROP_DATA_TYPE_COUNT][VMI_INSTR_PROP_DATA_TYPE_COUNT] = {
	// BOOL
	{
		vmp_const_add_not_implemented, 
		vmp_const_add_not_implemented, vmp_const_add_not_implemented,
		vmp_const_add_not_implemented, vmp_const_add_not_implemented, 
		vmp_const_add_not_implemented, vmp_const_add_not_implemented, 
		vmp_const_add_not_implemented, vmp_const_add_not_implemented,
		vmp_const_add_not_implemented, vmp_const_add_not_implemented,
		vmp_const_add_not_implemented
	},
	// INT8
	{ 
		vmp_const_add_i1_bool, 
		vmp_const_add_i1_i1, vmp_const_add_i1_ui1, 
		vmp_const_add_i1_i2, vmp_const_add_i1_ui2, 
		vmp_const_add_i1_i4, vmp_const_add_i1_ui4,
		vmp_const_add_i1_i8, vmp_const_add_i1_ui8,
		vmp_const_add_i1_f4, vmp_const_add_i1_f8,
		vmp_const_add_i1_ptr
	},
};

void vmp_const_add(const vmp_constant* lhs, const vmp_constant* rhs, vmp_constant* result)
{
	assert(lhs->type >= VMI_INSTR_PROP_BOOL && lhs->type < VMI_INSTR_PROP_DATA_TYPE_COUNT);
	assert(rhs->type >= VMI_INSTR_PROP_BOOL && rhs->type < VMI_INSTR_PROP_DATA_TYPE_COUNT);
	vmp_const_add_functions[lhs->type][rhs->type](lhs, rhs, result);
}

const vmp_const_combine_fn vmp_const_sub_functions[VMI_INSTR_PROP_DATA_TYPE_COUNT][VMI_INSTR_PROP_DATA_TYPE_COUNT] = {
	NULL
};

void vmp_const_sub(const vmp_constant* lhs, const vmp_constant* rhs, vmp_constant* result)
{
	assert(lhs->type >= VMI_INSTR_PROP_BOOL && lhs->type < VMI_INSTR_PROP_DATA_TYPE_COUNT);
	assert(rhs->type >= VMI_INSTR_PROP_BOOL && rhs->type < VMI_INSTR_PROP_DATA_TYPE_COUNT);
	vmp_const_sub_functions[lhs->type][rhs->type](lhs, rhs, result);
}

const vmp_const_combine_fn vmp_const_mult_functions[VMI_INSTR_PROP_DATA_TYPE_COUNT][VMI_INSTR_PROP_DATA_TYPE_COUNT] = {
	NULL
};

void vmp_const_mult(const vmp_constant* lhs, const vmp_constant* rhs, vmp_constant* result)
{
	assert(lhs->type >= VMI_INSTR_PROP_BOOL && lhs->type < VMI_INSTR_PROP_DATA_TYPE_COUNT);
	assert(rhs->type >= VMI_INSTR_PROP_BOOL && rhs->type < VMI_INSTR_PROP_DATA_TYPE_COUNT);
	vmp_const_mult_functions[lhs->type][rhs->type](lhs, rhs, result);
}

const vmp_const_combine_fn vmp_const_div_functions[VMI_INSTR_PROP_DATA_TYPE_COUNT][VMI_INSTR_PROP_DATA_TYPE_COUNT] = {
	NULL
};

void vmp_const_div(const vmp_constant* lhs, const vmp_constant* rhs, vmp_constant* result)
{
	assert(lhs->type >= VMI_INSTR_PROP_BOOL && lhs->type < VMI_INSTR_PROP_DATA_TYPE_COUNT);
	assert(rhs->type >= VMI_INSTR_PROP_BOOL && rhs->type < VMI_INSTR_PROP_DATA_TYPE_COUNT);
	vmp_const_div_functions[lhs->type][rhs->type](lhs, rhs, result);
}
