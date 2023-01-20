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

	// Convert the supplied primitive type to a string
	static inline const char* ToString(PrimitiveType t)
	{
		switch (t)
		{
		case PrimitiveType::Unknown:
			return "Unknown";
		case PrimitiveType::Bool:
			return "Bool";
		case PrimitiveType::I8:
			return "I8";
		case PrimitiveType::U8:
			return "U8";
		case PrimitiveType::I16:
			return "I16";
		case PrimitiveType::U16:
			return "U16";
		case PrimitiveType::I32:
			return "I32";
		case PrimitiveType::U32:
			return "U32";
		case PrimitiveType::I64:
			return "I64";
		case PrimitiveType::U64:
			return "U64";
		case PrimitiveType::F32:
			return "F32";
		case PrimitiveType::F64:
			return "F64";
		case PrimitiveType::Ptr:
			return "Ptr";
		default:
			return "Unknown";
		}
	}

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

		// Represents an operation that can be done between two primitives
		typedef bool (*OpFn)(PrimitiveValue* lhs, const PrimitiveValue* rhs);

		// Figure out which primitive type is the result of combining
		// the two supplied types
		static PrimitiveType Result(PrimitiveType rhs, PrimitiveType lhs);

		// Add the values and put the result into the lhs value. Returns true
		// if the two primitives are added
		static bool Add(PrimitiveValue* lhs, const PrimitiveValue* rhs);

		// Subtract the values and put the result into the lhs value. Returns true
		// if the two primitives are subtracted
		static bool Sub(PrimitiveValue* lhs, const PrimitiveValue* rhs);

		// Multiply the values and put the result into the lhs value. Returns true
		// if the two primitives are multiplied
		static bool Mult(PrimitiveValue* lhs, const PrimitiveValue* rhs);

		// Divide the values and put the result into the lhs value. Returns true
		// if the two primitives are divided
		static bool Div(PrimitiveValue* lhs, const PrimitiveValue* rhs);

		// Negate the supplied value. Returns true if negation was successfull
		static bool Neg(PrimitiveValue* lhs);

		// Not the supplied value
		static bool Not(PrimitiveValue* lhs);
	};
	static_assert(sizeof(PrimitiveValue) == 16, "Expected the value size to be 16");

	template<typename T>
	struct PrimitiveTypeTraits
	{
		static constexpr auto Name = "?";
		static constexpr auto Primitive = "?";
		static constexpr auto Ptr = "*void";
	};

	template<>
	struct PrimitiveTypeTraits<I8>
	{
		static constexpr auto Name = "I8";
		static constexpr auto Primitive = "int8";
		static constexpr auto Ptr = "*int8";
	};

	template<>
	struct PrimitiveTypeTraits<U8>
	{
		static constexpr auto Name = "U8";
		static constexpr auto Primitive = "uint8";
		static constexpr auto Ptr = "*uint8";
	};

	template<>
	struct PrimitiveTypeTraits<I16>
	{
		static constexpr auto Name = "I16";
		static constexpr auto Primitive = "int16";
		static constexpr auto Ptr = "*int16";
	};

	template<>
	struct PrimitiveTypeTraits<U16>
	{
		static constexpr auto Name = "U16";
		static constexpr auto Primitive = "uint16";
		static constexpr auto Ptr = "*uint16";
	};

	template<>
	struct PrimitiveTypeTraits<I32>
	{
		static constexpr auto Name = "I32";
		static constexpr auto Primitive = "int32";
		static constexpr auto Ptr = "*int32";
	};

	template<>
	struct PrimitiveTypeTraits<U32>
	{
		static constexpr auto Name = "U32";
		static constexpr auto Primitive = "uint32";
		static constexpr auto Ptr = "*uint32";
	};

	template<>
	struct PrimitiveTypeTraits<I64>
	{
		static constexpr auto Name = "I64";
		static constexpr auto Primitive = "int64";
		static constexpr auto Ptr = "*int64";
	};

	template<>
	struct PrimitiveTypeTraits<U64>
	{
		static constexpr auto Name = "U64";
		static constexpr auto Primitive = "uint64";
		static constexpr auto Ptr = "*uint64";
	};

	template<>
	struct PrimitiveTypeTraits<F32>
	{
		static constexpr auto Name = "F32";
		static constexpr auto Primitive = "float32";
		static constexpr auto Ptr = "*float32";
	};

	template<>
	struct PrimitiveTypeTraits<F64>
	{
		static constexpr auto Name = "F64";
		static constexpr auto Primitive = "float64";
		static constexpr auto Ptr = "*float64";
	};

	template<typename T>
	struct PrimitiveTypeTraits<T*>
	{
		static constexpr auto Name = "Ptr";
		static constexpr auto Primitive = PrimitiveTypeTraits<T>::Ptr;
		static constexpr auto Ptr = "**void";
	};
}
