#pragma once

#include "test.hpp"

template<int T>
struct utils_vm_datatype
{
};

template<>
struct utils_vm_datatype<ARLANG_PRIMITIVE_BOOL>
{
	typedef arInt32 type;
};

template<>
struct utils_vm_datatype<ARLANG_PRIMITIVE_I8>
{
	typedef arInt8 type;
};

template<>
struct utils_vm_datatype<ARLANG_PRIMITIVE_UI8>
{
	typedef arUint8 type;
};

template<>
struct utils_vm_datatype<ARLANG_PRIMITIVE_I16>
{
	typedef arInt16 type;
};

template<>
struct utils_vm_datatype<ARLANG_PRIMITIVE_UI16>
{
	typedef arUint16 type;
};

template<>
struct utils_vm_datatype<ARLANG_PRIMITIVE_I32>
{
	typedef arInt32 type;
};

template<>
struct utils_vm_datatype<ARLANG_PRIMITIVE_UI32>
{
	typedef arUint32 type;
};

template<>
struct utils_vm_datatype<ARLANG_PRIMITIVE_I64>
{
	typedef arInt64 type;
};

template<>
struct utils_vm_datatype<ARLANG_PRIMITIVE_UI64>
{
	typedef arUint64 type;
};

template<>
struct utils_vm_datatype<ARLANG_PRIMITIVE_F32>
{
	typedef arFloat32 type;
};

template<>
struct utils_vm_datatype<ARLANG_PRIMITIVE_F64>
{
	typedef arFloat64 type;
};

template<>
struct utils_vm_datatype<ARLANG_PRIMITIVE_PTR>
{
	typedef arByte* type;
};

template<typename T>
struct uitls_vm_type
{
};

template<>
struct uitls_vm_type<bool>
{
	static constexpr auto name = "bool";
	static constexpr auto ptr = "*bool";
	static constexpr auto shorthand = "bool";
	static constexpr auto memory = "4";
	static constexpr arPrimitive props1 = ARLANG_PRIMITIVE_BOOL;
};

template<>
struct uitls_vm_type<arInt8>
{
	static constexpr auto name = "int8";
	static constexpr auto ptr = "*int8";
	static constexpr auto shorthand = "i8";
	static constexpr auto memory = "1";
	static constexpr arPrimitive props1 = ARLANG_PRIMITIVE_I8;
};

template<>
struct uitls_vm_type<arUint8>
{
	static constexpr auto name = "uint8";
	static constexpr auto ptr = "*uint8";
	static constexpr auto shorthand = "ui8";
	static constexpr auto memory = "1";
	static constexpr arPrimitive props1 = ARLANG_PRIMITIVE_UI8;
};

template<>
struct uitls_vm_type<arInt16>
{
	static constexpr auto name = "int16";
	static constexpr auto ptr = "*int16";
	static constexpr auto shorthand = "i16";
	static constexpr auto memory = "2";
	static constexpr arPrimitive props1 = ARLANG_PRIMITIVE_I16;
};

template<>
struct uitls_vm_type<arUint16>
{
	static constexpr auto name = "uint16";
	static constexpr auto ptr = "*uint16";
	static constexpr auto shorthand = "ui16";
	static constexpr auto memory = "2";
	static constexpr arPrimitive props1 = ARLANG_PRIMITIVE_UI16;
};

template<>
struct uitls_vm_type<arInt32>
{
	static constexpr auto name = "int32";
	static constexpr auto ptr = "*int32";
	static constexpr auto shorthand = "i32";
	static constexpr auto memory = "4";
	static constexpr arPrimitive props1 = ARLANG_PRIMITIVE_I32;
};

template<>
struct uitls_vm_type<arUint32>
{
	static constexpr auto name = "uint32";
	static constexpr auto ptr = "*uint32";
	static constexpr auto shorthand = "ui32";
	static constexpr auto memory = "4";
	static constexpr arPrimitive props1 = ARLANG_PRIMITIVE_UI32;
};

template<>
struct uitls_vm_type<arInt64>
{
	static constexpr auto name = "int64";
	static constexpr auto ptr = "*int64";
	static constexpr auto shorthand = "i64";
	static constexpr auto memory = "8";
	static constexpr arPrimitive props1 = ARLANG_PRIMITIVE_I64;
};

template<>
struct uitls_vm_type<arUint64>
{
	static constexpr auto name = "uint64";
	static constexpr auto ptr = "*uint64";
	static constexpr auto shorthand = "ui64";
	static constexpr auto memory = "8";
	static constexpr arPrimitive props1 = ARLANG_PRIMITIVE_UI64;
};

template<>
struct uitls_vm_type<arFloat32>
{
	static constexpr auto name = "float32";
	static constexpr auto ptr = "*float32";
	static constexpr auto shorthand = "f32";
	static constexpr auto memory = "4";
	static constexpr arPrimitive props1 = ARLANG_PRIMITIVE_F32;
};

template<>
struct uitls_vm_type<arFloat64>
{
	static constexpr auto name = "float64";
	static constexpr auto ptr = "*float64";
	static constexpr auto shorthand = "f64";
	static constexpr auto memory = "8";
	static constexpr arPrimitive props1 = ARLANG_PRIMITIVE_F64;
};

template<>
struct uitls_vm_type<void*>
{
	static constexpr auto name = "*void";
	static constexpr auto ptr = "**void";
	static constexpr auto shorthand = "void";
#if defined(ARLANG_64BIT)
	static constexpr auto memory = "8";
#else
	static constexpr auto memory = "4";
#endif
	static constexpr arPrimitive props1 = ARLANG_PRIMITIVE_PTR;
};

template<>
struct uitls_vm_type<arInt8*>
{
	static constexpr auto name = "*int8";
	static constexpr auto ptr = "**int8";
	static constexpr auto shorthand = "ptr";
#if defined(ARLANG_64BIT)
	static constexpr auto memory = "8";
#else
	static constexpr auto memory = "4";
#endif
	static constexpr arPrimitive props1 = ARLANG_PRIMITIVE_PTR;
};

template<>
struct uitls_vm_type<arUint8*>
{
	static constexpr auto name = "*uint8";
	static constexpr auto ptr = "**uint8";
	static constexpr auto shorthand = "ptr";
#if defined(ARLANG_64BIT)
	static constexpr auto memory = "8";
#else
	static constexpr auto memory = "4";
#endif
	static constexpr arPrimitive props1 = ARLANG_PRIMITIVE_PTR;
};

template<>
struct uitls_vm_type<arInt16*>
{
	static constexpr auto name = "*int16";
	static constexpr auto ptr = "**int16";
	static constexpr auto shorthand = "ptr";
#if defined(ARLANG_64BIT)
	static constexpr auto memory = "8";
#else
	static constexpr auto memory = "4";
#endif
	static constexpr arPrimitive props1 = ARLANG_PRIMITIVE_PTR;
};

template<>
struct uitls_vm_type<arUint16*>
{
	static constexpr auto name = "*uint16";
	static constexpr auto ptr = "**uint16";
	static constexpr auto shorthand = "ptr";
#if defined(ARLANG_64BIT)
	static constexpr auto memory = "8";
#else
	static constexpr auto memory = "4";
#endif
	static constexpr arPrimitive props1 = ARLANG_PRIMITIVE_PTR;
};

template<>
struct uitls_vm_type<arInt32*>
{
	static constexpr auto name = "*int32";
	static constexpr auto ptr = "**int32";
	static constexpr auto shorthand = "ptr";
#if defined(ARLANG_64BIT)
	static constexpr auto memory = "8";
#else
	static constexpr auto memory = "4";
#endif
	static constexpr arPrimitive props1 = ARLANG_PRIMITIVE_PTR;
};

template<>
struct uitls_vm_type<arUint32*>
{
	static constexpr auto name = "*uint32";
	static constexpr auto ptr = "**uint32";
	static constexpr auto shorthand = "ptr";
#if defined(ARLANG_64BIT)
	static constexpr auto memory = "8";
#else
	static constexpr auto memory = "4";
#endif
	static constexpr arPrimitive props1 = ARLANG_PRIMITIVE_PTR;
};

template<>
struct uitls_vm_type<arInt64*>
{
	static constexpr auto name = "*int64";
	static constexpr auto ptr = "**int64";
	static constexpr auto shorthand = "ptr";
#if defined(ARLANG_64BIT)
	static constexpr auto memory = "8";
#else
	static constexpr auto memory = "4";
#endif
	static constexpr arPrimitive props1 = ARLANG_PRIMITIVE_PTR;
};

template<>
struct uitls_vm_type<arUint64*>
{
	static constexpr auto name = "*uint64";
	static constexpr auto ptr = "**uint64";
	static constexpr auto shorthand = "ptr";
#if defined(ARLANG_64BIT)
	static constexpr auto memory = "8";
#else
	static constexpr auto memory = "4";
#endif
	static constexpr arPrimitive props1 = ARLANG_PRIMITIVE_PTR;
};

template<>
struct uitls_vm_type<arFloat32*>
{
	static constexpr auto name = "*float32";
	static constexpr auto ptr = "**float32";
	static constexpr auto shorthand = "ptr";
#if defined(ARLANG_64BIT)
	static constexpr auto memory = "8";
#else
	static constexpr auto memory = "4";
#endif
	static constexpr arPrimitive props1 = ARLANG_PRIMITIVE_PTR;
};

template<>
struct uitls_vm_type<arFloat64*>
{
	static constexpr auto name = "*float64";
	static constexpr auto ptr = "**float64";
	static constexpr auto shorthand = "ptr";
#if defined(ARLANG_64BIT)
	static constexpr auto memory = "8";
#else
	static constexpr auto memory = "4";
#endif
	static constexpr arPrimitive props1 = ARLANG_PRIMITIVE_PTR;
};

struct utils_arlang : test_utils
{
	template<typename T>
	static const char* name() {
		return uitls_vm_type<T>::name;
	}

	template<typename T>
	static const char* name_of(T) {
		return uitls_vm_type<T>::name;
	}

	template<typename T>
	static const char* ptr() {
		return uitls_vm_type<T>::ptr;
	}

	template<typename T>
	static const char* ptr_of(T) {
		return uitls_vm_type<T>::ptr;
	}

	template<typename T>
	static const char* shorthand() {
		return uitls_vm_type<T>::shorthand;
	}

	template<typename T>
	static const char* shorthand_of(T) {
		return uitls_vm_type<T>::shorthand;
	}

	template<typename T>
	static const char* memory() {
		return uitls_vm_type<T>::memory;
	}

	template<typename T>
	static const char* memory_of(T) {
		return uitls_vm_type<T>::memory;
	}

	template<typename T>
	static arPrimitive props1() {
		return uitls_vm_type<T>::props1;
	}

	template<typename T>
	static arPrimitive props1_of(T) {
		return uitls_vm_type<T>::props1;
	}

	template<typename T>
	void assert_equals(T value, T expected)
	{
		if (value != expected) {
			throw_(error() << "expected value to be " << expected << " but was " << value);
		}
	}

	template<typename T>
	void assert_not_null(T* ptr)
	{
		if (ptr == nullptr) {
			throw_(error() << "expected ptr to not be nullptr");
		}
	}

	template<typename T>
	void assert_null(T* ptr)
	{
		if (ptr != nullptr) {
			throw_(error() << "expected ptr to be nullptr");
		}
	}

	template<typename T>
	void verify_value(T value, T expected)
	{
		assert_equals<T>(value, expected);
	}

	template<typename T>
	void verify_not_null(T* ptr)
	{
		assert_not_null<T>(ptr);
	}

	template<typename T>
	void verify_null(T* ptr)
	{
		assert_null<T>(ptr);
	}

	void verify_false(bool value)
	{
		if (value) {
			throw_(error() << "expected false");
		}
	}

	void verify_true(bool value)
	{
		if (!value) {
			throw_(error() << "expected true");
		}
	}

	void beforeEach()
	{
	}

	void afterEach()
	{
	}

	const char* to_string(arInt8 value) {
		// Just use new but not delete because we are lazy
		// TODO: Make this better
		char* str = new char[64];
		sprintf(str, "%d", (arInt32)value);
		return str;
	}

	const char* to_string(arInt16 value) {
		// Just use new but not delete because we are lazy
		// TODO: Make this better
		char* str = new char[64];
		sprintf(str, "%d", (arInt32)value);
		return str;
	}

	const char* to_string(arInt32 value) {
		// Just use new but not delete because we are lazy
		// TODO: Make this better
		char* str = new char[64];
		sprintf(str, "%d", value);
		return str;
	}

	const char* to_string(arInt64 value) {
		// Just use new but not delete because we are lazy
		// TODO: Make this better
		char* str = new char[64];
		sprintf(str, "%lld", value);
		return str;
	}

	const char* to_string(arFloat32 value) {
		// Just use new but not delete because we are lazy
		// TODO: Make this better
		char* str = new char[64];
		sprintf(str, "%f", value);
		return str;
	}

	const char* to_string(arFloat64 value) {
		// Just use new but not delete because we are lazy
		// TODO: Make this better
		char* str = new char[64];
		sprintf(str, "%lf", value);
		return str;
	}

	void verify_stack_size(arThread* t, size_t expected_size)
	{
		const size_t size = (size_t)(t->stack.top - t->stack.blocks);
		if (expected_size != size) {
			throw_(error() << "expected stack size to be " << expected_size << " but was " << size);
		}
	}

	void verify_stack(arThread* t, arInt32 offset, arInt8 value)
	{
		const arInt8* byte = (arInt8*)(t->stack.blocks + offset);
		if (*byte != value) {
			throw_(error() << "expected stack value at " << offset << " to be " << (arInt32)value << " but was " << (arInt32)*byte);
		}
	}

	void verify_stack(arThread* t, arInt32 offset, arUint8 value)
	{
		const arUint8* byte = (arUint8*)(t->stack.blocks + offset);
		if (*byte != value) {
			throw_(error() << "expected stack value at " << offset << " to be " << (arInt32)value << " but was " << (arInt32)*byte);
		}
	}

	void verify_stack(arThread* t, arInt32 offset, arInt16 value)
	{
		const arInt16* byte = (arInt16*)(t->stack.blocks + offset);
		if (*byte != value) {
			throw_(error() << "expected stack value at " << offset << " to be " << value << " but was " << *byte);
		}
	}

	void verify_stack(arThread* t, arInt32 offset, arUint16 value)
	{
		const arUint16* byte = (arUint16*)(t->stack.blocks + offset);
		if (*byte != value) {
			throw_(error() << "expected stack value at " << offset << " to be " << value << " but was " << *byte);
		}
	}

	void verify_stack(arThread* t, arInt32 offset, arInt32 value)
	{
		const arInt32* byte = (arInt32*)(t->stack.blocks + offset);
		if (*byte != value) {
			throw_(error() << "expected stack value at " << offset << " to be " << value << " but was " << *byte);
		}
	}

	void verify_stack(arThread* t, arInt32 offset, arUint32 value)
	{
		const arUint32* byte = (arUint32*)(t->stack.blocks + offset);
		if (*byte != value) {
			throw_(error() << "expected stack value at " << offset << " to be " << value << " but was " << *byte);
		}
	}

	void verify_stack(arThread* t, arInt32 offset, arInt64 value)
	{
		const arInt64* byte = (arInt64*)(t->stack.blocks + offset);
		if (*byte != value) {
			throw_(error() << "expected stack value at " << offset << " to be " << value << " but was " << *byte);
		}
	}

	void verify_stack(arThread* t, arInt32 offset, arUint64 value)
	{
		const arUint64* byte = (arUint64*)(t->stack.blocks + offset);
		if (*byte != value) {
			throw_(error() << "expected stack value at " << offset << " to be " << value << " but was " << *byte);
		}
	}

	void verify_stack(arThread* t, arInt32 offset, arFloat32 value)
	{
		const arFloat32* byte = (arFloat32*)(t->stack.blocks + offset);
		if (abs(*byte - value) > FLT_EPSILON) {
			throw_(error() << "expected stack value at " << offset << " to be " << value << " but was " << *byte);
		}
	}

	void verify_stack(arThread* t, arInt32 offset, arFloat64 value)
	{
		const arFloat64* byte = (arFloat64*)(t->stack.blocks + offset);
		if (abs(*byte - value) > DBL_EPSILON) {
			throw_(error() << "expected stack value at " << offset << " to be " << value << " but was " << *byte);
		}
	}

	arPrimitiveValue to_const(arInt8 value) {
		return arPrimitiveValue_i8(value);
	}

	arPrimitiveValue to_const(arUint8 value) {
		return arPrimitiveValue_ui8(value);
	}

	arPrimitiveValue to_const(arInt16 value) {
		return arPrimitiveValue_i86(value);
	}

	arPrimitiveValue to_const(arUint16 value) {
		return arPrimitiveValue_ui86(value);
	}

	arPrimitiveValue to_const(arInt32 value) {
		return arPrimitiveValue_i32(value);
	}

	arPrimitiveValue to_const(arUint32 value) {
		return arPrimitiveValue_ui32(value);
	}

	arPrimitiveValue to_const(arInt64 value) {
		return arPrimitiveValue_i64(value);
	}

	arPrimitiveValue to_const(arUint64 value) {
		return arPrimitiveValue_ui64(value);
	}

	arPrimitiveValue to_const(arFloat32 value) {
		return arPrimitiveValue_f32(value);
	}

	arPrimitiveValue to_const(arFloat64 value) {
		return arPrimitiveValue_f64(value);
	}
};
