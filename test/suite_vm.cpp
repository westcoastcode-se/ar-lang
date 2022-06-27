#include "test.hpp"

template<typename T>
struct suite_vm_helper
{
};

template<>
struct suite_vm_helper<vm_int8>
{
	static constexpr auto name = "int8";
	static constexpr auto shorthand = "i1";
	static constexpr auto memory = "1";
};

template<>
struct suite_vm_helper<vm_int16>
{
	static constexpr auto name = "int16";
	static constexpr auto shorthand = "i2";
	static constexpr auto memory = "2";
};

template<>
struct suite_vm_helper<vm_int32>
{
	static constexpr auto name = "int32";
	static constexpr auto shorthand = "i4";
	static constexpr auto memory = "4";
};

template<>
struct suite_vm_helper<vm_int64>
{
	static constexpr auto name = "int64";
	static constexpr auto shorthand = "i8";
	static constexpr auto memory = "8";
};

template<>
struct suite_vm_helper<vm_float32>
{
	static constexpr auto name = "float32";
	static constexpr auto shorthand = "f4";
	static constexpr auto memory = "4";
};

template<>
struct suite_vm_helper<vm_float64>
{
	static constexpr auto name = "float64";
	static constexpr auto shorthand = "f8";
	static constexpr auto memory = "8";
};

// Base class for all vm tests
struct suite_vm_utils : test_utils
{
	template<typename T>
	static const char* name() {
		return suite_vm_helper<T>::name;
	}

	template<typename T>
	static const char* name_of(T) {
		return suite_vm_helper<T>::name;
	}

	template<typename T>
	static const char* shorthand() {
		return suite_vm_helper<T>::shorthand;
	}

	template<typename T>
	static const char* shorthand_of(T) {
		return suite_vm_helper<T>::shorthand;
	}

	template<typename T>
	static const char* memory() {
		return suite_vm_helper<T>::memory;
	}

	template<typename T>
	static const char* memory_of(T) {
		return suite_vm_helper<T>::memory;
	}

	void verify_compiler(vmc_compiler* c)
	{
		if (!vmc_compiler_success(c)) {
			error_string_stream e;
			e << "could not compile source code: [";
			auto message = c->messages.first;
			while (message != nullptr) {
				e << "\n" << message->message;
				message = message->next;
			}
			e << "\n]";
			vmc_compiler_destroy(c);
			throw_(e);
		}
	}

	void verify_stack(vmi_thread* t, vm_int32 offset, vm_int8 value)
	{
		const vm_int8* byte = (vm_int8*)(t->stack.blocks + offset);
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

	void verify_stack(vmi_thread* t, vm_int32 offset, vm_int32 value)
	{
		const vm_int32* byte = (vm_int32*)(t->stack.blocks + offset);
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

	void verify_stack_size(vmi_thread* t, size_t expected_size)
	{
		const size_t size = (size_t)(t->stack.top - t->stack.blocks);
		if (expected_size != size) {
			throw_(error() << "expected stack size to be " << expected_size << " but was " << size);
		}
	}

	template<typename T>
	void verify_value(T value, T expected)
	{
		if (value != expected) {
			throw_(error() << "expected value to be " << expected << " but was " << value);
		}
	}

	void push_value(vmi_thread* t, vm_int16 value)
	{
		vmi_thread_push_i16(t, value);
	}

	void push_value(vmi_thread* t, vm_int32 value)
	{
		vmi_thread_push_i32(t, value);
	}

	void push_value(vmi_thread* t, void* ptr)
	{
		vmi_thread_push_ptr(t, ptr);
	}

	vmc_compiler* compile(const vm_byte* src)
	{
		auto const compiler = vmc_compiler_new(NULL);
		vmc_compiler_compile(compiler, src);
		verify_compiler(compiler);
		return compiler;
	}

	void destroy(vmc_compiler* c)
	{
		vmc_compiler_destroy(c);
	}

	void destroy(vmi_process* p)
	{
		vmi_process_destroy(p);
	}

	void destroy(vmi_thread* p)
	{
		vmi_thread_destroy(p);
	}

	void invoke(vmi_process* p, vmi_thread* t)
	{
		invoke(p, t, "Main");
	}

	void invoke(vmi_process* p, vmi_thread* t, const char* entry_point)
	{
		const vmi_package* package = vmi_process_find_package_by_name(p, "main", 4);
		if (package == NULL)
			throw_(error() << "expected 'main' package but was not found");

		const vmi_package_func* func = vmi_package_find_function_by_name(package, entry_point, strlen(entry_point));
		if (func == NULL)
			throw_(error() << "could not find function '" << entry_point << "'");

		const auto result = vmi_process_exec(p, t, func);
		if (result != 0)
			throw_(error() << "error occurred when executing thread: " << result << ". Message: " << t->exit_reason);
	}

	vmi_process* process(vmc_compiler* c)
	{
		auto const p = vmi_process_new();
		if (p == nullptr)
			throw_(error() << "could not create a new VM process");
		const auto result = vmi_process_load(p, vmc_compiler_bytecode(c));
		if (result != 0)
			throw_(error() << "failed to load bytecode because: " << result);
		return p;
	}

	vmi_thread* thread(vmi_process* p)
	{
		auto const t = vmi_thread_new(p);
		if (t == nullptr)
			throw_(error() << "could not spawn a new VM thread");
		return t;
	}
};

// All test functions
struct suite_vm_tests : suite_vm_utils
{
	void calculate_return_two_values()
	{
		const auto source = R"(
fn Get () (int32, int32) {
	// return 123, 456
	ldc_i4 123
	str 0
	ldc_i4 456
	str 1
	ret
}
)";
		auto c = compile(source);
		auto p = process(c);
		auto t = thread(p);

		vmi_thread_push_i32(t, -1);
		vmi_thread_push_i32(t, -1);
		invoke(p, t, "Get");

		verify_stack_size(t, sizeof(vm_int32) * 2);
		verify_stack(t, 0, 456);
		verify_stack(t, 4, 123);

		destroy(t);
		destroy(p);
		destroy(c);
	}

	template<typename T>
	void add_test(const char* type, T lhs, T rhs) {
		const auto format = R"(
fn Add (lhs %s, rhs %s) (%s) {
	// return lhs + rhs
	lda 0
	lda 1
	add %s
	str 0
	ret
}
)";
		char source[1024];
		sprintf(source, format, type, type, type, type);

		auto c = compile(source);
		auto p = process(c);
		auto t = thread(p);

		vmi_thread_reserve_stack(t, sizeof(T));
		push_value(t, (T)rhs);
		push_value(t, (T)lhs);
		invoke(p, t, "Add");

		verify_stack_size(t, sizeof(T));
		verify_stack(t, 0, (T)(lhs + rhs));

		destroy(t);
		destroy(p);
		destroy(c);
	}

	// Add two values of types:
	// * int16
	// * int32
	void add()
	{
		TEST_FN(add_test<vm_int16>("int16", 12, 24));
		TEST_FN(add_test<vm_int32>("int32", 10, 20));
	}

	void calculate_multiple_funcs() {
		/*
fn Add1(lhs int32, rhs int32) (int32) {
	return lhs + rhs
}

fn Add2(lhs int32, rhs int32) (int32) {
	return lhs + rhs
}
*/
		const auto source = R"(
fn Add1 (lhs int32, rhs int32) (int32) {
	lda 0	// Push first arg (4 bytes) to the stack (esp + 0)
	lda 1	// Push second arg (4 bytes) to the stack (esp + 4)
	add int32	// Pop the two top-most i32 values on the stack and push the sum of those values to the stack
	str 0	// Pop the top stack value and put it into the first return value
	ret			// Return to the caller address (assume return value is on the top of the stack)
}

fn Add2 (lhs int32, rhs int32) (int32) {
	lda 0	// Push first arg (4 bytes) to the stack (esp + 0)
	lda 1	// Push second arg (4 bytes) to the stack (esp + 4)
	add int32	// Pop the two top-most i32 values on the stack and push the sum of those values to the stack
	str 0	// Pop the top stack value and put it into the first return value
	ret			// Return to the caller address (assume return value is on the top of the stack)
}
)";
		auto c = compile(source);
		auto p = process(c);
		auto t = thread(p);

		// begin_
		vmi_thread_push_i32(t, 99); // return value here (can be done by the API)
		vmi_thread_push_i32(t, 20);
		vmi_thread_push_i32(t, 10);
		invoke(p, t, "Add2");

		verify_stack_size(t, sizeof(vm_int32));
		verify_stack(t, 0, 30);

		destroy(t);
		destroy(p);
		destroy(c);
	}

	void calculate_two_int32_inner() {
		/*
fn Add(lhs int32, rhs int32) (int32) {
	return lhs + rhs
}

fn AddTwoInts() (int32) {
	return Add(10, 20)
}
*/
		const auto source = R"(
fn Add (lhs int32, rhs int32) (int32) {
	lda 0	// Push first arg (4 bytes) to the stack (esp + 0)
	lda 1	// Push second arg (4 bytes) to the stack (esp + 4)
	add int32	// Pop the two top-most i32 values on the stack and push the sum of those values to the stack
	str 0	// Pop the top stack value and put it into the first return value
	ret			// Return to the caller address (assume return value is on the top of the stack)
}

fn AddTwoInts() (int32) {
	alloc_s 4		// Allocate memory for return value of sizeof(int32)
	ldc_i4 20		// Load constant int32 20
	ldc_i4 10		// Load constant int32 10
	call Add(int32,int32)(int32)
	str 0		// Save the value on the top of the stack to the return position
	ret
}
)";
		auto c = compile(source);
		auto p = process(c);
		auto t = thread(p);

		vmi_thread_push_i32(t, 99); // return value here (can be done by the API)
		invoke(p, t, "AddTwoInts");

		verify_stack_size(t, sizeof(vm_int32));
		verify_stack(t, 0, 30);

		destroy(t);
		destroy(p);
		destroy(c);
	}

	void operator()()
	{
		TEST(calculate_return_two_values);
		TEST(add);
		TEST(calculate_multiple_funcs);
		TEST(calculate_two_int32_inner);
	}
};

struct suite_vm_memory : suite_vm_utils
{
	// Local variable
	void allocate_locals1()
	{
		const auto source = R"(
fn Func() (int32) {
	// var i int32
	locals (i int32)
	// return 5
	ldc_i4 5
	str 0
	ret
}
)";
		auto c = compile(source);
		auto p = process(c);
		auto t = thread(p);

		// Reserve memory for the return value
		vmi_thread_reserve_stack(t, sizeof(vm_int32));
		invoke(p, t, "Func");

		verify_stack_size(t, sizeof(vm_int32));
		verify_stack(t, 0, (vm_int32)5);

		destroy(t);
		destroy(p);
		destroy(c);
	}

	// Local variable
	void allocate_locals2()
	{
		const auto source = R"(
fn InnerFunc() (int32) {
	// var i int32
	locals (i int32)
	// return 5
	ldc_i4 5
	str 0
	ret
}
fn Func() (int32) {
	// return InnerFunc()
	alloc_s 4
	call InnerFunc()(int32)
	str 0
	ret
}
)";
		auto c = compile(source);
		auto p = process(c);
		auto t = thread(p);

		// Reserve memory for the return value
		vmi_thread_reserve_stack(t, sizeof(vm_int32));
		invoke(p, t, "Func");

		verify_stack_size(t, sizeof(vm_int32));
		verify_stack(t, 0, (vm_int32)5);

		destroy(t);
		destroy(p);
		destroy(c);
	}

	// Local variable
	void allocate_load_save_locals1()
	{
		const auto source = R"(
fn Func() (int32) {
	// var i int32
	locals (i int32)
	// i = 10
	ldc_i4 10
	stl 0
	// i += 5
	ldl 0
	ldc_i4 5
	add int32
	stl 0
	// return i
	ldl 0
	str 0
	ret	
}
)";
		auto c = compile(source);
		auto p = process(c);
		auto t = thread(p);

		// Reserve memory for the return value
		vmi_thread_reserve_stack(t, sizeof(vm_int32));
		invoke(p, t, "Func");

		verify_stack_size(t, sizeof(vm_int32));
		verify_stack(t, 0, (vm_int32)15);

		destroy(t);
		destroy(p);
		destroy(c);
	}

	// Local variable
	void allocate_load_save_locals2()
	{
/*
fn InnerFunc(in int32) (int32) {
	var i = 10
	i += in
	return i
}
fn Func() (int32) {
	return InnerFunc(5);
}
*/
		const auto source = R"(
fn InnerFunc(in int32) (int32) {
	// var i int32
	locals (i int32)
	// i = 10
	ldc_i4 10
	stl 0
	// i += 5
	ldl 0
	ldc_i4 5
	add int32
	stl 0
	// return i
	ldl 0
	str 0
	ret
}
fn Func() (int32) {
	// InnerFunc(5)
	alloc_s 4
	ldc_i4 5
	call InnerFunc(int32)(int32)
	str 0
	ret
}
)";
		// push 20
		// push 10
		// call add(int32,int32)(int32)
		//		lda 1
		//		lda 0
		//		add
		//		ret
		// stloc 0
		auto c = compile(source);
		auto p = process(c);
		auto t = thread(p);

		// Reserve memory for the return value
		vmi_thread_reserve_stack(t, sizeof(vm_int32));
		invoke(p, t, "Func");

		verify_stack_size(t, sizeof(vm_int32));
		verify_stack(t, 0, (vm_int32)15);

		destroy(t);
		destroy(p);
		destroy(c);
	}

	template<typename T>
	void copy_s_test(const char* type, T in) {
		/*
				fn Mul2(in <type>) (<type>) {
					return lhs+lhs
				}
		*/
		const auto format = R"(
fn Mul2 (in %s) (%s) {
	lda 0	// Push arg to the stack
	copy_s %s	// Copy value to the stack
	add %s		// Add the two values together
	str 0	// Pop the top stack value and put it into the first return value
	ret			// Return to the caller address (assume return value is on the top of the stack)
}
)";
		char source[1024];
		sprintf(source, format, type, type, type, type);

		auto c = compile(source);
		auto p = process(c);
		auto t = thread(p);

		vmi_thread_reserve_stack(t, sizeof(T));
		push_value(t, (T)in);
		invoke(p, t, "Mul2");

		verify_stack_size(t, sizeof(T));
		verify_stack(t, 0, (T)(in + in));

		destroy(t);
		destroy(p);
		destroy(c);
	}

	void copy_s()
	{
		TEST_FN(copy_s_test<vm_int32>("int32", 10));
	}

	void operator()()
	{
		TEST(allocate_locals1);
		TEST(allocate_locals2);
		TEST(allocate_load_save_locals1);
		TEST(allocate_load_save_locals2);
		TEST(copy_s);
	}
};

struct suite_vm_compare : suite_vm_utils
{
	void clt()
	{
		const auto source = R"(
fn Compare () (int32) {
	// return 12 < 34
	ldc_i4 34
	ldc_i4 12
	clt
	str 0
	ret
}
)";
		auto c = compile(source);
		auto p = process(c);
		auto t = thread(p);

		// Reserve memory for the return value
		vmi_thread_reserve_stack(t, sizeof(vm_int32));
		invoke(p, t, "Compare");

		verify_stack_size(t, sizeof(vm_int32));
		verify_stack(t, 0, 1);

		destroy(t);
		destroy(p);
		destroy(c);
	}

	// Compare greater-then
	void cgt()
	{
		const auto source = R"(
fn Compare () (int32) {
	// return 34 > 12
	ldc_i4 12
	ldc_i4 34
	cgt
	str 0
	ret
}
)";
		auto c = compile(source);
		auto p = process(c);
		auto t = thread(p);

		// Reserve memory for the return value
		vmi_thread_reserve_stack(t, sizeof(vm_int32));
		invoke(p, t, "Compare");

		verify_stack_size(t, sizeof(vm_int32));
		verify_stack(t, 0, 1);

		destroy(t);
		destroy(p);
		destroy(c);
	}

	// Jump if value on the stack is true
	void jmpt()
	{
/*
fn Test() (int32) {
	if 34 > 12 {
		return 10
	} else {
		return 20
	}
}
*/
		const auto source = R"(
fn Test() (int32) {
	ldc_i4 12	// Push a constant
	ldc_i4 34	// Push a constant
	cgt			// Compare 32 > 12
	jmpt marker	// if > jmp marker
	ldc_i4 20
	str 0
	ret			// return 20
#marker 
	ldc_i4 10
	str 0
	ret			// return 10
}
)";
		auto c = compile(source);
		auto p = process(c);
		auto t = thread(p);

		// Reserve memory for the return value
		vmi_thread_reserve_stack(t, sizeof(vm_int32));
		invoke(p, t, "Test");

		verify_stack_size(t, sizeof(vm_int32));
		verify_stack(t, 0, 10);

		destroy(t);
		destroy(p);
		destroy(c);
	}

	// Jump if value on the stack is false
	void jmpf()
	{
		/*
		fn Test() (int32) {
			if 34 < 12 {
				return 10
			} else {
				return 20
			}
		}
		*/
		const auto source = R"(
fn Test() (int32) {
	ldc_i4 12	// Push a constant
	ldc_i4 34	// Push a constant
	clt			// Compare 32 < 12
	jmpt marker	// if > jmp marker
	ldc_i4 20
	str 0
	ret			// return 20
#marker 
	ldc_i4 10
	str 0
	ret			// return 10
}
)";
		auto c = compile(source);
		auto p = process(c);
		auto t = thread(p);

		// Reserve memory for the return value
		vmi_thread_reserve_stack(t, sizeof(vm_int32));
		invoke(p, t, "Test");

		verify_stack_size(t, sizeof(vm_int32));
		verify_stack(t, 0, 20);

		destroy(t);
		destroy(p);
		destroy(c);
	}

	void operator()()
	{
		TEST(clt);
		TEST(cgt);
		TEST(jmpt);
		TEST(jmpf);
	}
};

struct suite_vm_constants : suite_vm_utils
{
	template<typename T>
	void ldc_type(T value) {
		const auto format = R"(
fn Get () (%s) {
	ldc_%s %d
	str 0
	ret
}
)";
		char source[1024];
		sprintf(source, format, name_of(value), shorthand_of(value), value);

		auto c = compile(source);
		auto p = process(c);
		auto t = thread(p);

		vmi_thread_reserve_stack(t, sizeof(T));
		invoke(p, t, "Get");

		verify_stack_size(t, sizeof(T));
		verify_stack(t, 0, (T)value);

		destroy(t);
		destroy(p);
		destroy(c);
	}

	void ldc()
	{
		TEST_FN(ldc_type<vm_int8>(12));
		TEST_FN(ldc_type<vm_int16>(INT16_MAX - 10));
		TEST_FN(ldc_type<vm_int32>(INT32_MAX - 1234));
	}

	void ldc_i64()
	{
		const auto source = R"(
fn Get () (int64) {
	ldc_i8 1234567890
	str 0
	ret
}
)";
		auto c = compile(source);
		auto p = process(c);
		auto t = thread(p);

		vmi_thread_reserve_stack(t, sizeof(vm_int64));
		invoke(p, t, "Get");

		verify_stack_size(t, sizeof(vm_int64));
		verify_stack(t, 0, 1234567890L);

		destroy(t);
		destroy(p);
		destroy(c);
	}

	void ldc_f32()
	{
		const auto source = R"(
fn Get () (float32) {
	ldc_f4 123.67f
	str 0
	ret
}
)";
		auto c = compile(source);
		auto p = process(c);
		auto t = thread(p);

		vmi_thread_reserve_stack(t, sizeof(vm_float32));
		invoke(p, t, "Get");

		verify_stack_size(t, sizeof(vm_float32));
		verify_stack(t, 0, 123.67f);

		destroy(t);
		destroy(p);
		destroy(c);
	}

	void ldc_f64()
	{
		const auto source = R"(
fn Get () (float64) {
	ldc_f8 12345.6789
	str 0
	ret
}
)";
		auto c = compile(source);
		auto p = process(c);
		auto t = thread(p);

		vmi_thread_reserve_stack(t, sizeof(vm_float64));
		invoke(p, t, "Get");

		verify_stack_size(t, sizeof(vm_float64));
		verify_stack(t, 0, 12345.6789);

		destroy(t);
		destroy(p);
		destroy(c);
	}

	void operator()()
	{
		TEST(ldc);
		TEST(ldc_i64);
		TEST(ldc_f32);
		TEST(ldc_f64);
	}
};

struct suite_vm_convert : suite_vm_utils
{
	template<typename FROM, typename TO>
	void conv_test(FROM from, TO to)
	{
		const auto format = R"(
fn Convert () (%s) {
	// return int32(int16(1234))
	ldc_%s %d
	conv_%s_%s
	str 0
	ret
}
)";
		char source[1024];
		sprintf(source, format, name_of(to), shorthand_of(from), from, shorthand_of(from), shorthand_of(to));

		auto c = compile(source);
		auto p = process(c);
		auto t = thread(p);

		// Reserve memory for the return value
		vmi_thread_reserve_stack(t, sizeof(TO));
		invoke(p, t, "Convert");

		verify_stack_size(t, sizeof(TO));
		verify_stack(t, 0, (TO)to);

		destroy(t);
		destroy(p);
		destroy(c);
	}

	// Convert a value from one type to another
	void conv()
	{
		conv_test<vm_int32, vm_int16>(1234, 1234);
		conv_test<vm_int16, vm_int32>(123, 123);
	}

	void operator()()
	{
		TEST(conv);
	}
};

struct suite_vm_pointer : suite_vm_utils
{
	void call_fn_using_pointer()
	{
		const auto source= R"(
fn InnerGet(val *int32) () {
	// *val = 10
	lda 0
	ldc_i4 10
	sturef int32
	ret
}

fn Get () (int32) {
	// var value int32
	locals (value int32)
	// InnerGet(&value)
	ldl_a 0
	call InnerGet(*int32)()
	// return value
	ldl 0
	str 0
	ret
}
)";
		auto c = compile(source);
		auto p = process(c);
		auto t = thread(p);

		vmi_thread_reserve_stack(t, sizeof(vm_int32));
		invoke(p, t, "Get");

		verify_stack_size(t, sizeof(vm_int32));
		verify_stack(t, 0, 10);

		destroy(t);
		destroy(p);
		destroy(c);
	}

	template<typename T>
	void sturef_s_types(T value)
	{
		const auto format = R"(
fn Get(val *%s) () {
	lda 0
	ldc_%s %d
	sturef_s_%s
	ret
}
)";
		char source[1024];
		sprintf(source, format, name_of(value), shorthand_of(value), value, shorthand_of(value));

		auto c = compile(source);
		auto p = process(c);
		auto t = thread(p);

		T result;
		push_value(t, &result);
		invoke(p, t, "Get");

		verify_stack_size(t, 0);
		verify_value(result, (T)value);

		destroy(t);
		destroy(p);
		destroy(c);
	}

	void sturef_s_types_i64()
	{
		const auto source = R"(
fn Get(val *int64) () {
	lda 0
	ldc_i8 1234567890
	sturef_s_i8
	ret
}
)";
		auto c = compile(source);
		auto p = process(c);
		auto t = thread(p);

		vm_int64 result;
		push_value(t, &result);
		invoke(p, t, "Get");

		verify_stack_size(t, 0);
		verify_value(result, (vm_int64)1234567890L);

		destroy(t);
		destroy(p);
		destroy(c);
	}

	void sturef_s_types_f32()
	{
		const auto source = R"(
fn Get(val *float32) () {
	lda 0
	ldc_f4 123.45f
	sturef_s_f4
	ret
}
)";
		auto c = compile(source);
		auto p = process(c);
		auto t = thread(p);

		vm_float32 result;
		push_value(t, &result);
		invoke(p, t, "Get");

		verify_stack_size(t, 0);
		verify_value(result, 123.45f);

		destroy(t);
		destroy(p);
		destroy(c);
	}

	void sturef_s_types_f64()
	{
		const auto source = R"(
fn Get(val *float64) () {
	lda 0
	ldc_f8 12345.67890
	sturef_s_f8
	ret
}
)";
		auto c = compile(source);
		auto p = process(c);
		auto t = thread(p);

		vm_float64 result;
		push_value(t, &result);
		invoke(p, t, "Get");

		verify_stack_size(t, 0);
		verify_value(result, 12345.67890);

		destroy(t);
		destroy(p);
		destroy(c);
	}

	void sturef_s()
	{
		TEST_FN(sturef_s_types<vm_int8>(123));
		TEST_FN(sturef_s_types<vm_int16>(INT16_MAX - 2));
		TEST_FN(sturef_s_types<vm_int32>(INT32_MAX - 1231));
		TEST_FN(sturef_s_types_i64());
		TEST_FN(sturef_s_types_f32());
		TEST_FN(sturef_s_types_f64());
	}

	void operator()()
	{
		TEST(call_fn_using_pointer);
		TEST(sturef_s);
	}
};

struct suite_vm_arrays : suite_vm_utils
{
	void load_and_store_array_value()
	{
		const auto source = R"(
fn Get () (int32) {
	// var value int32
	locals (values [2]int32)
	// values[0] = 10
	ldl_a 0
	ldc_i4 0
	ldc_i4 10
	stelem int32
	// return values[0]
	ldl_a 0
	ldc_i4 0
	ldelem int32
	str 0
	ret
}
)";
		auto c = compile(source);
		auto p = process(c);
		auto t = thread(p);

		vmi_thread_reserve_stack(t, sizeof(vm_int32));
		invoke(p, t, "Get");

		verify_stack_size(t, sizeof(vm_int32));
		verify_stack(t, 0, 10);

		destroy(t);
		destroy(p);
		destroy(c);
	}

	template<typename T>
	void array_test(T value)
	{
		const auto format = R"(
fn Get () (%s) {
	locals (values [2]%s)
	// values[0] = ?
	ldl_a 0
	ldc_i4 0
	ldc_%s %d
	stelem %s
	// return values[0]
	ldl_a 0
	ldc_i4 0
	ldelem %s
	str 0
	ret
}
)";
		char source[1024];
		sprintf(source, format, name_of(value), name_of(value), shorthand_of(value), value, name_of(value), name_of(value));

		auto c = compile(source);
		auto p = process(c);
		auto t = thread(p);

		vmi_thread_reserve_stack(t, sizeof(T));
		invoke(p, t, "Get");

		verify_stack_size(t, sizeof(T));
		verify_stack(t, 0, (T)value);

		destroy(t);
		destroy(p);
		destroy(c);
	}

	void array_test_i64()
	{
		const auto source = R"(
fn Get () (int64) {
	locals (values [2]int64)
	// values[0] = 1234567890
	ldl_a 0
	ldc_i4 0
	ldc_i8 1234567890
	stelem int64
	// return values[0]
	ldl_a 0
	ldc_i4 0
	ldelem int64
	str 0
	ret
}
)";
		auto c = compile(source);
		auto p = process(c);
		auto t = thread(p);

		vmi_thread_reserve_stack(t, sizeof(vm_int64));
		invoke(p, t, "Get");

		verify_stack_size(t, sizeof(vm_int64));
		verify_stack(t, 0, 1234567890L);

		destroy(t);
		destroy(p);
		destroy(c);
	}

	void array_test_f32()
	{
		const auto source = R"(
fn Get () (float32) {
	locals (values [2]float32)
	// values[0] = 123.45f
	ldl_a 0
	ldc_i4 0
	ldc_f4 123.45f
	stelem float32
	// return values[0]
	ldl_a 0
	ldc_i4 0
	ldelem float32
	str 0
	ret
}
)";
		auto c = compile(source);
		auto p = process(c);
		auto t = thread(p);

		vmi_thread_reserve_stack(t, sizeof(vm_float32));
		invoke(p, t, "Get");

		verify_stack_size(t, sizeof(vm_float32));
		verify_stack(t, 0, 123.45f);

		destroy(t);
		destroy(p);
		destroy(c);
	}

	void array_test_f64()
	{
		const auto source = R"(
fn Get () (float64) {
	locals (values [2]float64)
	// values[0] = 12345.6789
	ldl_a 0
	ldc_i4 0
	ldc_f8 12345.6789
	stelem float64
	// return values[0]
	ldl_a 0
	ldc_i4 0
	ldelem float64
	str 0
	ret
}
)";
		auto c = compile(source);
		auto p = process(c);
		auto t = thread(p);

		vmi_thread_reserve_stack(t, sizeof(vm_float64));
		invoke(p, t, "Get");

		verify_stack_size(t, sizeof(vm_float64));
		verify_stack(t, 0, 12345.6789);

		destroy(t);
		destroy(p);
		destroy(c);
	}

	void load_store_types()
	{
		TEST_FN(array_test<vm_int8>(12));
		TEST_FN(array_test<vm_int16>(INT16_MAX - 150));
		TEST_FN(array_test<vm_int32>(INT32_MAX / 2));
		TEST_FN(array_test_i64());
		TEST_FN(array_test_f32());
		TEST_FN(array_test_f64());
	}

	void return_two_values_from_array()
	{
		const auto source = R"(
fn Get () (int32, int32) {
	// var value int32
	locals (values [2]int32)
	// values[0] = 10
	ldl_a 0
	ldc_i4 0
	ldc_i4 10
	stelem int32
	// values[1] = 20
	ldl_a 0
	ldc_i4 1
	ldc_i4 20
	stelem int32
	// return values[0], values[1]
	ldl_a 0
	ldc_i4 0
	ldelem int32
	str 0
	ldl_a 0
	ldc_i4 1
	ldelem int32
	str 1
	ret
}
)";
		auto c = compile(source);
		auto p = process(c);
		auto t = thread(p);

		vmi_thread_reserve_stack(t, sizeof(vm_int32));
		vmi_thread_reserve_stack(t, sizeof(vm_int32));
		invoke(p, t, "Get");

		verify_stack_size(t, sizeof(vm_int32) * 2);
		verify_stack(t, 0, 20);
		verify_stack(t, sizeof(vm_int32), 10);

		destroy(t);
		destroy(p);
		destroy(c);
	}

	void return_array_4int8()
	{
		const auto source = R"(
fn Get () ([4]int8) {
	// var values [4]int8
	locals (values [4]int8)
	// values[0] = 10
	ldl_a 0
	ldc_i4 0
	ldc_i1 10
	stelem int8
	// values[1] = 20
	ldl_a 0
	ldc_i4 1
	ldc_i1 20
	stelem int8
	// values[2] = 30
	ldl_a 0
	ldc_i4 2
	ldc_i1 30
	stelem int8
	// values[3] = 40
	ldl_a 0
	ldc_i4 3
	ldc_i1 40
	stelem int8
	// return values
	ldl 0
	str 0
	ret
}
)";
		auto c = compile(source);
		auto p = process(c);
		auto t = thread(p);

		vm_int8* const ret = (vm_int8*)vmi_thread_reserve_stack(t, sizeof(vm_int8[4]));
		invoke(p, t, "Get");

		verify_stack_size(t, sizeof(vm_int8[4]));
		verify_value(ret[0], (vm_int8)10);
		verify_value(ret[1], (vm_int8)20);
		verify_value(ret[2], (vm_int8)30);
		verify_value(ret[3], (vm_int8)40);

		destroy(t);
		destroy(p);
		destroy(c);
	}

	void return_array_4int32()
	{
		const auto source = R"(
fn Get () ([4]int32) {
	// var values [4]int32
	locals (values [4]int32)
	// values[0] = 10
	ldl_a 0
	ldc_i4 0
	ldc_i4 10
	stelem int32
	// values[1] = 20
	ldl_a 0
	ldc_i4 1
	ldc_i4 20
	stelem int32
	// values[2] = 30
	ldl_a 0
	ldc_i4 2
	ldc_i4 30
	stelem int32
	// values[3] = 40
	ldl_a 0
	ldc_i4 3
	ldc_i4 40
	stelem int32
	// return values
	ldl 0
	str 0
	ret
}
)";
		auto c = compile(source);
		auto p = process(c);
		auto t = thread(p);

		vm_int32* const ret = (vm_int32*)vmi_thread_reserve_stack(t, sizeof(vm_int32[4]));
		invoke(p, t, "Get");

		verify_stack_size(t, sizeof(vm_int32[4]));
		verify_value(ret[0], 10);
		verify_value(ret[1], 20);
		verify_value(ret[2], 30);
		verify_value(ret[3], 40);

		destroy(t);
		destroy(p);
		destroy(c);
	}

	void call_function_with_array_address()
	{
		const auto source = R"(
fn InnerGet(values *int32) () {
	// values[0] = 10
	lda 0
	ldc_i4 0
	ldc_i4 10
	stelem int32
	// values[1] = 20
	lda 0
	ldc_i4 1
	ldc_i4 20
	stelem int32
	// values[2] = 30
	lda 0
	ldc_i4 2
	ldc_i4 30
	stelem int32
	// values[3] = 40
	lda 0
	ldc_i4 3
	ldc_i4 40
	stelem int32
	ret
}

fn Get () ([4]int32) {
	// var values [4]int32
	locals (values [4]int32)
	// InnerGet(&values)
	ldl_a 0
	call InnerGet(*int32)()
	// return values
	ldl 0
	str 0
	ret
}
)";
		auto c = compile(source);
		auto p = process(c);
		auto t = thread(p);

		vm_int32* const ret = (vm_int32*)vmi_thread_reserve_stack(t, sizeof(vm_int32[4]));
		invoke(p, t, "Get");

		verify_stack_size(t, sizeof(vm_int32[4]));
		verify_value(ret[0], 10);
		verify_value(ret[1], 20);
		verify_value(ret[2], 30);
		verify_value(ret[3], 40);

		destroy(t);
		destroy(p);
		destroy(c);
	}

	void call_function_with_array_of_addresses()
	{
		const auto source = R"(
fn InnerGet(values [4]*int32) () {
	// *values[0] = 10
	lda_a 0
	ldc_i4 0
	ldelem *int32
	ldc_i4 10
	sturef_s_i4
	// *values[1] = 20
	lda_a 0
	ldc_i4 1
	ldelem *int32
	ldc_i4 20
	sturef_s_i4
	// *values[2] = 30
	lda_a 0
	ldc_i4 2
	ldelem *int32
	ldc_i4 30
	sturef_s_i4
	// *values[3] = 40
	lda_a 0
	ldc_i4 3
	ldelem *int32
	ldc_i4 40
	sturef_s_i4
	ret
}

fn Get () (int32, int32, int32, int32) {
	// var val0, val1, val2, val3 int32
	// var values [4]*int32
	locals (val0 int32, val1 int32, val2 int32, val3 int32, values [4]*int32)
	// values[0] = &val0
	ldl_a 4
	ldc_i4 0
	ldl_a 0
	stelem *int32
	// values[1] = &val1
	ldl_a 4
	ldc_i4 1
	ldl_a 1
	stelem *int32
	// values[2] = &val2
	ldl_a 4
	ldc_i4 2
	ldl_a 2
	stelem *int32
	// values[3] = &val3
	ldl_a 4
	ldc_i4 3
	ldl_a 3
	stelem *int32
	// InnerGet(values)
	ldl 4
	call InnerGet([4]*int32)()
	// return values
	ldl 0
	str 0
	ldl 1
	str 1
	ldl 2
	str 2
	ldl 3
	str 3
	ret
}
)";
		auto c = compile(source);
		auto p = process(c);
		auto t = thread(p);

		vm_int32* const val3 = (vm_int32*)vmi_thread_reserve_stack(t, sizeof(vm_int32));
		vm_int32* const val2 = (vm_int32*)vmi_thread_reserve_stack(t, sizeof(vm_int32));
		vm_int32* const val1 = (vm_int32*)vmi_thread_reserve_stack(t, sizeof(vm_int32));
		vm_int32* const val0 = (vm_int32*)vmi_thread_reserve_stack(t, sizeof(vm_int32));
		invoke(p, t, "Get");

		verify_stack_size(t, sizeof(vm_int32) * 4);
		verify_value(*val0, 10);
		verify_value(*val1, 20);
		verify_value(*val2, 30);
		verify_value(*val3, 40);

		destroy(t);
		destroy(p);
		destroy(c);
	}

	void operator()()
	{
		TEST(load_and_store_array_value);
		TEST(load_store_types);
		TEST(return_two_values_from_array);
		TEST(return_array_4int8);
		TEST(return_array_4int32);
		TEST(call_function_with_array_address);
		TEST(call_function_with_array_of_addresses);
	}
};

struct suite_vm_stack : suite_vm_utils
{
	template<typename T>
	void alloc_s_free_s()
	{
		
	}

	void alloc_free()
	{
		TEST_FN(alloc_s_free_s<vm_int8>());
		TEST_FN(alloc_s_free_s<vm_int16>());
		TEST_FN(alloc_s_free_s<vm_int32>());
		TEST_FN(alloc_s_free_s<vm_int64>());
		TEST_FN(alloc_s_free_s<vm_float32>());
		TEST_FN(alloc_s_free_s<vm_float64>());
	}

	void operator()()
	{
		//TEST(alloc_free);
	}
};

void suite_vm()
{
	SUITE(suite_vm_tests);
	SUITE(suite_vm_compare);
	SUITE(suite_vm_memory);
	SUITE(suite_vm_constants);
	SUITE(suite_vm_convert);
	SUITE(suite_vm_pointer);
	SUITE(suite_vm_arrays);
	SUITE(suite_vm_stack);
}
