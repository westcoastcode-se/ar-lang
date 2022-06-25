#include "test.hpp"

template<typename T>
struct suite_vm_helper
{
};

template<>
struct suite_vm_helper<vm_int32>
{
	static constexpr auto name = "int32";
	static constexpr auto shorthand = "i32";
};

template<>
struct suite_vm_helper<vm_int16>
{
	static constexpr auto name = "int16";
	static constexpr auto shorthand = "i16";
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

	void verify_stack(vmi_thread* t, vm_int32 offset, vm_int32 value)
	{
		const vm_int32* byte = (vm_int32*)(t->stack.blocks + offset);
		if (*byte != value) {
			throw_(error() << "expected stack value at " << offset << " to be " << value << " but was " << *byte);
		}
	}

	void verify_stack(vmi_thread* t, vm_int32 offset, vm_int16 value)
	{
		const vm_int16* byte = (vm_int16*)(t->stack.blocks + offset);
		if (*byte != value) {
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

	void push_value(vmi_thread* t, vm_int16 value)
	{
		vmi_thread_push_i16(t, value);
	}

	void push_value(vmi_thread* t, vm_int32 value)
	{
		vmi_thread_push_i32(t, value);
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
	c_i32 123
	save_r 0
	c_i32 456
	save_r 1
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
	load_a 0
	load_a 1
	add %s
	save_r 0
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
		add_test<vm_int16>("int16", 12, 24);
		add_test<vm_int32>("int32", 10, 20);
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
	load_a 0	// Push first arg (4 bytes) to the stack (esp + 0)
	load_a 1	// Push second arg (4 bytes) to the stack (esp + 4)
	add int32	// Pop the two top-most i32 values on the stack and push the sum of those values to the stack
	save_r 0	// Pop the top stack value and put it into the first return value
	ret			// Return to the caller address (assume return value is on the top of the stack)
}

fn Add2 (lhs int32, rhs int32) (int32) {
	load_a 0	// Push first arg (4 bytes) to the stack (esp + 0)
	load_a 1	// Push second arg (4 bytes) to the stack (esp + 4)
	add int32	// Pop the two top-most i32 values on the stack and push the sum of those values to the stack
	save_r 0	// Pop the top stack value and put it into the first return value
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
	load_a 0	// Push first arg (4 bytes) to the stack (esp + 0)
	load_a 1	// Push second arg (4 bytes) to the stack (esp + 4)
	add int32	// Pop the two top-most i32 values on the stack and push the sum of those values to the stack
	save_r 0	// Pop the top stack value and put it into the first return value
	ret			// Return to the caller address (assume return value is on the top of the stack)
}

fn AddTwoInts() (int32) {
	alloc_s 4		// Allocate memory for return value of sizeof(int32)
	c_i32 20		// Load constant int32 20
	c_i32 10		// Load constant int32 10
	call Add(int32,int32)(int32)
	save_r 0		// Save the value on the top of the stack to the return position
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
/*
fn Func() (int32) {
	var i int32
	return 5
}
*/
		const auto source = R"(
fn Func() (int32) {
	locals (i int32)// Allocate local memory storage for an int32 named i
	c_i32 5			// Load 5
	save_r 0
	ret				// Return
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
		/*
		fn InnerFunc() (int32) {
			var i int32
			return 5
		}
		fn Func() (int32) {
			return InnerFunc()
		}
		*/
		const auto source = R"(
fn InnerFunc() (int32) {
	locals (i int32)// Allocate local memory storage for an int32 named i
	c_i32 5			// Load 5
	save_r 0
	ret				// Return
}
fn Func() (int32) {
	alloc_s 4		// Allocate memory for return value of sizeof(int32)
	call InnerFunc()(int32)
	save_r 0
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
		/*
		fn Func() (int32) {
			var i = 10
			i += 5
			return i
		}
		*/
		const auto source = R"(
fn Func() (int32) {
	// var i int32
	locals (i int32)
	// i = 10
	c_i32 10
	save_l 0
	// i += 5
	load_l 0
	c_i32 5
	add int32
	save_l 0
	// return i
	load_l 0
	save_r 0
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
	c_i32 10
	save_l 0
	// i += 5
	load_l 0
	c_i32 5
	add int32
	save_l 0
	// return i
	load_l 0
	save_r 0
	ret
}
fn Func() (int32) {
	// InnerFunc(5)
	alloc_s 4
	c_i32 5
	call InnerFunc(int32)(int32)
	save_r 0
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

	template<typename T>
	void copy_s_test(const char* type, T in) {
		/*
				fn Mul2(in <type>) (<type>) {
					return lhs+lhs
				}
		*/
		const auto format = R"(
fn Mul2 (in %s) (%s) {
	load_a 0	// Push arg to the stack
	copy_s %s	// Copy value to the stack
	add %s		// Add the two values together
	save_r 0	// Pop the top stack value and put it into the first return value
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
		copy_s_test<vm_int32>("int32", 10);
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
	// Compare less-then
	void clt()
	{
		/*
		fn Compare() (int32) {
			return 12 < 34
		}
		*/
		const auto source = R"(
fn Compare () (int32) {
	c_i32 34	// Push a constant
	c_i32 12	// Push a constant
	clt			// Compare 
	save_r 0	// Pop the top stack value and put it into the first return value
	ret			// Return
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
/*
fn Compare() (int32) {
	return 34 > 12
}
*/
		const auto source = R"(
fn Compare () (int32) {
	c_i32 12	// Push a constant
	c_i32 34	// Push a constant
	cgt			// Compare 
	save_r 0	// Pop the top stack value and put it into the first return value
	ret			// Return
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
	c_i32 12	// Push a constant
	c_i32 34	// Push a constant
	cgt			// Compare 32 > 12
	jmpt marker	// if > jmp marker
	c_i32 20
	save_r 0
	ret			// return 20
#marker 
	c_i32 10
	save_r 0
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
	c_i32 12	// Push a constant
	c_i32 34	// Push a constant
	clt			// Compare 32 < 12
	jmpt marker	// if > jmp marker
	c_i32 20
	save_r 0
	ret			// return 20
#marker 
	c_i32 10
	save_r 0
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
	void const_test(T value) {
		const auto format = R"(
fn Get () (%s) {
	c_%s %d
	save_r 0
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
		verify_stack(t, 0, (T)(value));

		destroy(t);
		destroy(p);
		destroy(c);
	}

	void const_test()
	{
		const_test<vm_int16>(123);
		const_test<vm_int32>(123546);
	}

	void operator()()
	{
		TEST(const_test);
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
	c_%s %d
	conv_%s_%s
	save_r 0
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
	load_a 0
	c_i32 10
	sunref_i32
	ret
}

fn Get () (int32) {
	// var value int32
	locals (value int32)
	// InnerGet(&value)
	ldl_a 0
	call InnerGet(*int32)()
	// return value
	load_l 0
	save_r 0
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

	void operator()()
	{
		TEST(call_fn_using_pointer);
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
}
