#pragma once

#include "../Common.h"

namespace WestCoastCode::Interpreter
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
	struct PrimitiveValue
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
			void* ptr;
		};
		PrimitiveType type;

		// Create a primitive
		static PrimitiveValue I8(I8 val)
		{
			PrimitiveValue p;
			p.i8 = val;
			p.type = PrimitiveType::I8;
			return p;
		}

		// Create a primitive
		static PrimitiveValue U8(U8 val)
		{
			PrimitiveValue p;
			p.u8 = val;
			p.type = PrimitiveType::U8;
			return p;
		}

		// Create a primitive
		static PrimitiveValue I16(I16 val)
		{
			PrimitiveValue p;
			p.i16 = val;
			p.type = PrimitiveType::I16;
			return p;
		}

		// Create a primitive
		static PrimitiveValue U16(U16 val)
		{
			PrimitiveValue p;
			p.u16 = val;
			p.type = PrimitiveType::U16;
			return p;
		}

		// Create a primitive
		static PrimitiveValue I32(I32 val)
		{
			PrimitiveValue p;
			p.i32 = val;
			p.type = PrimitiveType::I32;
			return p;
		}

		// Create a primitive
		static PrimitiveValue U32(U32 val)
		{
			PrimitiveValue p;
			p.u32 = val;
			p.type = PrimitiveType::U32;
			return p;
		}

		// Create a primitive
		static PrimitiveValue I64(I64 val)
		{
			PrimitiveValue p;
			p.i64 = val;
			p.type = PrimitiveType::I64;
			return p;
		}

		// Create a primitive
		static PrimitiveValue U64(U64 val)
		{
			PrimitiveValue p;
			p.u64 = val;
			p.type = PrimitiveType::U64;
			return p;
		}
		// Create a primitive
		static PrimitiveValue F32(F32 val)
		{
			PrimitiveValue p;
			p.f32 = val;
			p.type = PrimitiveType::F32;
			return p;
		}

		// Create a primitive
		static PrimitiveValue F64(F64 val)
		{
			PrimitiveValue p;
			p.f64 = val;
			p.type = PrimitiveType::F64;
			return p;
		}

		// Create a primitive
		static PrimitiveValue Bool(IB val)
		{
			PrimitiveValue p;
			p.bool4 = val;
			p.type = PrimitiveType::Bool;
			return p;
		}

		// Create a primitive
		static PrimitiveValue Ptr(void* val)
		{
			PrimitiveValue p;
			p.ptr = val;
			p.type = PrimitiveType::Ptr;
			return p;
		}
	};
	static_assert(sizeof(PrimitiveValue) == 16, "Expected the value size to be 16");
}
