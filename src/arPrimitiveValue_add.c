#include "arPrimitiveValue.h"
#include "arInstructions.h"

////////////////////// 
//////////// INT8
/////////////////////

BOOL arPrimitiveValue_add_i1_bool(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	return arPrimitiveValue_not_allowed(lhs, rhs);
}

BOOL arPrimitiveValue_add_i1_i1(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->i32 = lhs->i8 + rhs->i8;
	return TRUE;
}

BOOL arPrimitiveValue_add_i1_ui1(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->i32 = lhs->i8 + rhs->ui8;
	return TRUE;
}

BOOL arPrimitiveValue_add_i1_i2(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->i32 = lhs->i8 + rhs->i16;
	return TRUE;
}

BOOL arPrimitiveValue_add_i1_ui2(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->i32 = lhs->i8 + rhs->ui16;
	return TRUE;
}

BOOL arPrimitiveValue_add_i1_i4(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->i32 = lhs->i8 + rhs->i32;
	return TRUE;
}

BOOL arPrimitiveValue_add_i1_ui4(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->ui32 = lhs->i8 + rhs->ui32;
	return TRUE;
}

BOOL arPrimitiveValue_add_i1_i8(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->i64 = lhs->i8 + rhs->i64;
	return TRUE;
}

BOOL arPrimitiveValue_add_i1_ui8(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->ui64 = lhs->i8 + rhs->ui64;
	return TRUE;
}

BOOL arPrimitiveValue_add_i1_f4(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->f32 = lhs->i8 + rhs->f32;
	return TRUE;
}

BOOL arPrimitiveValue_add_i1_f8(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->f64 = lhs->i8 + rhs->f64;
	return TRUE;
}

BOOL arPrimitiveValue_add_i1_ptr(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->ptr = (size_t)lhs->i8 + rhs->ptr;
	return TRUE;
}

////////////////////// 
//////////// UNSIGNED INT8
/////////////////////

BOOL arPrimitiveValue_add_ui1_bool(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	return arPrimitiveValue_not_allowed(lhs, rhs);
}

BOOL arPrimitiveValue_add_ui1_i1(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->i32 = lhs->ui8 + rhs->i8;
	return TRUE;
}

BOOL arPrimitiveValue_add_ui1_ui1(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->i32 = lhs->ui8 + rhs->ui8;
	return TRUE;
}

BOOL arPrimitiveValue_add_ui1_i2(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->i32 = lhs->ui8 + rhs->i16;
	return TRUE;
}

BOOL arPrimitiveValue_add_ui1_ui2(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->i32 = lhs->ui8 + rhs->ui16;
	return TRUE;
}

BOOL arPrimitiveValue_add_ui1_i4(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->i32 = lhs->ui8 + rhs->i32;
	return TRUE;
}

BOOL arPrimitiveValue_add_ui1_ui4(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->ui32 = lhs->ui8 + rhs->ui32;
	return TRUE;
}

BOOL arPrimitiveValue_add_ui1_i8(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->i64 = lhs->ui8 + rhs->i64;
	return TRUE;
}

BOOL arPrimitiveValue_add_ui1_ui8(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->ui64 = lhs->ui8 + rhs->ui64;
	return TRUE;
}

BOOL arPrimitiveValue_add_ui1_f4(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->f32 = lhs->ui8 + rhs->f32;
	return TRUE;
}

BOOL arPrimitiveValue_add_ui1_f8(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->f64 = lhs->ui8 + rhs->f64;
	return TRUE;
}

BOOL arPrimitiveValue_add_ui1_ptr(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->ptr = (size_t)lhs->ui8 + rhs->ptr;
	return TRUE;
}

////////////////////// 
//////////// INT16
/////////////////////

BOOL arPrimitiveValue_add_i2_bool(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	return arPrimitiveValue_not_allowed(lhs, rhs);
}

BOOL arPrimitiveValue_add_i2_i1(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->i16 = lhs->i16 + rhs->i8;
	return TRUE;
}

BOOL arPrimitiveValue_add_i2_ui1(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->i16 = lhs->i16 + rhs->ui8;
	return TRUE;
}

BOOL arPrimitiveValue_add_i2_i2(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->i16 = lhs->i16 + rhs->i16;
	return TRUE;
}

BOOL arPrimitiveValue_add_i2_ui2(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->ui16 = lhs->i16 + rhs->ui16;
	return TRUE;
}

BOOL arPrimitiveValue_add_i2_i4(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->i32 = lhs->i16 + rhs->i32;
	return TRUE;
}

BOOL arPrimitiveValue_add_i2_ui4(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->ui32 = lhs->i16 + rhs->ui32;
	return TRUE;
}

BOOL arPrimitiveValue_add_i2_i8(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->i64 = lhs->i16 + rhs->i64;
	return TRUE;
}

BOOL arPrimitiveValue_add_i2_ui8(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->ui64 = lhs->i16 + rhs->ui64;
	return TRUE;
}

BOOL arPrimitiveValue_add_i2_f4(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->f32 = lhs->i16 + rhs->f32;
	return TRUE;
}

BOOL arPrimitiveValue_add_i2_f8(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->f64 = lhs->i16 + rhs->f64;
	return TRUE;
}

BOOL arPrimitiveValue_add_i2_ptr(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->ptr = (size_t)lhs->i16 + rhs->ptr;
	return TRUE;
}

////////////////////// 
//////////// UNSIGNED INT16
/////////////////////

BOOL arPrimitiveValue_add_ui2_bool(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	return arPrimitiveValue_not_allowed(lhs, rhs);
}

BOOL arPrimitiveValue_add_ui2_i1(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->ui16 = lhs->ui16 + rhs->i8;
	return TRUE;
}

BOOL arPrimitiveValue_add_ui2_ui1(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->ui16 = lhs->ui16 + rhs->ui8;
	return TRUE;
}

BOOL arPrimitiveValue_add_ui2_i2(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->i16 = lhs->ui16 + rhs->i16;
	return TRUE;
}

BOOL arPrimitiveValue_add_ui2_ui2(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->ui16 = lhs->ui16 + rhs->ui16;
	return TRUE;
}

BOOL arPrimitiveValue_add_ui2_i4(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->i32 = lhs->ui16 + rhs->i32;
	return TRUE;
}

BOOL arPrimitiveValue_add_ui2_ui4(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->ui32 = lhs->ui16 + rhs->ui32;
	return TRUE;
}

BOOL arPrimitiveValue_add_ui2_i8(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->i64 = lhs->ui16 + rhs->i64;
	return TRUE;
}

BOOL arPrimitiveValue_add_ui2_ui8(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->ui64 = lhs->ui16 + rhs->ui64;
	return TRUE;
}

BOOL arPrimitiveValue_add_ui2_f4(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->f32 = lhs->ui16 + rhs->f32;
	return TRUE;
}

BOOL arPrimitiveValue_add_ui2_f8(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->f64 = lhs->ui16 + rhs->f64;
	return TRUE;
}

BOOL arPrimitiveValue_add_ui2_ptr(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->ptr = (size_t)lhs->ui16 + rhs->ptr;
	return TRUE;
}

////////////////////// 
//////////// INT32
/////////////////////

BOOL arPrimitiveValue_add_i4_bool(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	return arPrimitiveValue_not_allowed(lhs, rhs);
}

BOOL arPrimitiveValue_add_i4_i1(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->i32 = lhs->i32 + rhs->i8;
	return TRUE;
}

BOOL arPrimitiveValue_add_i4_ui1(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->i32 = lhs->i32 + rhs->ui8;
	return TRUE;
}

BOOL arPrimitiveValue_add_i4_i2(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->i32 = lhs->i32 + rhs->i16;
	return TRUE;
}

BOOL arPrimitiveValue_add_i4_ui2(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->i32 = lhs->i32 + rhs->ui16;
	return TRUE;
}

BOOL arPrimitiveValue_add_i4_i4(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->i32 = lhs->i32 + rhs->i32;
	return TRUE;
}

BOOL arPrimitiveValue_add_i4_ui4(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->ui32 = lhs->i32 + rhs->ui32;
	return TRUE;
}

BOOL arPrimitiveValue_add_i4_i8(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->i64 = lhs->i32 + rhs->i64;
	return TRUE;
}

BOOL arPrimitiveValue_add_i4_ui8(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->ui64 = lhs->i32 + rhs->ui64;
	return TRUE;
}

BOOL arPrimitiveValue_add_i4_f4(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->f32 = lhs->i32 + rhs->f32;
	return TRUE;
}

BOOL arPrimitiveValue_add_i4_f8(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->f64 = lhs->i32 + rhs->f64;
	return TRUE;
}

BOOL arPrimitiveValue_add_i4_ptr(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->ptr = (size_t)lhs->i32 + rhs->ptr;
	return TRUE;
}

////////////////////// 
//////////// UNSIGNED INT32
/////////////////////

BOOL arPrimitiveValue_add_ui4_bool(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	return arPrimitiveValue_not_allowed(lhs, rhs);
}

BOOL arPrimitiveValue_add_ui4_i1(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->ui32 = lhs->ui32 + rhs->i8;
	return TRUE;
}

BOOL arPrimitiveValue_add_ui4_ui1(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->ui32 = lhs->ui32 + rhs->ui8;
	return TRUE;
}

BOOL arPrimitiveValue_add_ui4_i2(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->ui32 = lhs->ui32 + rhs->i16;
	return TRUE;
}

BOOL arPrimitiveValue_add_ui4_ui2(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->ui32 = lhs->ui32 + rhs->ui16;
	return TRUE;
}

BOOL arPrimitiveValue_add_ui4_i4(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->i32 = lhs->ui32 + rhs->i32;
	return TRUE;
}

BOOL arPrimitiveValue_add_ui4_ui4(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->ui32 = lhs->ui32 + rhs->ui32;
	return TRUE;
}

BOOL arPrimitiveValue_add_ui4_i8(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->i64 = lhs->ui32 + rhs->i64;
	return TRUE;
}

BOOL arPrimitiveValue_add_ui4_ui8(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->ui64 = lhs->ui32 + rhs->ui64;
	return TRUE;
}

BOOL arPrimitiveValue_add_ui4_f4(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->f32 = lhs->ui32 + rhs->f32;
	return TRUE;
}

BOOL arPrimitiveValue_add_ui4_f8(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->f64 = lhs->ui32 + rhs->f64;
	return TRUE;
}

BOOL arPrimitiveValue_add_ui4_ptr(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->ptr = (size_t)lhs->ui32 + rhs->ptr;
	return TRUE;
}

////////////////////// 
//////////// INT64
/////////////////////

BOOL arPrimitiveValue_add_i8_bool(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	return arPrimitiveValue_not_allowed(lhs, rhs);
}

BOOL arPrimitiveValue_add_i8_i1(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->i64 = lhs->i64 + rhs->i8;
	return TRUE;
}

BOOL arPrimitiveValue_add_i8_ui1(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->i64 = lhs->i64 + rhs->ui8;
	return TRUE;
}

BOOL arPrimitiveValue_add_i8_i2(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->i64 = lhs->i64 + rhs->i16;
	return TRUE;
}

BOOL arPrimitiveValue_add_i8_ui2(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->i64 = lhs->i64 + rhs->ui16;
	return TRUE;
}

BOOL arPrimitiveValue_add_i8_i4(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->i64 = lhs->i64 + rhs->i32;
	return TRUE;
}

BOOL arPrimitiveValue_add_i8_ui4(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->i64 = lhs->i64 + rhs->ui32;
	return TRUE;
}

BOOL arPrimitiveValue_add_i8_i8(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->i64 = lhs->i64 + rhs->i64;
	return TRUE;
}

BOOL arPrimitiveValue_add_i8_ui8(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->ui64 = lhs->i64 + rhs->ui64;
	return TRUE;
}

BOOL arPrimitiveValue_add_i8_f4(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->f32 = lhs->i64 + rhs->f32;
	return TRUE;
}

BOOL arPrimitiveValue_add_i8_f8(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->f64 = lhs->i64 + rhs->f64;
	return TRUE;
}

BOOL arPrimitiveValue_add_i8_ptr(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->ptr = (size_t)lhs->i64 + rhs->ptr;
	return TRUE;
}

////////////////////// 
//////////// INT64
/////////////////////

BOOL arPrimitiveValue_add_ui8_bool(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	return arPrimitiveValue_not_allowed(lhs, rhs);
}

BOOL arPrimitiveValue_add_ui8_i1(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->ui64 = lhs->ui64 + rhs->i8;
	return TRUE;
}

BOOL arPrimitiveValue_add_ui8_ui1(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->ui64 = lhs->ui64 + rhs->ui8;
	return TRUE;
}

BOOL arPrimitiveValue_add_ui8_i2(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->ui64 = lhs->ui64 + rhs->i16;
	return TRUE;
}

BOOL arPrimitiveValue_add_ui8_ui2(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->ui64 = lhs->ui64 + rhs->ui16;
	return TRUE;
}

BOOL arPrimitiveValue_add_ui8_i4(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->ui64 = lhs->ui64 + rhs->i32;
	return TRUE;
}

BOOL arPrimitiveValue_add_ui8_ui4(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->ui64 = lhs->ui64 + rhs->ui32;
	return TRUE;
}

BOOL arPrimitiveValue_add_ui8_i8(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->ui64 = lhs->ui64 + rhs->i64;
	return TRUE;
}

BOOL arPrimitiveValue_add_ui8_ui8(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->ui64 = lhs->ui64 + rhs->ui64;
	return TRUE;
}

BOOL arPrimitiveValue_add_ui8_f4(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->f32 = lhs->ui64 + rhs->f32;
	return TRUE;
}

BOOL arPrimitiveValue_add_ui8_f8(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->f64 = lhs->ui64 + rhs->f64;
	return TRUE;
}

BOOL arPrimitiveValue_add_ui8_ptr(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->ptr = (size_t)lhs->ui64 + rhs->ptr;
	return TRUE;
}

////////////////////// 
//////////// FLOAT32
/////////////////////

BOOL arPrimitiveValue_add_f4_bool(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	return arPrimitiveValue_not_allowed(lhs, rhs);
}

BOOL arPrimitiveValue_add_f4_i1(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->f32 = lhs->f32 + rhs->i8;
	return TRUE;
}

BOOL arPrimitiveValue_add_f4_ui1(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->f32 = lhs->f32 + rhs->ui8;
	return TRUE;
}

BOOL arPrimitiveValue_add_f4_i2(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->f32 = lhs->f32 + rhs->i16;
	return TRUE;
}

BOOL arPrimitiveValue_add_f4_ui2(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->f32 = lhs->f32 + rhs->ui16;
	return TRUE;
}

BOOL arPrimitiveValue_add_f4_i4(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->f32 = lhs->f32 + rhs->i32;
	return TRUE;
}

BOOL arPrimitiveValue_add_f4_ui4(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->f32 = lhs->f32 + rhs->ui32;
	return TRUE;
}

BOOL arPrimitiveValue_add_f4_i8(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->f32 = lhs->f32 + rhs->i64;
	return TRUE;
}

BOOL arPrimitiveValue_add_f4_ui8(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->f32 = lhs->f32 + rhs->ui64;
	return TRUE;
}

BOOL arPrimitiveValue_add_f4_f4(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->f32 = lhs->f32 + rhs->f32;
	return TRUE;
}

BOOL arPrimitiveValue_add_f4_f8(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->f64 = lhs->f32 + rhs->f64;
	return TRUE;
}

BOOL arPrimitiveValue_add_f4_ptr(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	return arPrimitiveValue_not_allowed(lhs, rhs);
}

////////////////////// 
//////////// FLOAT64
/////////////////////

BOOL arPrimitiveValue_add_f8_bool(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	return arPrimitiveValue_not_allowed(lhs, rhs);
}

BOOL arPrimitiveValue_add_f8_i1(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->f64 = lhs->f64 + rhs->i8;
	return TRUE;
}

BOOL arPrimitiveValue_add_f8_ui1(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->f64 = lhs->f64 + rhs->ui8;
	return TRUE;
}

BOOL arPrimitiveValue_add_f8_i2(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->f64 = lhs->f64 + rhs->i16;
	return TRUE;
}

BOOL arPrimitiveValue_add_f8_ui2(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->f64 = lhs->f64 + rhs->ui16;
	return TRUE;
}

BOOL arPrimitiveValue_add_f8_i4(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->f64 = lhs->f64 + rhs->i32;
	return TRUE;
}

BOOL arPrimitiveValue_add_f8_ui4(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->f64 = lhs->f64 + rhs->ui32;
	return TRUE;
}

BOOL arPrimitiveValue_add_f8_i8(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->f64 = lhs->f64 + rhs->i64;
	return TRUE;
}

BOOL arPrimitiveValue_add_f8_ui8(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->f64 = lhs->f64 + rhs->ui64;
	return TRUE;
}

BOOL arPrimitiveValue_add_f8_f4(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->f64 = lhs->f64 + rhs->f32;
	return TRUE;
}

BOOL arPrimitiveValue_add_f8_f8(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	lhs->f64 = lhs->f64 + rhs->f64;
	return TRUE;
}

BOOL arPrimitiveValue_add_f8_ptr(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	return arPrimitiveValue_not_allowed(lhs, rhs);
}

BOOL arPrimitiveValue_add(arPrimitiveValue* lhs, const arPrimitiveValue* rhs)
{
	static const arPrimitiveValue_combine_fn functions[ARLANG_PRIMITIVE_COUNT][ARLANG_PRIMITIVE_COUNT] = {
		// UNKNOWN
		{
			arPrimitiveValue_not_allowed,
			arPrimitiveValue_not_allowed,
			arPrimitiveValue_not_allowed, arPrimitiveValue_not_allowed,
			arPrimitiveValue_not_allowed, arPrimitiveValue_not_allowed,
			arPrimitiveValue_not_allowed, arPrimitiveValue_not_allowed,
			arPrimitiveValue_not_allowed, arPrimitiveValue_not_allowed,
			arPrimitiveValue_not_allowed, arPrimitiveValue_not_allowed,
			arPrimitiveValue_not_allowed
		},
		// BOOL
		{
			arPrimitiveValue_not_allowed,
			arPrimitiveValue_not_allowed,
			arPrimitiveValue_not_allowed, arPrimitiveValue_not_allowed,
			arPrimitiveValue_not_allowed, arPrimitiveValue_not_allowed,
			arPrimitiveValue_not_allowed, arPrimitiveValue_not_allowed,
			arPrimitiveValue_not_allowed, arPrimitiveValue_not_allowed,
			arPrimitiveValue_not_allowed, arPrimitiveValue_not_allowed,
			arPrimitiveValue_not_allowed
		},
		// INT8
		{
			arPrimitiveValue_not_allowed,
			arPrimitiveValue_add_i1_bool,
			arPrimitiveValue_add_i1_i1, arPrimitiveValue_add_i1_ui1,
			arPrimitiveValue_add_i1_i2, arPrimitiveValue_add_i1_ui2,
			arPrimitiveValue_add_i1_i4, arPrimitiveValue_add_i1_ui4,
			arPrimitiveValue_add_i1_i8, arPrimitiveValue_add_i1_ui8,
			arPrimitiveValue_add_i1_f4, arPrimitiveValue_add_i1_f8,
			arPrimitiveValue_add_i1_ptr
		},
		// UINT8
		{
			arPrimitiveValue_not_allowed,
			arPrimitiveValue_add_ui1_bool,
			arPrimitiveValue_add_ui1_i1, arPrimitiveValue_add_ui1_ui1,
			arPrimitiveValue_add_ui1_i2, arPrimitiveValue_add_ui1_ui2,
			arPrimitiveValue_add_ui1_i4, arPrimitiveValue_add_ui1_ui4,
			arPrimitiveValue_add_ui1_i8, arPrimitiveValue_add_ui1_ui8,
			arPrimitiveValue_add_ui1_f4, arPrimitiveValue_add_ui1_f8,
			arPrimitiveValue_add_ui1_ptr
		},
		// INT16
		{
			arPrimitiveValue_not_allowed,
			arPrimitiveValue_add_i2_bool,
			arPrimitiveValue_add_i2_i1, arPrimitiveValue_add_i2_ui1,
			arPrimitiveValue_add_i2_i2, arPrimitiveValue_add_i2_ui2,
			arPrimitiveValue_add_i2_i4, arPrimitiveValue_add_i2_ui4,
			arPrimitiveValue_add_i2_i8, arPrimitiveValue_add_i2_ui8,
			arPrimitiveValue_add_i2_f4, arPrimitiveValue_add_i2_f8,
			arPrimitiveValue_add_i2_ptr
		},
		// UINT16
		{
			arPrimitiveValue_not_allowed,
			arPrimitiveValue_add_ui2_bool,
			arPrimitiveValue_add_ui2_i1, arPrimitiveValue_add_ui2_ui1,
			arPrimitiveValue_add_ui2_i2, arPrimitiveValue_add_ui2_ui2,
			arPrimitiveValue_add_ui2_i4, arPrimitiveValue_add_ui2_ui4,
			arPrimitiveValue_add_ui2_i8, arPrimitiveValue_add_ui2_ui8,
			arPrimitiveValue_add_ui2_f4, arPrimitiveValue_add_ui2_f8,
			arPrimitiveValue_add_ui2_ptr
		},
		// INT32
		{
			arPrimitiveValue_not_allowed,
			arPrimitiveValue_add_i4_bool,
			arPrimitiveValue_add_i4_i1, arPrimitiveValue_add_i4_ui1,
			arPrimitiveValue_add_i4_i2, arPrimitiveValue_add_i4_ui2,
			arPrimitiveValue_add_i4_i4, arPrimitiveValue_add_i4_ui4,
			arPrimitiveValue_add_i4_i8, arPrimitiveValue_add_i4_ui8,
			arPrimitiveValue_add_i4_f4, arPrimitiveValue_add_i4_f8,
			arPrimitiveValue_add_i4_ptr
		},
		// UINT32
		{
			arPrimitiveValue_not_allowed,
			arPrimitiveValue_add_ui4_bool,
			arPrimitiveValue_add_ui4_i1, arPrimitiveValue_add_ui4_ui1,
			arPrimitiveValue_add_ui4_i2, arPrimitiveValue_add_ui4_ui2,
			arPrimitiveValue_add_ui4_i4, arPrimitiveValue_add_ui4_ui4,
			arPrimitiveValue_add_ui4_i8, arPrimitiveValue_add_ui4_ui8,
			arPrimitiveValue_add_ui4_f4, arPrimitiveValue_add_ui4_f8,
			arPrimitiveValue_add_ui4_ptr
		},
		// INT64
		{
			arPrimitiveValue_not_allowed,
			arPrimitiveValue_add_i8_bool,
			arPrimitiveValue_add_i8_i1, arPrimitiveValue_add_i8_ui1,
			arPrimitiveValue_add_i8_i2, arPrimitiveValue_add_i8_ui2,
			arPrimitiveValue_add_i8_i4, arPrimitiveValue_add_i8_ui4,
			arPrimitiveValue_add_i8_i8, arPrimitiveValue_add_i8_ui8,
			arPrimitiveValue_add_i8_f4, arPrimitiveValue_add_i8_f8,
			arPrimitiveValue_add_i8_ptr
		},
		// UINT64
		{
			arPrimitiveValue_not_allowed,
			arPrimitiveValue_add_ui8_bool,
			arPrimitiveValue_add_ui8_i1, arPrimitiveValue_add_ui8_ui1,
			arPrimitiveValue_add_ui8_i2, arPrimitiveValue_add_ui8_ui2,
			arPrimitiveValue_add_ui8_i4, arPrimitiveValue_add_ui8_ui4,
			arPrimitiveValue_add_ui8_i8, arPrimitiveValue_add_ui8_ui8,
			arPrimitiveValue_add_ui8_f4, arPrimitiveValue_add_ui8_f8,
			arPrimitiveValue_add_ui8_ptr
		},
		// FLOAT32
		{
			arPrimitiveValue_not_allowed,
			arPrimitiveValue_add_f4_bool,
			arPrimitiveValue_add_f4_i1, arPrimitiveValue_add_f4_ui1,
			arPrimitiveValue_add_f4_i2, arPrimitiveValue_add_f4_ui2,
			arPrimitiveValue_add_f4_i4, arPrimitiveValue_add_f4_ui4,
			arPrimitiveValue_add_f4_i8, arPrimitiveValue_add_f4_ui8,
			arPrimitiveValue_add_f4_f4, arPrimitiveValue_add_f4_f8,
			arPrimitiveValue_add_f4_ptr
		},
		// FLOAT64
		{
			arPrimitiveValue_not_allowed,
			arPrimitiveValue_add_f8_bool,
			arPrimitiveValue_add_f8_i1, arPrimitiveValue_add_f8_ui1,
			arPrimitiveValue_add_f8_i2, arPrimitiveValue_add_f8_ui2,
			arPrimitiveValue_add_f8_i4, arPrimitiveValue_add_f8_ui4,
			arPrimitiveValue_add_f8_i8, arPrimitiveValue_add_f8_ui8,
			arPrimitiveValue_add_f8_f4, arPrimitiveValue_add_f8_f8,
			arPrimitiveValue_add_f8_ptr
		},
		// PTR
		{
			arPrimitiveValue_not_implemented,
			arPrimitiveValue_not_implemented,
			arPrimitiveValue_not_implemented, arPrimitiveValue_not_implemented,
			arPrimitiveValue_not_implemented, arPrimitiveValue_not_implemented,
			arPrimitiveValue_not_implemented, arPrimitiveValue_not_implemented,
			arPrimitiveValue_not_implemented, arPrimitiveValue_not_implemented,
			arPrimitiveValue_not_implemented, arPrimitiveValue_not_implemented,
			arPrimitiveValue_not_implemented
		}
	};

	if (lhs->type <= ARLANG_PRIMITIVE_UNKNOWN || lhs->type >= ARLANG_PRIMITIVE_COUNT)
		return FALSE;
	if (rhs->type <= ARLANG_PRIMITIVE_UNKNOWN || rhs->type >= ARLANG_PRIMITIVE_COUNT)
		return FALSE;
	if (!functions[lhs->type][rhs->type](lhs, rhs))
		return FALSE;
	lhs->type = arPrimitiveValue_datatype(lhs->type, rhs->type);
	return TRUE;
}
