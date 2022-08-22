#pragma once

#include "test.hpp"

template<typename T>
struct uitls_vm_type
{
};

template<>
struct uitls_vm_type<vm_int8>
{
	static constexpr auto name = "int8";
	static constexpr auto ptr = "*int8";
	static constexpr auto shorthand = "i1";
	static constexpr auto memory = "1";
	static constexpr auto props1 = VMI_INSTR_PROP_INT8;
};

template<>
struct uitls_vm_type<vm_uint8>
{
	static constexpr auto name = "uint8";
	static constexpr auto ptr = "*uint8";
	static constexpr auto shorthand = "ui1";
	static constexpr auto memory = "1";
	static constexpr auto props1 = VMI_INSTR_PROP_UINT8;
};

template<>
struct uitls_vm_type<vm_int16>
{
	static constexpr auto name = "int16";
	static constexpr auto ptr = "*int16";
	static constexpr auto shorthand = "i2";
	static constexpr auto memory = "2";
	static constexpr auto props1 = VMI_INSTR_PROP_INT16;
};

template<>
struct uitls_vm_type<vm_uint16>
{
	static constexpr auto name = "uint16";
	static constexpr auto ptr = "*uint16";
	static constexpr auto shorthand = "ui2";
	static constexpr auto memory = "2";
	static constexpr auto props1 = VMI_INSTR_PROP_UINT16;
};

template<>
struct uitls_vm_type<vm_int32>
{
	static constexpr auto name = "int32";
	static constexpr auto ptr = "*int32";
	static constexpr auto shorthand = "i4";
	static constexpr auto memory = "4";
	static constexpr auto props1 = VMI_INSTR_PROP_INT32;
};

template<>
struct uitls_vm_type<vm_uint32>
{
	static constexpr auto name = "uint32";
	static constexpr auto ptr = "*uint32";
	static constexpr auto shorthand = "ui4";
	static constexpr auto memory = "4";
	static constexpr auto props1 = VMI_INSTR_PROP_UINT32;
};

template<>
struct uitls_vm_type<vm_int64>
{
	static constexpr auto name = "int64";
	static constexpr auto ptr = "*int64";
	static constexpr auto shorthand = "i8";
	static constexpr auto memory = "8";
	static constexpr auto props1 = VMI_INSTR_PROP_INT64;
};

template<>
struct uitls_vm_type<vm_uint64>
{
	static constexpr auto name = "uint64";
	static constexpr auto ptr = "*uint64";
	static constexpr auto shorthand = "ui8";
	static constexpr auto memory = "8";
	static constexpr auto props1 = VMI_INSTR_PROP_UINT64;
};

template<>
struct uitls_vm_type<vm_float32>
{
	static constexpr auto name = "float32";
	static constexpr auto ptr = "*float32";
	static constexpr auto shorthand = "f4";
	static constexpr auto memory = "4";
	static constexpr auto props1 = VMI_INSTR_PROP_FLOAT32;
};

template<>
struct uitls_vm_type<vm_float64>
{
	static constexpr auto name = "float64";
	static constexpr auto ptr = "*float64";
	static constexpr auto shorthand = "f8";
	static constexpr auto memory = "8";
	static constexpr auto props1 = VMI_INSTR_PROP_FLOAT64;
};

template<>
struct uitls_vm_type<vm_int8*>
{
	static constexpr auto name = "*int8";
	static constexpr auto ptr = "**int8";
	static constexpr auto shorthand = "ptr";
#if defined(VM_64BIT)
	static constexpr auto memory = "8";
#else
	static constexpr auto memory = "4";
#endif
	static constexpr auto props1 = VMI_INSTR_PROP_PTR;
};

template<>
struct uitls_vm_type<vm_uint8*>
{
	static constexpr auto name = "*uint8";
	static constexpr auto ptr = "**uint8";
	static constexpr auto shorthand = "ptr";
#if defined(VM_64BIT)
	static constexpr auto memory = "8";
#else
	static constexpr auto memory = "4";
#endif
	static constexpr auto props1 = VMI_INSTR_PROP_PTR;
};

template<>
struct uitls_vm_type<vm_int16*>
{
	static constexpr auto name = "*int16";
	static constexpr auto ptr = "**int16";
	static constexpr auto shorthand = "ptr";
#if defined(VM_64BIT)
	static constexpr auto memory = "8";
#else
	static constexpr auto memory = "4";
#endif
	static constexpr auto props1 = VMI_INSTR_PROP_PTR;
};

template<>
struct uitls_vm_type<vm_uint16*>
{
	static constexpr auto name = "*uint16";
	static constexpr auto ptr = "**uint16";
	static constexpr auto shorthand = "ptr";
#if defined(VM_64BIT)
	static constexpr auto memory = "8";
#else
	static constexpr auto memory = "4";
#endif
	static constexpr auto props1 = VMI_INSTR_PROP_PTR;
};

template<>
struct uitls_vm_type<vm_int32*>
{
	static constexpr auto name = "*int32";
	static constexpr auto ptr = "**int32";
	static constexpr auto shorthand = "ptr";
#if defined(VM_64BIT)
	static constexpr auto memory = "8";
#else
	static constexpr auto memory = "4";
#endif
	static constexpr auto props1 = VMI_INSTR_PROP_PTR;
};

template<>
struct uitls_vm_type<vm_uint32*>
{
	static constexpr auto name = "*uint32";
	static constexpr auto ptr = "**uint32";
	static constexpr auto shorthand = "ptr";
#if defined(VM_64BIT)
	static constexpr auto memory = "8";
#else
	static constexpr auto memory = "4";
#endif
	static constexpr auto props1 = VMI_INSTR_PROP_PTR;
};

template<>
struct uitls_vm_type<vm_int64*>
{
	static constexpr auto name = "*int64";
	static constexpr auto ptr = "**int64";
	static constexpr auto shorthand = "ptr";
#if defined(VM_64BIT)
	static constexpr auto memory = "8";
#else
	static constexpr auto memory = "4";
#endif
	static constexpr auto props1 = VMI_INSTR_PROP_PTR;
};

template<>
struct uitls_vm_type<vm_uint64*>
{
	static constexpr auto name = "*uint64";
	static constexpr auto ptr = "**uint64";
	static constexpr auto shorthand = "ptr";
#if defined(VM_64BIT)
	static constexpr auto memory = "8";
#else
	static constexpr auto memory = "4";
#endif
	static constexpr auto props1 = VMI_INSTR_PROP_PTR;
};

template<>
struct uitls_vm_type<vm_float32*>
{
	static constexpr auto name = "*float32";
	static constexpr auto ptr = "**float32";
	static constexpr auto shorthand = "ptr";
#if defined(VM_64BIT)
	static constexpr auto memory = "8";
#else
	static constexpr auto memory = "4";
#endif
	static constexpr auto props1 = VMI_INSTR_PROP_PTR;
};

template<>
struct uitls_vm_type<vm_float64*>
{
	static constexpr auto name = "*float64";
	static constexpr auto ptr = "**float64";
	static constexpr auto shorthand = "ptr";
#if defined(VM_64BIT)
	static constexpr auto memory = "8";
#else
	static constexpr auto memory = "4";
#endif
	static constexpr auto props1 = VMI_INSTR_PROP_PTR;
};

struct utils_vm : test_utils
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
	static vm_int8 props1() {
		return uitls_vm_type<T>::props1;
	}

	template<typename T>
	static vm_int8 props1_of(T) {
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

	const char* to_string(vm_int8 value) {
		// Just use new but not delete because we are lazy
		// TODO: Make this better
		char* str = new char[64];
		sprintf(str, "%d", (vm_int32)value);
		return str;
	}

	const char* to_string(vm_int16 value) {
		// Just use new but not delete because we are lazy
		// TODO: Make this better
		char* str = new char[64];
		sprintf(str, "%d", (vm_int32)value);
		return str;
	}

	const char* to_string(vm_int32 value) {
		// Just use new but not delete because we are lazy
		// TODO: Make this better
		char* str = new char[64];
		sprintf(str, "%d", value);
		return str;
	}

	const char* to_string(vm_int64 value) {
		// Just use new but not delete because we are lazy
		// TODO: Make this better
		char* str = new char[64];
		sprintf(str, "%lld", value);
		return str;
	}

	const char* to_string(vm_float32 value) {
		// Just use new but not delete because we are lazy
		// TODO: Make this better
		char* str = new char[64];
		sprintf(str, "%f", value);
		return str;
	}

	const char* to_string(vm_float64 value) {
		// Just use new but not delete because we are lazy
		// TODO: Make this better
		char* str = new char[64];
		sprintf(str, "%lf", value);
		return str;
	}

	void verify_stack_size(vmi_thread* t, size_t expected_size)
	{
		const size_t size = (size_t)(t->stack.top - t->stack.blocks);
		if (expected_size != size) {
			throw_(error() << "expected stack size to be " << expected_size << " but was " << size);
		}
	}

	void verify_stack(vmi_thread* t, vm_int32 offset, vm_int8 value)
	{
		const vm_int8* byte = (vm_int8*)(t->stack.blocks + offset);
		if (*byte != value) {
			throw_(error() << "expected stack value at " << offset << " to be " << (vm_int32)value << " but was " << (vm_int32)*byte);
		}
	}

	void verify_stack(vmi_thread* t, vm_int32 offset, vm_uint8 value)
	{
		const vm_uint8* byte = (vm_uint8*)(t->stack.blocks + offset);
		if (*byte != value) {
			throw_(error() << "expected stack value at " << offset << " to be " << (vm_int32)value << " but was " << (vm_int32)*byte);
		}
	}

	void verify_stack(vmi_thread* t, vm_int32 offset, vm_int16 value)
	{
		const vm_int16* byte = (vm_int16*)(t->stack.blocks + offset);
		if (*byte != value) {
			throw_(error() << "expected stack value at " << offset << " to be " << value << " but was " << *byte);
		}
	}

	void verify_stack(vmi_thread* t, vm_int32 offset, vm_uint16 value)
	{
		const vm_uint16* byte = (vm_uint16*)(t->stack.blocks + offset);
		if (*byte != value) {
			throw_(error() << "expected stack value at " << offset << " to be " << value << " but was " << *byte);
		}
	}

	void verify_stack(vmi_thread* t, vm_int32 offset, vm_int32 value)
	{
		const vm_int32* byte = (vm_int32*)(t->stack.blocks + offset);
		if (*byte != value) {
			throw_(error() << "expected stack value at " << offset << " to be " << value << " but was " << *byte);
		}
	}

	void verify_stack(vmi_thread* t, vm_int32 offset, vm_uint32 value)
	{
		const vm_uint32* byte = (vm_uint32*)(t->stack.blocks + offset);
		if (*byte != value) {
			throw_(error() << "expected stack value at " << offset << " to be " << value << " but was " << *byte);
		}
	}

	void verify_stack(vmi_thread* t, vm_int32 offset, vm_int64 value)
	{
		const vm_int64* byte = (vm_int64*)(t->stack.blocks + offset);
		if (*byte != value) {
			throw_(error() << "expected stack value at " << offset << " to be " << value << " but was " << *byte);
		}
	}

	void verify_stack(vmi_thread* t, vm_int32 offset, vm_uint64 value)
	{
		const vm_uint64* byte = (vm_uint64*)(t->stack.blocks + offset);
		if (*byte != value) {
			throw_(error() << "expected stack value at " << offset << " to be " << value << " but was " << *byte);
		}
	}

	void verify_stack(vmi_thread* t, vm_int32 offset, vm_float32 value)
	{
		const vm_float32* byte = (vm_float32*)(t->stack.blocks + offset);
		if (abs(*byte - value) > FLT_EPSILON) {
			throw_(error() << "expected stack value at " << offset << " to be " << value << " but was " << *byte);
		}
	}

	void verify_stack(vmi_thread* t, vm_int32 offset, vm_float64 value)
	{
		const vm_float64* byte = (vm_float64*)(t->stack.blocks + offset);
		if (abs(*byte - value) > DBL_EPSILON) {
			throw_(error() << "expected stack value at " << offset << " to be " << value << " but was " << *byte);
		}
	}
};
