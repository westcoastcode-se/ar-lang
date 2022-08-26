#include "const.h"
#include "../interpreter/vmi_ops.h"

////////////////////// 
//////////// INT8
/////////////////////

BOOL vmp_const_add_i1_bool(vmp_const* lhs, const vmp_const* rhs)
{
	return vmp_const_not_allowed(lhs, rhs);
}

BOOL vmp_const_add_i1_i1(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i32 = lhs->i8 + rhs->i8;
	return TRUE;
}

BOOL vmp_const_add_i1_ui1(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i32 = lhs->i8 + rhs->ui8;
	return TRUE;
}

BOOL vmp_const_add_i1_i2(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i32 = lhs->i8 + rhs->i16;
	return TRUE;
}

BOOL vmp_const_add_i1_ui2(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i32 = lhs->i8 + rhs->ui16;
	return TRUE;
}

BOOL vmp_const_add_i1_i4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i32 = lhs->i8 + rhs->i32;
	return TRUE;
}

BOOL vmp_const_add_i1_ui4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui32 = lhs->i8 + rhs->ui32;
	return TRUE;
}

BOOL vmp_const_add_i1_i8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i64 = lhs->i8 + rhs->i64;
	return TRUE;
}

BOOL vmp_const_add_i1_ui8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui64 = lhs->i8 + rhs->ui64;
	return TRUE;
}

BOOL vmp_const_add_i1_f4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f32 = lhs->i8 + rhs->f32;
	return TRUE;
}

BOOL vmp_const_add_i1_f8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f64 = lhs->i8 + rhs->f64;
	return TRUE;
}

BOOL vmp_const_add_i1_ptr(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ptr = (size_t)lhs->i8 + rhs->ptr;
	return TRUE;
}

////////////////////// 
//////////// UNSIGNED INT8
/////////////////////

BOOL vmp_const_add_ui1_bool(vmp_const* lhs, const vmp_const* rhs)
{
	return vmp_const_not_allowed(lhs, rhs);
}

BOOL vmp_const_add_ui1_i1(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i32 = lhs->ui8 + rhs->i8;
	return TRUE;
}

BOOL vmp_const_add_ui1_ui1(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i32 = lhs->ui8 + rhs->ui8;
	return TRUE;
}

BOOL vmp_const_add_ui1_i2(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i32 = lhs->ui8 + rhs->i16;
	return TRUE;
}

BOOL vmp_const_add_ui1_ui2(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i32 = lhs->ui8 + rhs->ui16;
	return TRUE;
}

BOOL vmp_const_add_ui1_i4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i32 = lhs->ui8 + rhs->i32;
	return TRUE;
}

BOOL vmp_const_add_ui1_ui4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui32 = lhs->ui8 + rhs->ui32;
	return TRUE;
}

BOOL vmp_const_add_ui1_i8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i64 = lhs->ui8 + rhs->i64;
	return TRUE;
}

BOOL vmp_const_add_ui1_ui8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui64 = lhs->ui8 + rhs->ui64;
	return TRUE;
}

BOOL vmp_const_add_ui1_f4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f32 = lhs->ui8 + rhs->f32;
	return TRUE;
}

BOOL vmp_const_add_ui1_f8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f64 = lhs->ui8 + rhs->f64;
	return TRUE;
}

BOOL vmp_const_add_ui1_ptr(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ptr = (size_t)lhs->ui8 + rhs->ptr;
	return TRUE;
}

////////////////////// 
//////////// INT16
/////////////////////

BOOL vmp_const_add_i2_bool(vmp_const* lhs, const vmp_const* rhs)
{
	return vmp_const_not_allowed(lhs, rhs);
}

BOOL vmp_const_add_i2_i1(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i16 = lhs->i16 + rhs->i8;
	return TRUE;
}

BOOL vmp_const_add_i2_ui1(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i16 = lhs->i16 + rhs->ui8;
	return TRUE;
}

BOOL vmp_const_add_i2_i2(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i16 = lhs->i16 + rhs->i16;
	return TRUE;
}

BOOL vmp_const_add_i2_ui2(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui16 = lhs->i16 + rhs->ui16;
	return TRUE;
}

BOOL vmp_const_add_i2_i4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i32 = lhs->i16 + rhs->i32;
	return TRUE;
}

BOOL vmp_const_add_i2_ui4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui32 = lhs->i16 + rhs->ui32;
	return TRUE;
}

BOOL vmp_const_add_i2_i8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i64 = lhs->i16 + rhs->i64;
	return TRUE;
}

BOOL vmp_const_add_i2_ui8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui64 = lhs->i16 + rhs->ui64;
	return TRUE;
}

BOOL vmp_const_add_i2_f4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f32 = lhs->i16 + rhs->f32;
	return TRUE;
}

BOOL vmp_const_add_i2_f8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f64 = lhs->i16 + rhs->f64;
	return TRUE;
}

BOOL vmp_const_add_i2_ptr(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ptr = (size_t)lhs->i16 + rhs->ptr;
	return TRUE;
}

////////////////////// 
//////////// UNSIGNED INT16
/////////////////////

BOOL vmp_const_add_ui2_bool(vmp_const* lhs, const vmp_const* rhs)
{
	return vmp_const_not_allowed(lhs, rhs);
}

BOOL vmp_const_add_ui2_i1(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui16 = lhs->ui16 + rhs->i8;
	return TRUE;
}

BOOL vmp_const_add_ui2_ui1(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui16 = lhs->ui16 + rhs->ui8;
	return TRUE;
}

BOOL vmp_const_add_ui2_i2(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i16 = lhs->ui16 + rhs->i16;
	return TRUE;
}

BOOL vmp_const_add_ui2_ui2(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui16 = lhs->ui16 + rhs->ui16;
	return TRUE;
}

BOOL vmp_const_add_ui2_i4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i32 = lhs->ui16 + rhs->i32;
	return TRUE;
}

BOOL vmp_const_add_ui2_ui4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui32 = lhs->ui16 + rhs->ui32;
	return TRUE;
}

BOOL vmp_const_add_ui2_i8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i64 = lhs->ui16 + rhs->i64;
	return TRUE;
}

BOOL vmp_const_add_ui2_ui8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui64 = lhs->ui16 + rhs->ui64;
	return TRUE;
}

BOOL vmp_const_add_ui2_f4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f32 = lhs->ui16 + rhs->f32;
	return TRUE;
}

BOOL vmp_const_add_ui2_f8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f64 = lhs->ui16 + rhs->f64;
	return TRUE;
}

BOOL vmp_const_add_ui2_ptr(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ptr = (size_t)lhs->ui16 + rhs->ptr;
	return TRUE;
}

////////////////////// 
//////////// INT32
/////////////////////

BOOL vmp_const_add_i4_bool(vmp_const* lhs, const vmp_const* rhs)
{
	return vmp_const_not_allowed(lhs, rhs);
}

BOOL vmp_const_add_i4_i1(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i32 = lhs->i32 + rhs->i8;
	return TRUE;
}

BOOL vmp_const_add_i4_ui1(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i32 = lhs->i32 + rhs->ui8;
	return TRUE;
}

BOOL vmp_const_add_i4_i2(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i32 = lhs->i32 + rhs->i16;
	return TRUE;
}

BOOL vmp_const_add_i4_ui2(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i32 = lhs->i32 + rhs->ui16;
	return TRUE;
}

BOOL vmp_const_add_i4_i4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i32 = lhs->i32 + rhs->i32;
	return TRUE;
}

BOOL vmp_const_add_i4_ui4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui32 = lhs->i32 + rhs->ui32;
	return TRUE;
}

BOOL vmp_const_add_i4_i8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i64 = lhs->i32 + rhs->i64;
	return TRUE;
}

BOOL vmp_const_add_i4_ui8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui64 = lhs->i32 + rhs->ui64;
	return TRUE;
}

BOOL vmp_const_add_i4_f4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f32 = lhs->i32 + rhs->f32;
	return TRUE;
}

BOOL vmp_const_add_i4_f8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f64 = lhs->i32 + rhs->f64;
	return TRUE;
}

BOOL vmp_const_add_i4_ptr(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ptr = (size_t)lhs->i32 + rhs->ptr;
	return TRUE;
}

////////////////////// 
//////////// UNSIGNED INT32
/////////////////////

BOOL vmp_const_add_ui4_bool(vmp_const* lhs, const vmp_const* rhs)
{
	return vmp_const_not_allowed(lhs, rhs);
}

BOOL vmp_const_add_ui4_i1(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui32 = lhs->ui32 + rhs->i8;
	return TRUE;
}

BOOL vmp_const_add_ui4_ui1(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui32 = lhs->ui32 + rhs->ui8;
	return TRUE;
}

BOOL vmp_const_add_ui4_i2(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui32 = lhs->ui32 + rhs->i16;
	return TRUE;
}

BOOL vmp_const_add_ui4_ui2(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui32 = lhs->ui32 + rhs->ui16;
	return TRUE;
}

BOOL vmp_const_add_ui4_i4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i32 = lhs->ui32 + rhs->i32;
	return TRUE;
}

BOOL vmp_const_add_ui4_ui4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui32 = lhs->ui32 + rhs->ui32;
	return TRUE;
}

BOOL vmp_const_add_ui4_i8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i64 = lhs->ui32 + rhs->i64;
	return TRUE;
}

BOOL vmp_const_add_ui4_ui8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui64 = lhs->ui32 + rhs->ui64;
	return TRUE;
}

BOOL vmp_const_add_ui4_f4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f32 = lhs->ui32 + rhs->f32;
	return TRUE;
}

BOOL vmp_const_add_ui4_f8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f64 = lhs->ui32 + rhs->f64;
	return TRUE;
}

BOOL vmp_const_add_ui4_ptr(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ptr = (size_t)lhs->ui32 + rhs->ptr;
	return TRUE;
}

////////////////////// 
//////////// INT64
/////////////////////

BOOL vmp_const_add_i8_bool(vmp_const* lhs, const vmp_const* rhs)
{
	return vmp_const_not_allowed(lhs, rhs);
}

BOOL vmp_const_add_i8_i1(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i64 = lhs->i64 + rhs->i8;
	return TRUE;
}

BOOL vmp_const_add_i8_ui1(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i64 = lhs->i64 + rhs->ui8;
	return TRUE;
}

BOOL vmp_const_add_i8_i2(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i64 = lhs->i64 + rhs->i16;
	return TRUE;
}

BOOL vmp_const_add_i8_ui2(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i64 = lhs->i64 + rhs->ui16;
	return TRUE;
}

BOOL vmp_const_add_i8_i4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i64 = lhs->i64 + rhs->i32;
	return TRUE;
}

BOOL vmp_const_add_i8_ui4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i64 = lhs->i64 + rhs->ui32;
	return TRUE;
}

BOOL vmp_const_add_i8_i8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->i64 = lhs->i64 + rhs->i64;
	return TRUE;
}

BOOL vmp_const_add_i8_ui8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui64 = lhs->i64 + rhs->ui64;
	return TRUE;
}

BOOL vmp_const_add_i8_f4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f32 = lhs->i64 + rhs->f32;
	return TRUE;
}

BOOL vmp_const_add_i8_f8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f64 = lhs->i64 + rhs->f64;
	return TRUE;
}

BOOL vmp_const_add_i8_ptr(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ptr = (size_t)lhs->i64 + rhs->ptr;
	return TRUE;
}

////////////////////// 
//////////// INT64
/////////////////////

BOOL vmp_const_add_ui8_bool(vmp_const* lhs, const vmp_const* rhs)
{
	return vmp_const_not_allowed(lhs, rhs);
}

BOOL vmp_const_add_ui8_i1(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui64 = lhs->ui64 + rhs->i8;
	return TRUE;
}

BOOL vmp_const_add_ui8_ui1(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui64 = lhs->ui64 + rhs->ui8;
	return TRUE;
}

BOOL vmp_const_add_ui8_i2(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui64 = lhs->ui64 + rhs->i16;
	return TRUE;
}

BOOL vmp_const_add_ui8_ui2(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui64 = lhs->ui64 + rhs->ui16;
	return TRUE;
}

BOOL vmp_const_add_ui8_i4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui64 = lhs->ui64 + rhs->i32;
	return TRUE;
}

BOOL vmp_const_add_ui8_ui4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui64 = lhs->ui64 + rhs->ui32;
	return TRUE;
}

BOOL vmp_const_add_ui8_i8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui64 = lhs->ui64 + rhs->i64;
	return TRUE;
}

BOOL vmp_const_add_ui8_ui8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ui64 = lhs->ui64 + rhs->ui64;
	return TRUE;
}

BOOL vmp_const_add_ui8_f4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f32 = lhs->ui64 + rhs->f32;
	return TRUE;
}

BOOL vmp_const_add_ui8_f8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f64 = lhs->ui64 + rhs->f64;
	return TRUE;
}

BOOL vmp_const_add_ui8_ptr(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->ptr = (size_t)lhs->ui64 + rhs->ptr;
	return TRUE;
}

////////////////////// 
//////////// FLOAT32
/////////////////////

BOOL vmp_const_add_f4_bool(vmp_const* lhs, const vmp_const* rhs)
{
	return vmp_const_not_allowed(lhs, rhs);
}

BOOL vmp_const_add_f4_i1(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f32 = lhs->f32 + rhs->i8;
	return TRUE;
}

BOOL vmp_const_add_f4_ui1(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f32 = lhs->f32 + rhs->ui8;
	return TRUE;
}

BOOL vmp_const_add_f4_i2(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f32 = lhs->f32 + rhs->i16;
	return TRUE;
}

BOOL vmp_const_add_f4_ui2(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f32 = lhs->f32 + rhs->ui16;
	return TRUE;
}

BOOL vmp_const_add_f4_i4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f32 = lhs->f32 + rhs->i32;
	return TRUE;
}

BOOL vmp_const_add_f4_ui4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f32 = lhs->f32 + rhs->ui32;
	return TRUE;
}

BOOL vmp_const_add_f4_i8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f32 = lhs->f32 + rhs->i64;
	return TRUE;
}

BOOL vmp_const_add_f4_ui8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f32 = lhs->f32 + rhs->ui64;
	return TRUE;
}

BOOL vmp_const_add_f4_f4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f32 = lhs->f32 + rhs->f32;
	return TRUE;
}

BOOL vmp_const_add_f4_f8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f64 = lhs->f32 + rhs->f64;
	return TRUE;
}

BOOL vmp_const_add_f4_ptr(vmp_const* lhs, const vmp_const* rhs)
{
	return vmp_const_not_allowed(lhs, rhs);
}

////////////////////// 
//////////// FLOAT64
/////////////////////

BOOL vmp_const_add_f8_bool(vmp_const* lhs, const vmp_const* rhs)
{
	return vmp_const_not_allowed(lhs, rhs);
}

BOOL vmp_const_add_f8_i1(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f64 = lhs->f64 + rhs->i8;
	return TRUE;
}

BOOL vmp_const_add_f8_ui1(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f64 = lhs->f64 + rhs->ui8;
	return TRUE;
}

BOOL vmp_const_add_f8_i2(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f64 = lhs->f64 + rhs->i16;
	return TRUE;
}

BOOL vmp_const_add_f8_ui2(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f64 = lhs->f64 + rhs->ui16;
	return TRUE;
}

BOOL vmp_const_add_f8_i4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f64 = lhs->f64 + rhs->i32;
	return TRUE;
}

BOOL vmp_const_add_f8_ui4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f64 = lhs->f64 + rhs->ui32;
	return TRUE;
}

BOOL vmp_const_add_f8_i8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f64 = lhs->f64 + rhs->i64;
	return TRUE;
}

BOOL vmp_const_add_f8_ui8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f64 = lhs->f64 + rhs->ui64;
	return TRUE;
}

BOOL vmp_const_add_f8_f4(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f64 = lhs->f64 + rhs->f32;
	return TRUE;
}

BOOL vmp_const_add_f8_f8(vmp_const* lhs, const vmp_const* rhs)
{
	lhs->f64 = lhs->f64 + rhs->f64;
	return TRUE;
}

BOOL vmp_const_add_f8_ptr(vmp_const* lhs, const vmp_const* rhs)
{
	return vmp_const_not_allowed(lhs, rhs);
}

BOOL vmp_const_add(vmp_const* lhs, const vmp_const* rhs)
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
			vmp_const_add_i1_bool,
			vmp_const_add_i1_i1, vmp_const_add_i1_ui1,
			vmp_const_add_i1_i2, vmp_const_add_i1_ui2,
			vmp_const_add_i1_i4, vmp_const_add_i1_ui4,
			vmp_const_add_i1_i8, vmp_const_add_i1_ui8,
			vmp_const_add_i1_f4, vmp_const_add_i1_f8,
			vmp_const_add_i1_ptr
		},
		// UINT8
		{
			vmp_const_not_allowed,
			vmp_const_add_ui1_bool,
			vmp_const_add_ui1_i1, vmp_const_add_ui1_ui1,
			vmp_const_add_ui1_i2, vmp_const_add_ui1_ui2,
			vmp_const_add_ui1_i4, vmp_const_add_ui1_ui4,
			vmp_const_add_ui1_i8, vmp_const_add_ui1_ui8,
			vmp_const_add_ui1_f4, vmp_const_add_ui1_f8,
			vmp_const_add_ui1_ptr
		},
		// INT16
		{
			vmp_const_not_allowed,
			vmp_const_add_i2_bool,
			vmp_const_add_i2_i1, vmp_const_add_i2_ui1,
			vmp_const_add_i2_i2, vmp_const_add_i2_ui2,
			vmp_const_add_i2_i4, vmp_const_add_i2_ui4,
			vmp_const_add_i2_i8, vmp_const_add_i2_ui8,
			vmp_const_add_i2_f4, vmp_const_add_i2_f8,
			vmp_const_add_i2_ptr
		},
		// UINT16
		{
			vmp_const_not_allowed,
			vmp_const_add_ui2_bool,
			vmp_const_add_ui2_i1, vmp_const_add_ui2_ui1,
			vmp_const_add_ui2_i2, vmp_const_add_ui2_ui2,
			vmp_const_add_ui2_i4, vmp_const_add_ui2_ui4,
			vmp_const_add_ui2_i8, vmp_const_add_ui2_ui8,
			vmp_const_add_ui2_f4, vmp_const_add_ui2_f8,
			vmp_const_add_ui2_ptr
		},
		// INT32
		{
			vmp_const_not_allowed,
			vmp_const_add_i4_bool,
			vmp_const_add_i4_i1, vmp_const_add_i4_ui1,
			vmp_const_add_i4_i2, vmp_const_add_i4_ui2,
			vmp_const_add_i4_i4, vmp_const_add_i4_ui4,
			vmp_const_add_i4_i8, vmp_const_add_i4_ui8,
			vmp_const_add_i4_f4, vmp_const_add_i4_f8,
			vmp_const_add_i4_ptr
		},
		// UINT32
		{
			vmp_const_not_allowed,
			vmp_const_add_ui4_bool,
			vmp_const_add_ui4_i1, vmp_const_add_ui4_ui1,
			vmp_const_add_ui4_i2, vmp_const_add_ui4_ui2,
			vmp_const_add_ui4_i4, vmp_const_add_ui4_ui4,
			vmp_const_add_ui4_i8, vmp_const_add_ui4_ui8,
			vmp_const_add_ui4_f4, vmp_const_add_ui4_f8,
			vmp_const_add_ui4_ptr
		},
		// INT64
		{
			vmp_const_not_allowed,
			vmp_const_add_i8_bool,
			vmp_const_add_i8_i1, vmp_const_add_i8_ui1,
			vmp_const_add_i8_i2, vmp_const_add_i8_ui2,
			vmp_const_add_i8_i4, vmp_const_add_i8_ui4,
			vmp_const_add_i8_i8, vmp_const_add_i8_ui8,
			vmp_const_add_i8_f4, vmp_const_add_i8_f8,
			vmp_const_add_i8_ptr
		},
		// UINT64
		{
			vmp_const_not_allowed,
			vmp_const_add_ui8_bool,
			vmp_const_add_ui8_i1, vmp_const_add_ui8_ui1,
			vmp_const_add_ui8_i2, vmp_const_add_ui8_ui2,
			vmp_const_add_ui8_i4, vmp_const_add_ui8_ui4,
			vmp_const_add_ui8_i8, vmp_const_add_ui8_ui8,
			vmp_const_add_ui8_f4, vmp_const_add_ui8_f8,
			vmp_const_add_ui8_ptr
		},
		// FLOAT32
		{
			vmp_const_not_allowed,
			vmp_const_add_f4_bool,
			vmp_const_add_f4_i1, vmp_const_add_f4_ui1,
			vmp_const_add_f4_i2, vmp_const_add_f4_ui2,
			vmp_const_add_f4_i4, vmp_const_add_f4_ui4,
			vmp_const_add_f4_i8, vmp_const_add_f4_ui8,
			vmp_const_add_f4_f4, vmp_const_add_f4_f8,
			vmp_const_add_f4_ptr
		},
		// FLOAT64
		{
			vmp_const_not_allowed,
			vmp_const_add_f8_bool,
			vmp_const_add_f8_i1, vmp_const_add_f8_ui1,
			vmp_const_add_f8_i2, vmp_const_add_f8_ui2,
			vmp_const_add_f8_i4, vmp_const_add_f8_ui4,
			vmp_const_add_f8_i8, vmp_const_add_f8_ui8,
			vmp_const_add_f8_f4, vmp_const_add_f8_f8,
			vmp_const_add_f8_ptr
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
