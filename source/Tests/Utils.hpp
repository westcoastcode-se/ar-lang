#pragma once

#include "Test.hpp"

template<PrimitiveType T>
struct UtilsPrimitiveType
{};

template<>
struct UtilsPrimitiveType<PrimitiveType::Bool>
{
	typedef IB Type;
};

template<>
struct UtilsPrimitiveType<PrimitiveType::I8>
{
	typedef I8 Type;
};

template<>
struct UtilsPrimitiveType<PrimitiveType::U8>
{
	typedef U8 Type;
};

template<>
struct UtilsPrimitiveType<PrimitiveType::I16>
{
	typedef I16 Type;
};

template<>
struct UtilsPrimitiveType<PrimitiveType::U16>
{
	typedef U16 Type;
};

template<>
struct UtilsPrimitiveType<PrimitiveType::I32>
{
	typedef I32 Type;
};

template<>
struct UtilsPrimitiveType<PrimitiveType::U32>
{
	typedef U32 Type;
};

template<>
struct UtilsPrimitiveType<PrimitiveType::I64>
{
	typedef I64 Type;
};

template<>
struct UtilsPrimitiveType<PrimitiveType::U64>
{
	typedef U64 Type;
};

template<>
struct UtilsPrimitiveType<PrimitiveType::F32>
{
	typedef F32 Type;
};

template<>
struct UtilsPrimitiveType<PrimitiveType::F64>
{
	typedef F64 Type;
};

template<>
struct UtilsPrimitiveType<PrimitiveType::Ptr>
{
	typedef void* Type;
};

template<typename T>
struct UtilsType
{
};

template<>
struct UtilsType<bool>
{
	static constexpr auto name = "bool";
	static constexpr auto ptr = "*bool";
	static constexpr auto shorthand = "bool";
	static constexpr auto memory = "4";
	static constexpr IncodeProps props1 = (IncodeProps)PrimitiveType::Bool;
};

template<>
struct UtilsType<I8>
{
	static constexpr auto name = "int8";
	static constexpr auto ptr = "*int8";
	static constexpr auto shorthand = "i8";
	static constexpr auto memory = "1";
	static constexpr IncodeProps props1 = (IncodeProps)PrimitiveType::I8;
};

template<>
struct UtilsType<U8>
{
	static constexpr auto name = "uint8";
	static constexpr auto ptr = "*uint8";
	static constexpr auto shorthand = "u8";
	static constexpr auto memory = "1";
	static constexpr IncodeProps props1 = (IncodeProps)PrimitiveType::U8;
};

template<>
struct UtilsType<I16>
{
	static constexpr auto name = "int16";
	static constexpr auto ptr = "*int16";
	static constexpr auto shorthand = "i16";
	static constexpr auto memory = "2";
	static constexpr IncodeProps props1 = (IncodeProps)PrimitiveType::I16;
};

template<>
struct UtilsType<U16>
{
	static constexpr auto name = "uint16";
	static constexpr auto ptr = "*uint16";
	static constexpr auto shorthand = "u16";
	static constexpr auto memory = "2";
	static constexpr IncodeProps props1 = (IncodeProps)PrimitiveType::U16;
};

template<>
struct UtilsType<I32>
{
	static constexpr auto name = "int32";
	static constexpr auto ptr = "*int32";
	static constexpr auto shorthand = "i32";
	static constexpr auto memory = "4";
	static constexpr IncodeProps props1 = (IncodeProps)PrimitiveType::I32;
};

template<>
struct UtilsType<U32>
{
	static constexpr auto name = "uint32";
	static constexpr auto ptr = "*uint32";
	static constexpr auto shorthand = "u32";
	static constexpr auto memory = "4";
	static constexpr IncodeProps props1 = (IncodeProps)PrimitiveType::U32;
};

template<>
struct UtilsType<I64>
{
	static constexpr auto name = "int64";
	static constexpr auto ptr = "*int64";
	static constexpr auto shorthand = "i64";
	static constexpr auto memory = "8";
	static constexpr IncodeProps props1 = (IncodeProps)PrimitiveType::I64;
};

template<>
struct UtilsType<U64>
{
	static constexpr auto name = "uint64";
	static constexpr auto ptr = "*uint64";
	static constexpr auto shorthand = "u64";
	static constexpr auto memory = "8";
	static constexpr IncodeProps props1 = (IncodeProps)PrimitiveType::U64;
};

template<>
struct UtilsType<F32>
{
	static constexpr auto name = "float32";
	static constexpr auto ptr = "*float32";
	static constexpr auto shorthand = "f32";
	static constexpr auto memory = "4";
	static constexpr IncodeProps props1 = (IncodeProps)PrimitiveType::F32;
};

template<>
struct UtilsType<F64>
{
	static constexpr auto name = "float64";
	static constexpr auto ptr = "*float64";
	static constexpr auto shorthand = "f64";
	static constexpr auto memory = "8";
	static constexpr IncodeProps props1 = (IncodeProps)PrimitiveType::F64;
};

template<>
struct UtilsType<void*>
{
	static constexpr auto name = "*void";
	static constexpr auto ptr = "**void";
	static constexpr auto shorthand = "void";
#if defined(ARLANG_64BIT)
	static constexpr auto memory = "8";
#else
	static constexpr auto memory = "4";
#endif
	static constexpr IncodeProps props1 = (IncodeProps)PrimitiveType::Ptr;
};

template<>
struct UtilsType<I8*>
{
	static constexpr auto name = "*int8";
	static constexpr auto ptr = "**int8";
	static constexpr auto shorthand = "ptr";
#if defined(ARLANG_64BIT)
	static constexpr auto memory = "8";
#else
	static constexpr auto memory = "4";
#endif
	static constexpr IncodeProps props1 = (IncodeProps)PrimitiveType::Ptr;
};

template<>
struct UtilsType<U8*>
{
	static constexpr auto name = "*uint8";
	static constexpr auto ptr = "**uint8";
	static constexpr auto shorthand = "ptr";
#if defined(ARLANG_64BIT)
	static constexpr auto memory = "8";
#else
	static constexpr auto memory = "4";
#endif
	static constexpr IncodeProps props1 = (IncodeProps)PrimitiveType::Ptr;
};

template<>
struct UtilsType<I16*>
{
	static constexpr auto name = "*int16";
	static constexpr auto ptr = "**int16";
	static constexpr auto shorthand = "ptr";
#if defined(ARLANG_64BIT)
	static constexpr auto memory = "8";
#else
	static constexpr auto memory = "4";
#endif
	static constexpr IncodeProps props1 = (IncodeProps)PrimitiveType::Ptr;
};

template<>
struct UtilsType<U16*>
{
	static constexpr auto name = "*uint16";
	static constexpr auto ptr = "**uint16";
	static constexpr auto shorthand = "ptr";
#if defined(ARLANG_64BIT)
	static constexpr auto memory = "8";
#else
	static constexpr auto memory = "4";
#endif
	static constexpr IncodeProps props1 = (IncodeProps)PrimitiveType::Ptr;
};

template<>
struct UtilsType<I32*>
{
	static constexpr auto name = "*int32";
	static constexpr auto ptr = "**int32";
	static constexpr auto shorthand = "ptr";
#if defined(ARLANG_64BIT)
	static constexpr auto memory = "8";
#else
	static constexpr auto memory = "4";
#endif
	static constexpr IncodeProps props1 = (IncodeProps)PrimitiveType::Ptr;
};

template<>
struct UtilsType<U32*>
{
	static constexpr auto name = "*uint32";
	static constexpr auto ptr = "**uint32";
	static constexpr auto shorthand = "ptr";
#if defined(ARLANG_64BIT)
	static constexpr auto memory = "8";
#else
	static constexpr auto memory = "4";
#endif
	static constexpr IncodeProps props1 = (IncodeProps)PrimitiveType::Ptr;
};

template<>
struct UtilsType<I64*>
{
	static constexpr auto name = "*int64";
	static constexpr auto ptr = "**int64";
	static constexpr auto shorthand = "ptr";
#if defined(ARLANG_64BIT)
	static constexpr auto memory = "8";
#else
	static constexpr auto memory = "4";
#endif
	static constexpr IncodeProps props1 = (IncodeProps)PrimitiveType::Ptr;
};

template<>
struct UtilsType<U64*>
{
	static constexpr auto name = "*uint64";
	static constexpr auto ptr = "**uint64";
	static constexpr auto shorthand = "ptr";
#if defined(ARLANG_64BIT)
	static constexpr auto memory = "8";
#else
	static constexpr auto memory = "4";
#endif
	static constexpr IncodeProps props1 = (IncodeProps)PrimitiveType::Ptr;
};

template<>
struct UtilsType<F32*>
{
	static constexpr auto name = "*float32";
	static constexpr auto ptr = "**float32";
	static constexpr auto shorthand = "ptr";
#if defined(ARLANG_64BIT)
	static constexpr auto memory = "8";
#else
	static constexpr auto memory = "4";
#endif
	static constexpr IncodeProps props1 = (IncodeProps)PrimitiveType::Ptr;
};

template<>
struct UtilsType<F64*>
{
	static constexpr auto name = "*float64";
	static constexpr auto ptr = "**float64";
	static constexpr auto shorthand = "ptr";
#if defined(ARLANG_64BIT)
	static constexpr auto memory = "8";
#else
	static constexpr auto memory = "4";
#endif
	static constexpr IncodeProps props1 = (IncodeProps)PrimitiveType::Ptr;
};

namespace WestCoastCode::Compilation
{
	static inline std::ostream& operator<< (std::ostream& out, TokenType const& t)
	{
		out << (I32)t;
		return out;
	}

	static inline std::ostream& operator<< (std::ostream& out, TokenModifier const& t)
	{
		out << (I32)t;
		return out;
	}
}

struct TestUtils
{
	static inline ErrorStringStream Error()
	{
		return ErrorStringStream();
	}

	static inline void Throw(const ErrorStringStream& stream)
	{
		stream.Throw();
	}

	static inline void Throw(const char* format, ...)
	{
		char memory[4096];
		va_list argptr;
		va_start(argptr, format);
		vsprintf(memory, format, argptr);
		va_end(argptr);
		throw std::runtime_error(memory);
	}

	template<typename T>
	void AssertEquals(T value, T expected)
	{
		if (value != expected) {
			Throw(Error() << "expected value to be " << expected << " but was " << value);
		}
	}

	template<typename T>
	void AssertNotNull(T* ptr)
	{
		if (ptr == nullptr)
			Throw(Error() << "expected ptr to not be nullptr");
	}

	template<typename T>
	void AssertNull(T* ptr)
	{
		if (ptr != nullptr)
			Throw(Error() << "expected ptr to be nullptr");
	}

	void AssertFalse(bool value)
	{
		if (value)
			Throw(Error() << "expected false");
	}

	void AssertTrue(bool value)
	{
		if (!value)
			Throw(Error() << "expected true");
	}

	template<class H, class T>
	void AssertType(T* in) {
		if (dynamic_cast<H*>(in) == nullptr) {
			Throw(Error() << typeid(T).name() << " is not " << typeid(H).name());
		}
	}

	template<typename T>
	const Char* Name()
	{
		return UtilsType<T>::name;
	}

	template<typename T>
	const Char* NameOf(T)
	{
		return UtilsType<T>::name;
	}

	template<typename T>
	static IncodeProps Props1()
	{
		return UtilsType<T>::props1;
	}

	template<typename T>
	static IncodeProps Props1Of(T)
	{
		return UtilsType<T>::props1;
	}

	const char* ToString(I8 value) {
		// Just use new but not delete because we are lazy
		// TODO: Make this better
		char* str = new char[64];
		sprintf(str, "%d", (I32)value);
		return str;
	}

	const char* ToString(I16 value) {
		// Just use new but not delete because we are lazy
		// TODO: Make this better
		char* str = new char[64];
		sprintf(str, "%d", (I32)value);
		return str;
	}

	const char* ToString(I32 value) {
		// Just use new but not delete because we are lazy
		// TODO: Make this better
		char* str = new char[64];
		sprintf(str, "%d", value);
		return str;
	}

	const char* ToString(I64 value) {
		// Just use new but not delete because we are lazy
		// TODO: Make this better
		char* str = new char[64];
		sprintf(str, "%lld", value);
		return str;
	}

	const char* ToString(F32 value) {
		// Just use new but not delete because we are lazy
		// TODO: Make this better
		char* str = new char[64];
		sprintf(str, "%f", value);
		return str;
	}

	const char* ToString(F64 value) {
		// Just use new but not delete because we are lazy
		// TODO: Make this better
		char* str = new char[64];
		sprintf(str, "%lf", value);
		return str;
	}

	PrimitiveValue Const(I8 value)
	{
		return PrimitiveValue::I8(value);
	}

	PrimitiveValue Const(U8 value)
	{
		return PrimitiveValue::U8(value);
	}

	PrimitiveValue Const(I16 value)
	{
		return PrimitiveValue::I16(value);
	}

	PrimitiveValue Const(U16 value)
	{
		return PrimitiveValue::U16(value);
	}

	PrimitiveValue Const(I32 value)
	{
		return PrimitiveValue::I32(value);
	}

	PrimitiveValue Const(U32 value)
	{
		return PrimitiveValue::U32(value);
	}

	PrimitiveValue Const(I64 value)
	{
		return PrimitiveValue::I64(value);
	}

	PrimitiveValue Const(U64 value)
	{
		return PrimitiveValue::U64(value);
	}

	PrimitiveValue Const(F32 value)
	{
		return PrimitiveValue::F32(value);
	}

	PrimitiveValue Const(F64 value)
	{
		return PrimitiveValue::F64(value);
	}

	PrimitiveValue Const(bool value)
	{
		return PrimitiveValue::Bool(value ? 1 : 0);
	}

	PrimitiveValue Const(void* value)
	{
		return PrimitiveValue::Ptr(value);
	}
};
