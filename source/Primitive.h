#pragma once

#include "Common.h"

namespace WestCoastCode
{
	enum class PrimitiveType : I32
	{
		Unknown = 0,
		Bool,
		I8,
		U8,
		I16,
		U16,
		I32,
		U32,
		I64,
		U64,
		F32,
		F64,
		Ptr,
		Count
	};

	// Represents a constant primitive value up to 8 bytes
	struct ARLANG_API PrimitiveValue
	{
		// Represents an unknown value up to 8 bytes
		union {
			I64 i64;
			U64 u64;
			F64 f64;
			F32 f32;
			IB bool4;
			I32 i32;
			U32 u32;
			I16 i16;
			U16 u16;
			I8 i8;
			U8 u8;
			Byte* ptr;
		};
		PrimitiveType type;

		// Create a primitive
		static inline PrimitiveValue I8(I8 val)
		{
			PrimitiveValue p = { 0, PrimitiveType::I8 };
			p.i8 = val;
			return p;
		}

		// Create a primitive
		static inline PrimitiveValue U8(U8 val)
		{
			PrimitiveValue p = { 0, PrimitiveType::U8 };
			p.u8 = val;
			return p;
		}

		// Create a primitive
		static inline PrimitiveValue I16(I16 val)
		{
			PrimitiveValue p = { 0, PrimitiveType::I16 };
			p.i16 = val;
			return p;
		}

		// Create a primitive
		static inline PrimitiveValue U16(U16 val)
		{
			PrimitiveValue p = { 0, PrimitiveType::U16 };
			p.u16 = val;
			return p;
		}

		// Create a primitive
		static inline PrimitiveValue I32(I32 val)
		{
			PrimitiveValue p = { 0, PrimitiveType::I32 };
			p.i32 = val;
			return p;
		}

		// Create a primitive
		static inline PrimitiveValue U32(U32 val)
		{
			PrimitiveValue p = { 0, PrimitiveType::U32 };
			p.u32 = val;
			return p;
		}

		// Create a primitive
		static inline PrimitiveValue I64(I64 val)
		{
			PrimitiveValue p = { 0, PrimitiveType::I64 };
			p.i64 = val;
			return p;
		}

		// Create a primitive
		static inline PrimitiveValue U64(U64 val)
		{
			PrimitiveValue p = { 0, PrimitiveType::U64 };
			p.u64 = val;
			return p;
		}
		// Create a primitive
		static inline PrimitiveValue F32(F32 val)
		{
			PrimitiveValue p = { 0, PrimitiveType::F32 };
			p.f32 = val;
			return p;
		}

		// Create a primitive
		static inline PrimitiveValue F64(F64 val)
		{
			PrimitiveValue p = { 0, PrimitiveType::F64 };
			p.f64 = val;
			return p;
		}

		// Create a primitive
		static inline PrimitiveValue Bool(IB val)
		{
			PrimitiveValue p = { 0, PrimitiveType::Bool };
			p.bool4 = val;
			return p;
		}

		// Create a primitive
		static inline PrimitiveValue Ptr(void* val)
		{
			PrimitiveValue p = { 0, PrimitiveType::Ptr };
			p.ptr = (Byte*)val;
			return p;
		}

		// Figure out which primitive type is the result of combining
		// the two supplied types
		static PrimitiveType Result(PrimitiveType rhs, PrimitiveType lhs);

		// Add the values and put the result into the lhs value. Returns true
		// if the two primitives are added
		static bool Add(PrimitiveValue* lhs, const PrimitiveValue* rhs);

		// Negate the supplied value. Returns true if negation was successfull
		static bool Neg(PrimitiveValue* lhs);

		// Not the supplied value
		static bool Not(PrimitiveValue* lhs);
	};
	static_assert(sizeof(PrimitiveValue) == 16, "Expected the value size to be 16");
}
