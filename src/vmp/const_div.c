#include "const.h"
#include "../interpreter/vmi_ops.h"

////////////////////// 
//////////// INT8
/////////////////////

BOOL vmp_const_div_i1_bool(vmp_const* lhs, const vmp_const* rhs)
{
	return vmp_const_not_allowed(lhs, rhs);
}

BOOL vmp_const_div_i1_i1(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i4 = lhs->i1 / rhs->i1;
	return TRUE;
}

BOOL vmp_const_div_i1_ui1(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i4 = lhs->i1 / rhs->ui1;
	return TRUE;
}

BOOL vmp_const_div_i1_i2(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i4 = lhs->i1 / rhs->i2;
	return TRUE;
}

BOOL vmp_const_div_i1_ui2(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i4 = lhs->i1 / rhs->ui2;
	return TRUE;
}

BOOL vmp_const_div_i1_i4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i4 = lhs->i1 / rhs->i4;
	return TRUE;
}

BOOL vmp_const_div_i1_ui4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui4 = lhs->i1 / rhs->ui4;
	return TRUE;
}

BOOL vmp_const_div_i1_i8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i8 = lhs->i1 / rhs->i8;
	return TRUE;
}

BOOL vmp_const_div_i1_ui8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui8 = lhs->i1 / rhs->ui8;
	return TRUE;
}

BOOL vmp_const_div_i1_f4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f4 = lhs->i1 / rhs->f4;
	return TRUE;
}

BOOL vmp_const_div_i1_f8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f8 = lhs->i1 / rhs->f8;
	return TRUE;
}

BOOL vmp_const_div_i1_ptr(vmp_const* lhs, const vmp_const* rhs)
{
	return vmp_const_not_allowed(lhs, rhs);
}

////////////////////// 
//////////// UNSIGNED INT8
/////////////////////

BOOL vmp_const_div_ui1_bool(vmp_const* lhs, const vmp_const* rhs)
{
	return vmp_const_not_allowed(lhs, rhs);
}

BOOL vmp_const_div_ui1_i1(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i4 = lhs->ui1 / rhs->i1;
	return TRUE;
}

BOOL vmp_const_div_ui1_ui1(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i4 = lhs->ui1 / rhs->ui1;
	return TRUE;
}

BOOL vmp_const_div_ui1_i2(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i4 = lhs->ui1 / rhs->i2;
	return TRUE;
}

BOOL vmp_const_div_ui1_ui2(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i4 = lhs->ui1 / rhs->ui2;
	return TRUE;
}

BOOL vmp_const_div_ui1_i4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i4 = lhs->ui1 / rhs->i4;
	return TRUE;
}

BOOL vmp_const_div_ui1_ui4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui4 = lhs->ui1 / rhs->ui4;
	return TRUE;
}

BOOL vmp_const_div_ui1_i8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i8 = lhs->ui1 / rhs->i8;
	return TRUE;
}

BOOL vmp_const_div_ui1_ui8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui8 = lhs->ui1 / rhs->ui8;
	return TRUE;
}

BOOL vmp_const_div_ui1_f4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f4 = lhs->ui1 / rhs->f4;
	return TRUE;
}

BOOL vmp_const_div_ui1_f8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f8 = lhs->ui1 / rhs->f8;
	return TRUE;
}

BOOL vmp_const_div_ui1_ptr(vmp_const* lhs, const vmp_const* rhs)
{
	return vmp_const_not_allowed(lhs, rhs);
}

////////////////////// 
//////////// INT16
/////////////////////

BOOL vmp_const_div_i2_bool(vmp_const* lhs, const vmp_const* rhs)
{
	return vmp_const_not_allowed(lhs, rhs);
}

BOOL vmp_const_div_i2_i1(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i2 = lhs->i2 / rhs->i1;
	return TRUE;
}

BOOL vmp_const_div_i2_ui1(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i2 = lhs->i2 / rhs->ui1;
	return TRUE;
}

BOOL vmp_const_div_i2_i2(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i2 = lhs->i2 / rhs->i2;
	return TRUE;
}

BOOL vmp_const_div_i2_ui2(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui2 = lhs->i2 / rhs->ui2;
	return TRUE;
}

BOOL vmp_const_div_i2_i4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i4 = lhs->i2 / rhs->i4;
	return TRUE;
}

BOOL vmp_const_div_i2_ui4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui4 = lhs->i2 / rhs->ui4;
	return TRUE;
}

BOOL vmp_const_div_i2_i8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i8 = lhs->i2 / rhs->i8;
	return TRUE;
}

BOOL vmp_const_div_i2_ui8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui8 = lhs->i2 / rhs->ui8;
	return TRUE;
}

BOOL vmp_const_div_i2_f4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f4 = lhs->i2 / rhs->f4;
	return TRUE;
}

BOOL vmp_const_div_i2_f8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f8 = lhs->i2 / rhs->f8;
	return TRUE;
}

BOOL vmp_const_div_i2_ptr(vmp_const* lhs, const vmp_const* rhs)
{
	return vmp_const_not_allowed(lhs, rhs);
}

////////////////////// 
//////////// UNSIGNED INT16
/////////////////////

BOOL vmp_const_div_ui2_bool(vmp_const* lhs, const vmp_const* rhs)
{
	return vmp_const_not_allowed(lhs, rhs);
}

BOOL vmp_const_div_ui2_i1(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui2 = lhs->ui2 / rhs->i1;
	return TRUE;
}

BOOL vmp_const_div_ui2_ui1(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui2 = lhs->ui2 / rhs->ui1;
	return TRUE;
}

BOOL vmp_const_div_ui2_i2(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i2 = lhs->ui2 / rhs->i2;
	return TRUE;
}

BOOL vmp_const_div_ui2_ui2(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui2 = lhs->ui2 / rhs->ui2;
	return TRUE;
}

BOOL vmp_const_div_ui2_i4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i4 = lhs->ui2 / rhs->i4;
	return TRUE;
}

BOOL vmp_const_div_ui2_ui4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui4 = lhs->ui2 / rhs->ui4;
	return TRUE;
}

BOOL vmp_const_div_ui2_i8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i8 = lhs->ui2 / rhs->i8;
	return TRUE;
}

BOOL vmp_const_div_ui2_ui8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui8 = lhs->ui2 / rhs->ui8;
	return TRUE;
}

BOOL vmp_const_div_ui2_f4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f4 = lhs->ui2 / rhs->f4;
	return TRUE;
}

BOOL vmp_const_div_ui2_f8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f8 = lhs->ui2 / rhs->f8;
	return TRUE;
}

BOOL vmp_const_div_ui2_ptr(vmp_const* lhs, const vmp_const* rhs)
{
	return vmp_const_not_allowed(lhs, rhs);
}

////////////////////// 
//////////// INT32
/////////////////////

BOOL vmp_const_div_i4_bool(vmp_const* lhs, const vmp_const* rhs)
{
	return vmp_const_not_allowed(lhs, rhs);
}

BOOL vmp_const_div_i4_i1(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i4 = lhs->i4 / rhs->i1;
	return TRUE;
}

BOOL vmp_const_div_i4_ui1(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i4 = lhs->i4 / rhs->ui1;
	return TRUE;
}

BOOL vmp_const_div_i4_i2(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i4 = lhs->i4 / rhs->i2;
	return TRUE;
}

BOOL vmp_const_div_i4_ui2(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i4 = lhs->i4 / rhs->ui2;
	return TRUE;
}

BOOL vmp_const_div_i4_i4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i4 = lhs->i4 / rhs->i4;
	return TRUE;
}

BOOL vmp_const_div_i4_ui4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui4 = lhs->i4 / rhs->ui4;
	return TRUE;
}

BOOL vmp_const_div_i4_i8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i8 = lhs->i4 / rhs->i8;
	return TRUE;
}

BOOL vmp_const_div_i4_ui8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui8 = lhs->i4 / rhs->ui8;
	return TRUE;
}

BOOL vmp_const_div_i4_f4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f4 = lhs->i4 / rhs->f4;
	return TRUE;
}

BOOL vmp_const_div_i4_f8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f8 = lhs->i4 / rhs->f8;
	return TRUE;
}

BOOL vmp_const_div_i4_ptr(vmp_const* lhs, const vmp_const* rhs)
{
	return vmp_const_not_allowed(lhs, rhs);
}

////////////////////// 
//////////// UNSIGNED INT32
/////////////////////

BOOL vmp_const_div_ui4_bool(vmp_const* lhs, const vmp_const* rhs)
{
	return vmp_const_not_allowed(lhs, rhs);
}

BOOL vmp_const_div_ui4_i1(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui4 = lhs->ui4 / rhs->i1;
	return TRUE;
}

BOOL vmp_const_div_ui4_ui1(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui4 = lhs->ui4 / rhs->ui1;
	return TRUE;
}

BOOL vmp_const_div_ui4_i2(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui4 = lhs->ui4 / rhs->i2;
	return TRUE;
}

BOOL vmp_const_div_ui4_ui2(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui4 = lhs->ui4 / rhs->ui2;
	return TRUE;
}

BOOL vmp_const_div_ui4_i4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i4 = lhs->ui4 / rhs->i4;
	return TRUE;
}

BOOL vmp_const_div_ui4_ui4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui4 = lhs->ui4 / rhs->ui4;
	return TRUE;
}

BOOL vmp_const_div_ui4_i8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i8 = lhs->ui4 / rhs->i8;
	return TRUE;
}

BOOL vmp_const_div_ui4_ui8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui8 = lhs->ui4 / rhs->ui8;
	return TRUE;
}

BOOL vmp_const_div_ui4_f4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f4 = lhs->ui4 / rhs->f4;
	return TRUE;
}

BOOL vmp_const_div_ui4_f8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f8 = lhs->ui4 / rhs->f8;
	return TRUE;
}

BOOL vmp_const_div_ui4_ptr(vmp_const* lhs, const vmp_const* rhs)
{
	return vmp_const_not_allowed(lhs, rhs);
}

////////////////////// 
//////////// INT64
/////////////////////

BOOL vmp_const_div_i8_bool(vmp_const* lhs, const vmp_const* rhs)
{
	return vmp_const_not_allowed(lhs, rhs);
}

BOOL vmp_const_div_i8_i1(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i8 = lhs->i8 / rhs->i1;
	return TRUE;
}

BOOL vmp_const_div_i8_ui1(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i8 = lhs->i8 / rhs->ui1;
	return TRUE;
}

BOOL vmp_const_div_i8_i2(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i8 = lhs->i8 / rhs->i2;
	return TRUE;
}

BOOL vmp_const_div_i8_ui2(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i8 = lhs->i8 / rhs->ui2;
	return TRUE;
}

BOOL vmp_const_div_i8_i4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i8 = lhs->i8 / rhs->i4;
	return TRUE;
}

BOOL vmp_const_div_i8_ui4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i8 = lhs->i8 / rhs->ui4;
	return TRUE;
}

BOOL vmp_const_div_i8_i8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i8 = lhs->i8 / rhs->i8;
	return TRUE;
}

BOOL vmp_const_div_i8_ui8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui8 = lhs->i8 / rhs->ui8;
	return TRUE;
}

BOOL vmp_const_div_i8_f4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f4 = lhs->i8 / rhs->f4;
	return TRUE;
}

BOOL vmp_const_div_i8_f8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f8 = lhs->i8 / rhs->f8;
	return TRUE;
}

BOOL vmp_const_div_i8_ptr(vmp_const* lhs, const vmp_const* rhs)
{
	return vmp_const_not_allowed(lhs, rhs);
}

////////////////////// 
//////////// INT64
/////////////////////

BOOL vmp_const_div_ui8_bool(vmp_const* lhs, const vmp_const* rhs)
{
	return vmp_const_not_allowed(lhs, rhs);
}

BOOL vmp_const_div_ui8_i1(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui8 = lhs->ui8 / rhs->i1;
	return TRUE;
}

BOOL vmp_const_div_ui8_ui1(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui8 = lhs->ui8 / rhs->ui1;
	return TRUE;
}

BOOL vmp_const_div_ui8_i2(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui8 = lhs->ui8 / rhs->i2;
	return TRUE;
}

BOOL vmp_const_div_ui8_ui2(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui8 = lhs->ui8 / rhs->ui2;
	return TRUE;
}

BOOL vmp_const_div_ui8_i4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui8 = lhs->ui8 / rhs->i4;
	return TRUE;
}

BOOL vmp_const_div_ui8_ui4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui8 = lhs->ui8 / rhs->ui4;
	return TRUE;
}

BOOL vmp_const_div_ui8_i8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui8 = lhs->ui8 / rhs->i8;
	return TRUE;
}

BOOL vmp_const_div_ui8_ui8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui8 = lhs->ui8 / rhs->ui8;
	return TRUE;
}

BOOL vmp_const_div_ui8_f4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f4 = lhs->ui8 / rhs->f4;
	return TRUE;
}

BOOL vmp_const_div_ui8_f8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f8 = lhs->ui8 / rhs->f8;
	return TRUE;
}

BOOL vmp_const_div_ui8_ptr(vmp_const* lhs, const vmp_const* rhs)
{
	return vmp_const_not_allowed(lhs, rhs);
}

////////////////////// 
//////////// FLOAT32
/////////////////////

BOOL vmp_const_div_f4_bool(vmp_const* lhs, const vmp_const* rhs)
{
	return vmp_const_not_allowed(lhs, rhs);
}

BOOL vmp_const_div_f4_i1(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f4 = lhs->f4 / rhs->i1;
	return TRUE;
}

BOOL vmp_const_div_f4_ui1(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f4 = lhs->f4 / rhs->ui1;
	return TRUE;
}

BOOL vmp_const_div_f4_i2(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f4 = lhs->f4 / rhs->i2;
	return TRUE;
}

BOOL vmp_const_div_f4_ui2(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f4 = lhs->f4 / rhs->ui2;
	return TRUE;
}

BOOL vmp_const_div_f4_i4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f4 = lhs->f4 / rhs->i4;
	return TRUE;
}

BOOL vmp_const_div_f4_ui4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f4 = lhs->f4 / rhs->ui4;
	return TRUE;
}

BOOL vmp_const_div_f4_i8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f4 = lhs->f4 / rhs->i8;
	return TRUE;
}

BOOL vmp_const_div_f4_ui8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f4 = lhs->f4 / rhs->ui8;
	return TRUE;
}

BOOL vmp_const_div_f4_f4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f4 = lhs->f4 / rhs->f4;
	return TRUE;
}

BOOL vmp_const_div_f4_f8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f8 = lhs->f4 / rhs->f8;
	return TRUE;
}

BOOL vmp_const_div_f4_ptr(vmp_const* lhs, const vmp_const* rhs)
{
	return vmp_const_not_allowed(lhs, rhs);
}

////////////////////// 
//////////// FLOAT64
/////////////////////

BOOL vmp_const_div_f8_bool(vmp_const* lhs, const vmp_const* rhs)
{
	return vmp_const_not_allowed(lhs, rhs);
}

BOOL vmp_const_div_f8_i1(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f8 = lhs->f8 / rhs->i1;
	return TRUE;
}

BOOL vmp_const_div_f8_ui1(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f8 = lhs->f8 / rhs->ui1;
	return TRUE;
}

BOOL vmp_const_div_f8_i2(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f8 = lhs->f8 / rhs->i2;
	return TRUE;
}

BOOL vmp_const_div_f8_ui2(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f8 = lhs->f8 / rhs->ui2;
	return TRUE;
}

BOOL vmp_const_div_f8_i4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f8 = lhs->f8 / rhs->i4;
	return TRUE;
}

BOOL vmp_const_div_f8_ui4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f8 = lhs->f8 / rhs->ui4;
	return TRUE;
}

BOOL vmp_const_div_f8_i8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f8 = lhs->f8 / rhs->i8;
	return TRUE;
}

BOOL vmp_const_div_f8_ui8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f8 = lhs->f8 / rhs->ui8;
	return TRUE;
}

BOOL vmp_const_div_f8_f4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f8 = lhs->f8 / rhs->f4;
	return TRUE;
}

BOOL vmp_const_div_f8_f8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f8 = lhs->f8 / rhs->f8;
	return TRUE;
}

BOOL vmp_const_div_f8_ptr(vmp_const* lhs, const vmp_const* rhs)
{
	return vmp_const_not_allowed(lhs, rhs);
}

BOOL vmp_const_div(vmp_const* lhs, const vmp_const* rhs)
{
	static const vmp_const_combine_fn functions[VMI_INSTR_PROP_DATA_TYPE_COUNT][VMI_INSTR_PROP_DATA_TYPE_COUNT] = {
		// UNKNOWN
		{
			vmp_const_not_allowed,
			vmp_const_not_allowed,
			vmp_const_not_allowed, vmp_const_not_allowed,
			vmp_const_not_allowed, vmp_const_not_allowed,
			vmp_const_not_allowed, vmp_const_not_allowed,
			vmp_const_not_allowed, vmp_const_not_allowed,
			vmp_const_not_allowed, vmp_const_not_allowed,
			vmp_const_not_allowed
		},
		// BOOL
		{
			vmp_const_not_allowed,
			vmp_const_not_allowed,
			vmp_const_not_allowed, vmp_const_not_allowed,
			vmp_const_not_allowed, vmp_const_not_allowed,
			vmp_const_not_allowed, vmp_const_not_allowed,
			vmp_const_not_allowed, vmp_const_not_allowed,
			vmp_const_not_allowed, vmp_const_not_allowed,
			vmp_const_not_allowed
		},
		// INT8
		{
			vmp_const_not_allowed,
			vmp_const_div_i1_bool,
			vmp_const_div_i1_i1, vmp_const_div_i1_ui1,
			vmp_const_div_i1_i2, vmp_const_div_i1_ui2,
			vmp_const_div_i1_i4, vmp_const_div_i1_ui4,
			vmp_const_div_i1_i8, vmp_const_div_i1_ui8,
			vmp_const_div_i1_f4, vmp_const_div_i1_f8,
			vmp_const_div_i1_ptr
		},
		// UINT8
		{
			vmp_const_not_allowed,
			vmp_const_div_ui1_bool,
			vmp_const_div_ui1_i1, vmp_const_div_ui1_ui1,
			vmp_const_div_ui1_i2, vmp_const_div_ui1_ui2,
			vmp_const_div_ui1_i4, vmp_const_div_ui1_ui4,
			vmp_const_div_ui1_i8, vmp_const_div_ui1_ui8,
			vmp_const_div_ui1_f4, vmp_const_div_ui1_f8,
			vmp_const_div_ui1_ptr
		},
		// INT16
		{
			vmp_const_not_allowed,
			vmp_const_div_i2_bool,
			vmp_const_div_i2_i1, vmp_const_div_i2_ui1,
			vmp_const_div_i2_i2, vmp_const_div_i2_ui2,
			vmp_const_div_i2_i4, vmp_const_div_i2_ui4,
			vmp_const_div_i2_i8, vmp_const_div_i2_ui8,
			vmp_const_div_i2_f4, vmp_const_div_i2_f8,
			vmp_const_div_i2_ptr
		},
		// UINT16
		{
			vmp_const_not_allowed,
			vmp_const_div_ui2_bool,
			vmp_const_div_ui2_i1, vmp_const_div_ui2_ui1,
			vmp_const_div_ui2_i2, vmp_const_div_ui2_ui2,
			vmp_const_div_ui2_i4, vmp_const_div_ui2_ui4,
			vmp_const_div_ui2_i8, vmp_const_div_ui2_ui8,
			vmp_const_div_ui2_f4, vmp_const_div_ui2_f8,
			vmp_const_div_ui2_ptr
		},
		// INT32
		{
			vmp_const_not_allowed,
			vmp_const_div_i4_bool,
			vmp_const_div_i4_i1, vmp_const_div_i4_ui1,
			vmp_const_div_i4_i2, vmp_const_div_i4_ui2,
			vmp_const_div_i4_i4, vmp_const_div_i4_ui4,
			vmp_const_div_i4_i8, vmp_const_div_i4_ui8,
			vmp_const_div_i4_f4, vmp_const_div_i4_f8,
			vmp_const_div_i4_ptr
		},
		// UINT32
		{
			vmp_const_not_allowed,
			vmp_const_div_ui4_bool,
			vmp_const_div_ui4_i1, vmp_const_div_ui4_ui1,
			vmp_const_div_ui4_i2, vmp_const_div_ui4_ui2,
			vmp_const_div_ui4_i4, vmp_const_div_ui4_ui4,
			vmp_const_div_ui4_i8, vmp_const_div_ui4_ui8,
			vmp_const_div_ui4_f4, vmp_const_div_ui4_f8,
			vmp_const_div_ui4_ptr
		},
		// INT64
		{
			vmp_const_not_allowed,
			vmp_const_div_i8_bool,
			vmp_const_div_i8_i1, vmp_const_div_i8_ui1,
			vmp_const_div_i8_i2, vmp_const_div_i8_ui2,
			vmp_const_div_i8_i4, vmp_const_div_i8_ui4,
			vmp_const_div_i8_i8, vmp_const_div_i8_ui8,
			vmp_const_div_i8_f4, vmp_const_div_i8_f8,
			vmp_const_div_i8_ptr
		},
		// UINT64
		{
			vmp_const_not_allowed,
			vmp_const_div_ui8_bool,
			vmp_const_div_ui8_i1, vmp_const_div_ui8_ui1,
			vmp_const_div_ui8_i2, vmp_const_div_ui8_ui2,
			vmp_const_div_ui8_i4, vmp_const_div_ui8_ui4,
			vmp_const_div_ui8_i8, vmp_const_div_ui8_ui8,
			vmp_const_div_ui8_f4, vmp_const_div_ui8_f8,
			vmp_const_div_ui8_ptr
		},
		// FLOAT32
		{
			vmp_const_not_allowed,
			vmp_const_div_f4_bool,
			vmp_const_div_f4_i1, vmp_const_div_f4_ui1,
			vmp_const_div_f4_i2, vmp_const_div_f4_ui2,
			vmp_const_div_f4_i4, vmp_const_div_f4_ui4,
			vmp_const_div_f4_i8, vmp_const_div_f4_ui8,
			vmp_const_div_f4_f4, vmp_const_div_f4_f8,
			vmp_const_div_f4_ptr
		},
		// FLOAT64
		{
			vmp_const_not_allowed,
			vmp_const_div_f8_bool,
			vmp_const_div_f8_i1, vmp_const_div_f8_ui1,
			vmp_const_div_f8_i2, vmp_const_div_f8_ui2,
			vmp_const_div_f8_i4, vmp_const_div_f8_ui4,
			vmp_const_div_f8_i8, vmp_const_div_f8_ui8,
			vmp_const_div_f8_f4, vmp_const_div_f8_f8,
			vmp_const_div_f8_ptr
		},
		// PTR
		{
			vmp_const_not_implemented,
			vmp_const_not_implemented,
			vmp_const_not_implemented, vmp_const_not_implemented,
			vmp_const_not_implemented, vmp_const_not_implemented,
			vmp_const_not_implemented, vmp_const_not_implemented,
			vmp_const_not_implemented, vmp_const_not_implemented,
			vmp_const_not_implemented, vmp_const_not_implemented,
			vmp_const_not_implemented
		}
	};

	if (lhs->type <= VMI_INSTR_PROP_UNKNOWN || lhs->type >= VMI_INSTR_PROP_DATA_TYPE_COUNT)
		return FALSE;
	if (rhs->type <= VMI_INSTR_PROP_UNKNOWN || rhs->type >= VMI_INSTR_PROP_DATA_TYPE_COUNT)
		return FALSE;
	if (!functions[lhs->type][rhs->type](lhs, rhs))
		return FALSE;
	lhs->type = vmp_const_datatype(lhs->type, rhs->type);
	return TRUE;
}
