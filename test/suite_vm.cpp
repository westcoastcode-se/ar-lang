#include "test.hpp"

// Base class for all vm tests
struct suite_vm_utils : test_utils
{
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

	void verify_stack_size(vmi_thread* t, size_t expected_size)
	{
		const size_t size = (size_t)(t->stack.top - t->stack.blocks);
		if (expected_size != size) {
			throw_(error() << "expected stack size to be " << expected_size << " but was " << size);
		}
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
	void calculate_return_constant1()
	{
/*
fn Get() (int32) {
	return 12
}
*/
		const auto source = R"(
fn Get () (int32) {
	const int32 123	// Push a constant
	save_r 0		// Pop the top stack value and put it into the first return value
	ret				// Return
}
)";
		auto c = compile(source);
		auto p = process(c);
		auto t = thread(p);

		// begin_
		vmi_thread_push_i32(t, 99); // return value here (can be done by the API)
		invoke(p, t, "Get");

		verify_stack_size(t, sizeof(vm_int32));
		verify_stack(t, 0, 123);

		destroy(t);
		destroy(p);
		destroy(c);
	}

	void calculate_return_constant2()
	{
/*
fn Get() (int32, int32) {
	return 123, 456
}
*/
		const auto source = R"(
fn Get () (int32, int32) {
	const int32 123	// Push 123
	save_r 0		// stack[4] = 123
	const int32 456	// Push 456
	save_r 1		// stack[0] = 456
	ret				// Return
}
)";
		auto c = compile(source);
		auto p = process(c);
		auto t = thread(p);

		// begin_
		vmi_thread_push_i32(t, -1); // return value here (can be done by the API)
		vmi_thread_push_i32(t, -1); // return value here (can be done by the API)
		invoke(p, t, "Get");

		verify_stack_size(t, sizeof(vm_int32) * 2);
		verify_stack(t, 0, 456);
		verify_stack(t, 4, 123);

		destroy(t);
		destroy(p);
		destroy(c);
	}

	void calculate_two_i32()
	{
/*
fn Add(lhs int32, rhs int32) (int32) {
	return lhs + rhs
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
)";
		auto c = compile(source);
		auto p = process(c);
		auto t = thread(p);

		// begin_
		vmi_thread_push_i32(t, 99); // return value here (can be done by the API)
		vmi_thread_push_i32(t, 20);
		vmi_thread_push_i32(t, 10);
		invoke(p, t, "Add");

		verify_stack_size(t, sizeof(vm_int32));
		verify_stack(t, 0, 30);

		destroy(t);
		destroy(p);
		destroy(c);
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
	const int32 20	// Load constant int32 20
	const int32 10	// Load constant int32 10
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
		TEST(calculate_return_constant1);
		TEST(calculate_return_constant2);
		TEST(calculate_two_i32);
		TEST(calculate_multiple_funcs);
		TEST(calculate_two_int32_inner);
	}
};

void suite_vm()
{
	SUITE(suite_vm_tests);
}
