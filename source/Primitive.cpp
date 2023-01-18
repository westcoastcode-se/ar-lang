#include "Primitive.h"

using namespace WestCoastCode;

PrimitiveType PrimitiveValue::Result(PrimitiveType rhs, PrimitiveType lhs)
{
	static constexpr auto count = (::I32)PrimitiveType::Count;

	static const PrimitiveType types[count][count] = {
		// UNKNOWN
		{
			PrimitiveType::Unknown
		},
		// bool
		{
			PrimitiveType::Unknown, PrimitiveType::Bool, PrimitiveType::Unknown
		},
		// INT8
		{
			PrimitiveType::Unknown, PrimitiveType::I32, PrimitiveType::I32, PrimitiveType::I32, PrimitiveType::I32, PrimitiveType::I32, PrimitiveType::I32,
			PrimitiveType::U32, PrimitiveType::I64, PrimitiveType::U64, PrimitiveType::F32, PrimitiveType::F64, PrimitiveType::Ptr
		},
		// uNT8
		{
			PrimitiveType::Unknown, PrimitiveType::I32, PrimitiveType::I32, PrimitiveType::I32, PrimitiveType::I32, PrimitiveType::I32, PrimitiveType::I32,
			PrimitiveType::U32, PrimitiveType::I64, PrimitiveType::U64, PrimitiveType::F32, PrimitiveType::F64, PrimitiveType::Ptr
		},
		// INT16
		{
			PrimitiveType::Unknown, PrimitiveType::I32, PrimitiveType::I32, PrimitiveType::I32, PrimitiveType::I32, PrimitiveType::I32, PrimitiveType::I32,
			PrimitiveType::U32, PrimitiveType::I64, PrimitiveType::U64, PrimitiveType::F32, PrimitiveType::F64, PrimitiveType::Ptr
		},
		// uNT16
		{
			PrimitiveType::Unknown, PrimitiveType::I32, PrimitiveType::I32, PrimitiveType::I32, PrimitiveType::I32, PrimitiveType::I32, PrimitiveType::I32,
			PrimitiveType::U32, PrimitiveType::I64, PrimitiveType::U64, PrimitiveType::F32, PrimitiveType::F64, PrimitiveType::Ptr
		},
		// INT32
		{
			PrimitiveType::Unknown, PrimitiveType::I32, PrimitiveType::I32, PrimitiveType::I32, PrimitiveType::I32, PrimitiveType::I32, PrimitiveType::I32,
			PrimitiveType::U32, PrimitiveType::I64, PrimitiveType::U64, PrimitiveType::F32, PrimitiveType::F64, PrimitiveType::Ptr
		},
		// uNT32
		{
			PrimitiveType::Unknown, PrimitiveType::U32, PrimitiveType::U32, PrimitiveType::U32, PrimitiveType::U32, PrimitiveType::U32, PrimitiveType::U32,
			PrimitiveType::U32, PrimitiveType::I64, PrimitiveType::U64, PrimitiveType::F32, PrimitiveType::F64, PrimitiveType::Ptr
		},
		// INT64
		{
			PrimitiveType::Unknown, PrimitiveType::I64, PrimitiveType::I64, PrimitiveType::I64, PrimitiveType::I64, PrimitiveType::I64, PrimitiveType::I64,
			PrimitiveType::I64, PrimitiveType::I64, PrimitiveType::U64, PrimitiveType::F32, PrimitiveType::F64, PrimitiveType::Ptr
		},
		// uNT64
		{
			PrimitiveType::Unknown, PrimitiveType::U64, PrimitiveType::U64, PrimitiveType::U64, PrimitiveType::U64, PrimitiveType::U64, PrimitiveType::U64,
			PrimitiveType::U64, PrimitiveType::U64, PrimitiveType::U64, PrimitiveType::F32, PrimitiveType::F64, PrimitiveType::Ptr
		},
		// FLOAT32
		{
			PrimitiveType::Unknown, PrimitiveType::F32, PrimitiveType::F32, PrimitiveType::F32, PrimitiveType::F32, PrimitiveType::F32, PrimitiveType::F32,
			PrimitiveType::F32, PrimitiveType::F32, PrimitiveType::F32, PrimitiveType::F32, PrimitiveType::F64, PrimitiveType::Ptr
		},
		// FLOAT64
		{
			PrimitiveType::Unknown, PrimitiveType::F64, PrimitiveType::F64, PrimitiveType::F64, PrimitiveType::F64, PrimitiveType::F64, PrimitiveType::F64,
			PrimitiveType::F64, PrimitiveType::F64, PrimitiveType::F64, PrimitiveType::F64, PrimitiveType::F64, PrimitiveType::Ptr
		},
		// PTR
		{
			PrimitiveType::Unknown, PrimitiveType::Ptr, PrimitiveType::Ptr, PrimitiveType::Ptr, PrimitiveType::Ptr, PrimitiveType::Ptr, PrimitiveType::Ptr,
			PrimitiveType::Ptr, PrimitiveType::Ptr, PrimitiveType::Ptr, PrimitiveType::Unknown, PrimitiveType::Unknown, PrimitiveType::Ptr
		}
	};

	if (lhs <= PrimitiveType::Unknown || lhs >= PrimitiveType::Count)
		return PrimitiveType::Unknown;
	if (rhs <= PrimitiveType::Unknown || rhs >= PrimitiveType::Count)
		return PrimitiveType::Unknown;
	return types[(::I32)lhs][(::I32)rhs];
}

// A function pointer that represents when we want to combine two constants
typedef bool(*PrimitiveValue_combine_fn)(PrimitiveValue* lhs, const PrimitiveValue* rhs);

bool PrimitiveValue_not_implemented(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	ASSERT_NOT_IMPLEMENTED();
	return false;
}

bool PrimitiveValue_not_allowed(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	return false;
}

#pragma region PrimitiveValue Add


////////////////////// 
//////////// INT8
/////////////////////

bool PrimitiveValue_add_i1_bool(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	return PrimitiveValue_not_allowed(lhs, rhs);
}

bool PrimitiveValue_add_i1_i1(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->i32 = lhs->i8 + rhs->i8;
	return true;
}

bool PrimitiveValue_add_i1_u1(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->i32 = lhs->i8 + rhs->u8;
	return true;
}

bool PrimitiveValue_add_i1_i2(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->i32 = lhs->i8 + rhs->i16;
	return true;
}

bool PrimitiveValue_add_i1_u2(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->i32 = lhs->i8 + rhs->u16;
	return true;
}

bool PrimitiveValue_add_i1_i4(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->i32 = lhs->i8 + rhs->i32;
	return true;
}

bool PrimitiveValue_add_i1_u4(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->u32 = lhs->i8 + rhs->u32;
	return true;
}

bool PrimitiveValue_add_i1_i8(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->i64 = lhs->i8 + rhs->i64;
	return true;
}

bool PrimitiveValue_add_i1_u8(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->u64 = lhs->i8 + rhs->u64;
	return true;
}

bool PrimitiveValue_add_i1_f4(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->f32 = lhs->i8 + rhs->f32;
	return true;
}

bool PrimitiveValue_add_i1_f8(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->f64 = lhs->i8 + rhs->f64;
	return true;
}

bool PrimitiveValue_add_i1_ptr(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->ptr = (size_t)lhs->i8 + rhs->ptr;
	return true;
}

////////////////////// 
//////////// UNSIGNED INT8
/////////////////////

bool PrimitiveValue_add_u1_bool(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	return PrimitiveValue_not_allowed(lhs, rhs);
}

bool PrimitiveValue_add_u1_i1(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->i32 = lhs->u8 + rhs->i8;
	return true;
}

bool PrimitiveValue_add_u1_u1(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->i32 = lhs->u8 + rhs->u8;
	return true;
}

bool PrimitiveValue_add_u1_i2(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->i32 = lhs->u8 + rhs->i16;
	return true;
}

bool PrimitiveValue_add_u1_u2(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->i32 = lhs->u8 + rhs->u16;
	return true;
}

bool PrimitiveValue_add_u1_i4(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->i32 = lhs->u8 + rhs->i32;
	return true;
}

bool PrimitiveValue_add_u1_u4(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->u32 = lhs->u8 + rhs->u32;
	return true;
}

bool PrimitiveValue_add_u1_i8(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->i64 = lhs->u8 + rhs->i64;
	return true;
}

bool PrimitiveValue_add_u1_u8(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->u64 = lhs->u8 + rhs->u64;
	return true;
}

bool PrimitiveValue_add_u1_f4(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->f32 = lhs->u8 + rhs->f32;
	return true;
}

bool PrimitiveValue_add_u1_f8(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->f64 = lhs->u8 + rhs->f64;
	return true;
}

bool PrimitiveValue_add_u1_ptr(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->ptr = (size_t)lhs->u8 + rhs->ptr;
	return true;
}

////////////////////// 
//////////// INT16
/////////////////////

bool PrimitiveValue_add_i2_bool(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	return PrimitiveValue_not_allowed(lhs, rhs);
}

bool PrimitiveValue_add_i2_i1(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->i16 = lhs->i16 + rhs->i8;
	return true;
}

bool PrimitiveValue_add_i2_u1(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->i16 = lhs->i16 + rhs->u8;
	return true;
}

bool PrimitiveValue_add_i2_i2(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->i16 = lhs->i16 + rhs->i16;
	return true;
}

bool PrimitiveValue_add_i2_u2(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->u16 = lhs->i16 + rhs->u16;
	return true;
}

bool PrimitiveValue_add_i2_i4(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->i32 = lhs->i16 + rhs->i32;
	return true;
}

bool PrimitiveValue_add_i2_u4(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->u32 = lhs->i16 + rhs->u32;
	return true;
}

bool PrimitiveValue_add_i2_i8(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->i64 = lhs->i16 + rhs->i64;
	return true;
}

bool PrimitiveValue_add_i2_u8(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->u64 = lhs->i16 + rhs->u64;
	return true;
}

bool PrimitiveValue_add_i2_f4(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->f32 = lhs->i16 + rhs->f32;
	return true;
}

bool PrimitiveValue_add_i2_f8(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->f64 = lhs->i16 + rhs->f64;
	return true;
}

bool PrimitiveValue_add_i2_ptr(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->ptr = (size_t)lhs->i16 + rhs->ptr;
	return true;
}

////////////////////// 
//////////// UNSIGNED INT16
/////////////////////

bool PrimitiveValue_add_u2_bool(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	return PrimitiveValue_not_allowed(lhs, rhs);
}

bool PrimitiveValue_add_u2_i1(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->u16 = lhs->u16 + rhs->i8;
	return true;
}

bool PrimitiveValue_add_u2_u1(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->u16 = lhs->u16 + rhs->u8;
	return true;
}

bool PrimitiveValue_add_u2_i2(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->i16 = lhs->u16 + rhs->i16;
	return true;
}

bool PrimitiveValue_add_u2_u2(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->u16 = lhs->u16 + rhs->u16;
	return true;
}

bool PrimitiveValue_add_u2_i4(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->i32 = lhs->u16 + rhs->i32;
	return true;
}

bool PrimitiveValue_add_u2_u4(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->u32 = lhs->u16 + rhs->u32;
	return true;
}

bool PrimitiveValue_add_u2_i8(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->i64 = lhs->u16 + rhs->i64;
	return true;
}

bool PrimitiveValue_add_u2_u8(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->u64 = lhs->u16 + rhs->u64;
	return true;
}

bool PrimitiveValue_add_u2_f4(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->f32 = lhs->u16 + rhs->f32;
	return true;
}

bool PrimitiveValue_add_u2_f8(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->f64 = lhs->u16 + rhs->f64;
	return true;
}

bool PrimitiveValue_add_u2_ptr(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->ptr = (size_t)lhs->u16 + rhs->ptr;
	return true;
}

////////////////////// 
//////////// INT32
/////////////////////

bool PrimitiveValue_add_i4_bool(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	return PrimitiveValue_not_allowed(lhs, rhs);
}

bool PrimitiveValue_add_i4_i1(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->i32 = lhs->i32 + rhs->i8;
	return true;
}

bool PrimitiveValue_add_i4_u1(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->i32 = lhs->i32 + rhs->u8;
	return true;
}

bool PrimitiveValue_add_i4_i2(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->i32 = lhs->i32 + rhs->i16;
	return true;
}

bool PrimitiveValue_add_i4_u2(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->i32 = lhs->i32 + rhs->u16;
	return true;
}

bool PrimitiveValue_add_i4_i4(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->i32 = lhs->i32 + rhs->i32;
	return true;
}

bool PrimitiveValue_add_i4_u4(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->u32 = lhs->i32 + rhs->u32;
	return true;
}

bool PrimitiveValue_add_i4_i8(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->i64 = lhs->i32 + rhs->i64;
	return true;
}

bool PrimitiveValue_add_i4_u8(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->u64 = lhs->i32 + rhs->u64;
	return true;
}

bool PrimitiveValue_add_i4_f4(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->f32 = lhs->i32 + rhs->f32;
	return true;
}

bool PrimitiveValue_add_i4_f8(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->f64 = lhs->i32 + rhs->f64;
	return true;
}

bool PrimitiveValue_add_i4_ptr(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->ptr = (size_t)lhs->i32 + rhs->ptr;
	return true;
}

////////////////////// 
//////////// UNSIGNED INT32
/////////////////////

bool PrimitiveValue_add_u4_bool(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	return PrimitiveValue_not_allowed(lhs, rhs);
}

bool PrimitiveValue_add_u4_i1(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->u32 = lhs->u32 + rhs->i8;
	return true;
}

bool PrimitiveValue_add_u4_u1(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->u32 = lhs->u32 + rhs->u8;
	return true;
}

bool PrimitiveValue_add_u4_i2(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->u32 = lhs->u32 + rhs->i16;
	return true;
}

bool PrimitiveValue_add_u4_u2(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->u32 = lhs->u32 + rhs->u16;
	return true;
}

bool PrimitiveValue_add_u4_i4(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->i32 = lhs->u32 + rhs->i32;
	return true;
}

bool PrimitiveValue_add_u4_u4(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->u32 = lhs->u32 + rhs->u32;
	return true;
}

bool PrimitiveValue_add_u4_i8(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->i64 = lhs->u32 + rhs->i64;
	return true;
}

bool PrimitiveValue_add_u4_u8(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->u64 = lhs->u32 + rhs->u64;
	return true;
}

bool PrimitiveValue_add_u4_f4(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->f32 = lhs->u32 + rhs->f32;
	return true;
}

bool PrimitiveValue_add_u4_f8(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->f64 = lhs->u32 + rhs->f64;
	return true;
}

bool PrimitiveValue_add_u4_ptr(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->ptr = (size_t)lhs->u32 + rhs->ptr;
	return true;
}

////////////////////// 
//////////// INT64
/////////////////////

bool PrimitiveValue_add_i8_bool(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	return PrimitiveValue_not_allowed(lhs, rhs);
}

bool PrimitiveValue_add_i8_i1(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->i64 = lhs->i64 + rhs->i8;
	return true;
}

bool PrimitiveValue_add_i8_u1(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->i64 = lhs->i64 + rhs->u8;
	return true;
}

bool PrimitiveValue_add_i8_i2(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->i64 = lhs->i64 + rhs->i16;
	return true;
}

bool PrimitiveValue_add_i8_u2(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->i64 = lhs->i64 + rhs->u16;
	return true;
}

bool PrimitiveValue_add_i8_i4(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->i64 = lhs->i64 + rhs->i32;
	return true;
}

bool PrimitiveValue_add_i8_u4(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->i64 = lhs->i64 + rhs->u32;
	return true;
}

bool PrimitiveValue_add_i8_i8(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->i64 = lhs->i64 + rhs->i64;
	return true;
}

bool PrimitiveValue_add_i8_u8(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->u64 = lhs->i64 + rhs->u64;
	return true;
}

bool PrimitiveValue_add_i8_f4(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->f32 = lhs->i64 + rhs->f32;
	return true;
}

bool PrimitiveValue_add_i8_f8(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->f64 = lhs->i64 + rhs->f64;
	return true;
}

bool PrimitiveValue_add_i8_ptr(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->ptr = (size_t)lhs->i64 + rhs->ptr;
	return true;
}

////////////////////// 
//////////// INT64
/////////////////////

bool PrimitiveValue_add_u8_bool(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	return PrimitiveValue_not_allowed(lhs, rhs);
}

bool PrimitiveValue_add_u8_i1(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->u64 = lhs->u64 + rhs->i8;
	return true;
}

bool PrimitiveValue_add_u8_u1(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->u64 = lhs->u64 + rhs->u8;
	return true;
}

bool PrimitiveValue_add_u8_i2(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->u64 = lhs->u64 + rhs->i16;
	return true;
}

bool PrimitiveValue_add_u8_u2(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->u64 = lhs->u64 + rhs->u16;
	return true;
}

bool PrimitiveValue_add_u8_i4(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->u64 = lhs->u64 + rhs->i32;
	return true;
}

bool PrimitiveValue_add_u8_u4(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->u64 = lhs->u64 + rhs->u32;
	return true;
}

bool PrimitiveValue_add_u8_i8(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->u64 = lhs->u64 + rhs->i64;
	return true;
}

bool PrimitiveValue_add_u8_u8(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->u64 = lhs->u64 + rhs->u64;
	return true;
}

bool PrimitiveValue_add_u8_f4(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->f32 = lhs->u64 + rhs->f32;
	return true;
}

bool PrimitiveValue_add_u8_f8(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->f64 = lhs->u64 + rhs->f64;
	return true;
}

bool PrimitiveValue_add_u8_ptr(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->ptr = (size_t)lhs->u64 + rhs->ptr;
	return true;
}

////////////////////// 
//////////// FLOAT32
/////////////////////

bool PrimitiveValue_add_f4_bool(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	return PrimitiveValue_not_allowed(lhs, rhs);
}

bool PrimitiveValue_add_f4_i1(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->f32 = lhs->f32 + rhs->i8;
	return true;
}

bool PrimitiveValue_add_f4_u1(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->f32 = lhs->f32 + rhs->u8;
	return true;
}

bool PrimitiveValue_add_f4_i2(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->f32 = lhs->f32 + rhs->i16;
	return true;
}

bool PrimitiveValue_add_f4_u2(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->f32 = lhs->f32 + rhs->u16;
	return true;
}

bool PrimitiveValue_add_f4_i4(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->f32 = lhs->f32 + rhs->i32;
	return true;
}

bool PrimitiveValue_add_f4_u4(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->f32 = lhs->f32 + rhs->u32;
	return true;
}

bool PrimitiveValue_add_f4_i8(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->f32 = lhs->f32 + rhs->i64;
	return true;
}

bool PrimitiveValue_add_f4_u8(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->f32 = lhs->f32 + rhs->u64;
	return true;
}

bool PrimitiveValue_add_f4_f4(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->f32 = lhs->f32 + rhs->f32;
	return true;
}

bool PrimitiveValue_add_f4_f8(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->f64 = lhs->f32 + rhs->f64;
	return true;
}

bool PrimitiveValue_add_f4_ptr(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	return PrimitiveValue_not_allowed(lhs, rhs);
}

////////////////////// 
//////////// FLOAT64
/////////////////////

bool PrimitiveValue_add_f8_bool(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	return PrimitiveValue_not_allowed(lhs, rhs);
}

bool PrimitiveValue_add_f8_i1(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->f64 = lhs->f64 + rhs->i8;
	return true;
}

bool PrimitiveValue_add_f8_u1(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->f64 = lhs->f64 + rhs->u8;
	return true;
}

bool PrimitiveValue_add_f8_i2(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->f64 = lhs->f64 + rhs->i16;
	return true;
}

bool PrimitiveValue_add_f8_u2(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->f64 = lhs->f64 + rhs->u16;
	return true;
}

bool PrimitiveValue_add_f8_i4(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->f64 = lhs->f64 + rhs->i32;
	return true;
}

bool PrimitiveValue_add_f8_u4(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->f64 = lhs->f64 + rhs->u32;
	return true;
}

bool PrimitiveValue_add_f8_i8(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->f64 = lhs->f64 + rhs->i64;
	return true;
}

bool PrimitiveValue_add_f8_u8(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->f64 = lhs->f64 + rhs->u64;
	return true;
}

bool PrimitiveValue_add_f8_f4(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->f64 = lhs->f64 + rhs->f32;
	return true;
}

bool PrimitiveValue_add_f8_f8(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	lhs->f64 = lhs->f64 + rhs->f64;
	return true;
}

bool PrimitiveValue_add_f8_ptr(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	return PrimitiveValue_not_allowed(lhs, rhs);
}

bool PrimitiveValue::Add(PrimitiveValue* lhs, const PrimitiveValue* rhs)
{
	static constexpr auto count = (::I32)PrimitiveType::Count;

	static const PrimitiveValue_combine_fn functions[count][count] = {
		// UNKNOWN
		{
			PrimitiveValue_not_allowed,
			PrimitiveValue_not_allowed,
			PrimitiveValue_not_allowed, PrimitiveValue_not_allowed,
			PrimitiveValue_not_allowed, PrimitiveValue_not_allowed,
			PrimitiveValue_not_allowed, PrimitiveValue_not_allowed,
			PrimitiveValue_not_allowed, PrimitiveValue_not_allowed,
			PrimitiveValue_not_allowed, PrimitiveValue_not_allowed,
			PrimitiveValue_not_allowed
		},
		// bool
		{
			PrimitiveValue_not_allowed,
			PrimitiveValue_not_allowed,
			PrimitiveValue_not_allowed, PrimitiveValue_not_allowed,
			PrimitiveValue_not_allowed, PrimitiveValue_not_allowed,
			PrimitiveValue_not_allowed, PrimitiveValue_not_allowed,
			PrimitiveValue_not_allowed, PrimitiveValue_not_allowed,
			PrimitiveValue_not_allowed, PrimitiveValue_not_allowed,
			PrimitiveValue_not_allowed
		},
		// INT8
		{
			PrimitiveValue_not_allowed,
			PrimitiveValue_add_i1_bool,
			PrimitiveValue_add_i1_i1, PrimitiveValue_add_i1_u1,
			PrimitiveValue_add_i1_i2, PrimitiveValue_add_i1_u2,
			PrimitiveValue_add_i1_i4, PrimitiveValue_add_i1_u4,
			PrimitiveValue_add_i1_i8, PrimitiveValue_add_i1_u8,
			PrimitiveValue_add_i1_f4, PrimitiveValue_add_i1_f8,
			PrimitiveValue_add_i1_ptr
		},
		// uNT8
		{
			PrimitiveValue_not_allowed,
			PrimitiveValue_add_u1_bool,
			PrimitiveValue_add_u1_i1, PrimitiveValue_add_u1_u1,
			PrimitiveValue_add_u1_i2, PrimitiveValue_add_u1_u2,
			PrimitiveValue_add_u1_i4, PrimitiveValue_add_u1_u4,
			PrimitiveValue_add_u1_i8, PrimitiveValue_add_u1_u8,
			PrimitiveValue_add_u1_f4, PrimitiveValue_add_u1_f8,
			PrimitiveValue_add_u1_ptr
		},
		// INT16
		{
			PrimitiveValue_not_allowed,
			PrimitiveValue_add_i2_bool,
			PrimitiveValue_add_i2_i1, PrimitiveValue_add_i2_u1,
			PrimitiveValue_add_i2_i2, PrimitiveValue_add_i2_u2,
			PrimitiveValue_add_i2_i4, PrimitiveValue_add_i2_u4,
			PrimitiveValue_add_i2_i8, PrimitiveValue_add_i2_u8,
			PrimitiveValue_add_i2_f4, PrimitiveValue_add_i2_f8,
			PrimitiveValue_add_i2_ptr
		},
		// uNT16
		{
			PrimitiveValue_not_allowed,
			PrimitiveValue_add_u2_bool,
			PrimitiveValue_add_u2_i1, PrimitiveValue_add_u2_u1,
			PrimitiveValue_add_u2_i2, PrimitiveValue_add_u2_u2,
			PrimitiveValue_add_u2_i4, PrimitiveValue_add_u2_u4,
			PrimitiveValue_add_u2_i8, PrimitiveValue_add_u2_u8,
			PrimitiveValue_add_u2_f4, PrimitiveValue_add_u2_f8,
			PrimitiveValue_add_u2_ptr
		},
		// INT32
		{
			PrimitiveValue_not_allowed,
			PrimitiveValue_add_i4_bool,
			PrimitiveValue_add_i4_i1, PrimitiveValue_add_i4_u1,
			PrimitiveValue_add_i4_i2, PrimitiveValue_add_i4_u2,
			PrimitiveValue_add_i4_i4, PrimitiveValue_add_i4_u4,
			PrimitiveValue_add_i4_i8, PrimitiveValue_add_i4_u8,
			PrimitiveValue_add_i4_f4, PrimitiveValue_add_i4_f8,
			PrimitiveValue_add_i4_ptr
		},
		// uNT32
		{
			PrimitiveValue_not_allowed,
			PrimitiveValue_add_u4_bool,
			PrimitiveValue_add_u4_i1, PrimitiveValue_add_u4_u1,
			PrimitiveValue_add_u4_i2, PrimitiveValue_add_u4_u2,
			PrimitiveValue_add_u4_i4, PrimitiveValue_add_u4_u4,
			PrimitiveValue_add_u4_i8, PrimitiveValue_add_u4_u8,
			PrimitiveValue_add_u4_f4, PrimitiveValue_add_u4_f8,
			PrimitiveValue_add_u4_ptr
		},
		// INT64
		{
			PrimitiveValue_not_allowed,
			PrimitiveValue_add_i8_bool,
			PrimitiveValue_add_i8_i1, PrimitiveValue_add_i8_u1,
			PrimitiveValue_add_i8_i2, PrimitiveValue_add_i8_u2,
			PrimitiveValue_add_i8_i4, PrimitiveValue_add_i8_u4,
			PrimitiveValue_add_i8_i8, PrimitiveValue_add_i8_u8,
			PrimitiveValue_add_i8_f4, PrimitiveValue_add_i8_f8,
			PrimitiveValue_add_i8_ptr
		},
		// uNT64
		{
			PrimitiveValue_not_allowed,
			PrimitiveValue_add_u8_bool,
			PrimitiveValue_add_u8_i1, PrimitiveValue_add_u8_u1,
			PrimitiveValue_add_u8_i2, PrimitiveValue_add_u8_u2,
			PrimitiveValue_add_u8_i4, PrimitiveValue_add_u8_u4,
			PrimitiveValue_add_u8_i8, PrimitiveValue_add_u8_u8,
			PrimitiveValue_add_u8_f4, PrimitiveValue_add_u8_f8,
			PrimitiveValue_add_u8_ptr
		},
		// FLOAT32
		{
			PrimitiveValue_not_allowed,
			PrimitiveValue_add_f4_bool,
			PrimitiveValue_add_f4_i1, PrimitiveValue_add_f4_u1,
			PrimitiveValue_add_f4_i2, PrimitiveValue_add_f4_u2,
			PrimitiveValue_add_f4_i4, PrimitiveValue_add_f4_u4,
			PrimitiveValue_add_f4_i8, PrimitiveValue_add_f4_u8,
			PrimitiveValue_add_f4_f4, PrimitiveValue_add_f4_f8,
			PrimitiveValue_add_f4_ptr
		},
		// FLOAT64
		{
			PrimitiveValue_not_allowed,
			PrimitiveValue_add_f8_bool,
			PrimitiveValue_add_f8_i1, PrimitiveValue_add_f8_u1,
			PrimitiveValue_add_f8_i2, PrimitiveValue_add_f8_u2,
			PrimitiveValue_add_f8_i4, PrimitiveValue_add_f8_u4,
			PrimitiveValue_add_f8_i8, PrimitiveValue_add_f8_u8,
			PrimitiveValue_add_f8_f4, PrimitiveValue_add_f8_f8,
			PrimitiveValue_add_f8_ptr
		},
		// PTR
		{
			PrimitiveValue_not_implemented,
			PrimitiveValue_not_implemented,
			PrimitiveValue_not_implemented, PrimitiveValue_not_implemented,
			PrimitiveValue_not_implemented, PrimitiveValue_not_implemented,
			PrimitiveValue_not_implemented, PrimitiveValue_not_implemented,
			PrimitiveValue_not_implemented, PrimitiveValue_not_implemented,
			PrimitiveValue_not_implemented, PrimitiveValue_not_implemented,
			PrimitiveValue_not_implemented
		}
	};

	if (lhs->type <= PrimitiveType::Unknown || lhs->type >= PrimitiveType::Count)
		return false;
	if (rhs->type <= PrimitiveType::Unknown || rhs->type >= PrimitiveType::Count)
		return false;
	if (!functions[(::I32)lhs->type][(::I32)rhs->type](lhs, rhs))
		return false;
	lhs->type = PrimitiveValue::Result(lhs->type, rhs->type);
	return true;
}

#pragma endregion

bool PrimitiveValue::Neg(PrimitiveValue* lhs)
{
	switch (lhs->type)
	{
	case PrimitiveType::I8:
		lhs->i8 = -lhs->i8;
		return true;
	case PrimitiveType::I16:
		lhs->i16 = -lhs->i16;
		return true;
	case PrimitiveType::I32:
		lhs->i32 = -lhs->i32;
		return true;
	case PrimitiveType::I64:
		lhs->i64 = -lhs->i64;
		return true;
	case PrimitiveType::F32:
		lhs->f32 = -lhs->f32;
		return true;
	case PrimitiveType::F64:
		lhs->f64 = -lhs->f64;
		return true;
	default:
		return false;
	}
}

bool PrimitiveValue::Not(PrimitiveValue* lhs)
{
	switch (lhs->type)
	{
	case PrimitiveType::Bool:
		lhs->bool4 = (lhs->bool4 == 1) ? 0 : 1;
		return true;
	default:
		return false;
	}
}
