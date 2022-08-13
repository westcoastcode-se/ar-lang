#include "utils.hpp"

struct utils_zpp : utils_vm
{
	zpp_compiler* compiler;
	vmi_process* process;

	void begin()
	{
		// Create a new pipeline to put bytecode into
		compiler = zpp_compiler_new();
		process = NULL;
	}

	void end()
	{
		if (compiler != NULL) {
			zpp_compiler_destroy(compiler);
			compiler = NULL;
		}

		if (vm_memory_test_bytes_left() == FALSE) {
			throw_(error() << "not all memory was released");
		}
	}

	vm_string to_vm_string(const vm_byte* source_code)
	{
		return { source_code, source_code + strlen(source_code) };
	}

	void add_source_code(const vm_byte* source_code, const vm_byte* filename)
	{
		const vm_string src = to_vm_string(source_code);
		const vm_string fn = to_vm_string(filename);
		if (zpp_compiler_add_source_code(compiler, zpp_source_code_new(&src, &fn)) == FALSE) {
			error_string_stream e;
			e << "could not add source code: [";
			auto message = compiler->messages.first;
			while (message != nullptr) {
				e << "\n" << message->message;
				message = message->next;
			}
			if (compiler->panic.code != 0) {
				e << "\n" << compiler->panic.message;
			}
			e << "\n]";
			throw_(e);
		}
	}

	void compile()
	{
		vm_byte* bytecode = zpp_compiler_compile(compiler);
		if (bytecode == NULL) {
			error_string_stream e;
			e << "could not parse source code: [";
			auto message = compiler->messages.first;
			while (message != nullptr) {
				e << "\n" << message->message;
				message = message->next;
			}
			if (compiler->panic.code != 0) {
				e << "\n" << compiler->panic.message;
			}
			e << "\n]";
			throw_(e);
		}

		process = vmi_process_new();
		if (process == nullptr) {
			throw_(error() << "could not create a new VM process");
		}

		const auto result = vmi_process_load(process, bytecode);
		if (result != 0) {
			throw_(error() << "failed to load bytecode because: " << result);
		}
	}

	vmi_thread* thread()
	{
		auto const t = vmi_thread_new(process);
		if (t == nullptr) {
			throw_(error() << "could not spawn a new VM thread");
		}
		return t;
	}

	void destroy(vmi_thread* p)
	{
		vmi_thread_destroy(p);
	}

	void invoke(vmi_thread* t, const char* entry_point)
	{
		const vmi_package* package = vmi_process_find_package_by_name(process, "main", 4);
		if (package == NULL) {
			throw_(error() << "expected 'main' package but was not found");
		}

		const auto func = vmi_package_find_function_by_name(package, entry_point, (vm_int32)strlen(entry_point));
		if (func == NULL) {
			throw_(error() << "could not find function '" << entry_point << "'");
		}

		const auto result = vmi_process_exec(process, t, func);
		if (result != 0) {
			throw_(error() << "error occurred when executing thread: " << result << ". Message: " << t->exit_reason);
		}
	}
};

struct suite_zpp_tests : utils_zpp
{
	void return1()
	{
		static const auto source = R"(
package main

func Get() int32 {
	return 12345
}
)";
		add_source_code(source, "/main.zpp");
		compile();

		auto t = thread();

		invoke(t, "Get");

		verify_stack_size(t, sizeof(vm_int32));
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(t, sizeof(vm_int32));
		verify_value(ret, 12345);

		destroy(t);
	}

	void return2()
	{
		static const auto source = R"(
package main

func Get() int32 {
	return 12345 + 10
}
)";
		add_source_code(source, "/main.zpp");
		compile();

		auto t = thread();

		invoke(t, "Get");

		verify_stack_size(t, sizeof(vm_int32));
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(t, sizeof(vm_int32));
		verify_value(ret, 12345 + 10);

		destroy(t);
	}

	void return3()
	{
		static const auto source = R"(
package main

func Get() int32 {
	return 12345 - 10
}
)";
		add_source_code(source, "/main.zpp");
		compile();

		auto t = thread();

		invoke(t, "Get");

		verify_stack_size(t, sizeof(vm_int32));
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(t, sizeof(vm_int32));
		verify_value(ret, 12345 - 10);

		destroy(t);
	}

	void return4()
	{
		static const auto source = R"(
package main

func Get() int32 {
	return -12345
}
)";
		add_source_code(source, "/main.zpp");
		compile();

		auto t = thread();

		invoke(t, "Get");

		verify_stack_size(t, sizeof(vm_int32));
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(t, sizeof(vm_int32));
		verify_value(ret, -12345);

		destroy(t);
	}

	void return5()
	{
		static const auto source = R"(
package main

func Get() int32 {
	return 12345 - -10
}
)";
		add_source_code(source, "/main.zpp");
		compile();

		auto t = thread();

		invoke(t, "Get");

		verify_stack_size(t, sizeof(vm_int32));
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(t, sizeof(vm_int32));
		verify_value(ret, 12345 - -10);

		destroy(t);
	}

	void return6()
	{
		static const auto source = R"(
package main

func Get() int32 {
	return -12345 - -10
}
)";
		add_source_code(source, "/main.zpp");
		compile();

		auto t = thread();

		invoke(t, "Get");

		verify_stack_size(t, sizeof(vm_int32));
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(t, sizeof(vm_int32));
		verify_value(ret, -12345 - -10);

		destroy(t);
	}

	void return7()
	{
		static const auto source = R"(
package main

func Get() int32 {
	return 10 + 20 + 30
}
)";
		add_source_code(source, "/main.zpp");
		compile();

		auto t = thread();

		invoke(t, "Get");

		verify_stack_size(t, sizeof(vm_int32));
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(t, sizeof(vm_int32));
		verify_value(ret, 10 + 20 + 30);

		destroy(t);
	}

	void return8()
	{
		static const auto source = R"(
package main

func Get() int32 {
	return 10 + 20 + 30 - 40
}
)";
		add_source_code(source, "/main.zpp");
		compile();

		auto t = thread();

		invoke(t, "Get");

		verify_stack_size(t, sizeof(vm_int32));
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(t, sizeof(vm_int32));
		verify_value(ret, 10 + 20 + 30 - 40);

		destroy(t);
	}

	void return9()
	{
		static const auto source = R"(
package main

func Get() int32 {
	return 10 + (20 + 30) - 40
}
)";
		add_source_code(source, "/main.zpp");
		compile();

		auto t = thread();

		invoke(t, "Get");

		verify_stack_size(t, sizeof(vm_int32));
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(t, sizeof(vm_int32));
		verify_value(ret, 10 + 20 + 30 - 40);

		destroy(t);
	}

	void local1()
	{
		static const auto source = R"(
package main

func Get() int32 {
	ret := 10 + 20 + 30 - 40
	return ret
}
)";
		add_source_code(source, "/main.zpp");
		compile();

		auto t = thread();

		invoke(t, "Get");

		verify_stack_size(t, sizeof(vm_int32));
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(t, sizeof(vm_int32));
		verify_value(ret, 10 + 20 + 30 - 40);

		destroy(t);
	}

	void local2()
	{
		static const auto source = R"(
package main

func Get() int32 {
	ret := 10 + 20 + 30 - 40
	ret = ret + 10
	return ret
}
)";
		add_source_code(source, "/main.zpp");
		compile();

		auto t = thread();

		invoke(t, "Get");

		verify_stack_size(t, sizeof(vm_int32));
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(t, sizeof(vm_int32));
		verify_value(ret, 10 + 20 + 30 - 40 + 10);

		destroy(t);
	}

	void local3()
	{
		static const auto source = R"(
package main

func Get() int32 {
	ret := 10 + 20 + 30 - 40
	ret2 := ret + 10
	return ret2
}
)";
		add_source_code(source, "/main.zpp");
		compile();

		auto t = thread();

		invoke(t, "Get");

		verify_stack_size(t, sizeof(vm_int32));
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(t, sizeof(vm_int32));
		verify_value(ret, 10 + 20 + 30 - 40 + 10);

		destroy(t);
	}

	void arg1()
	{
		static const auto source = R"(
package main

func Arg(value int32) int32 {
	return value
}
)";
		add_source_code(source, "/main.zpp");
		compile();

		auto t = thread();

		static constexpr auto value = 10000;
		*(vm_int32*)vmi_thread_push_stack(t, sizeof(vm_int32)) = value;

		invoke(t, "Arg");

		verify_stack_size(t, sizeof(vm_int32) * 2);
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(t, sizeof(vm_int32));
		verify_value(ret, value);
		const auto in = *(vm_int32*)vmi_thread_pop_stack(t, sizeof(vm_int32));
		verify_value(in, value);

		destroy(t);
	}

	void arg2()
	{
		static const auto source = R"(
package main

func Arg(value int32) int32 {
	return value + 10 + 20 - 30
}
)";
		add_source_code(source, "/main.zpp");
		compile();

		auto t = thread();

		static constexpr auto value = 10000;
		*(vm_int32*)vmi_thread_push_stack(t, sizeof(vm_int32)) = value;

		invoke(t, "Arg");

		verify_stack_size(t, sizeof(vm_int32) * 2);
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(t, sizeof(vm_int32));
		verify_value(ret, value + 10 + 20 - 30);
		const auto in = *(vm_int32*)vmi_thread_pop_stack(t, sizeof(vm_int32));
		verify_value(in, value);

		destroy(t);
	}

	static vm_int32 c_complex1(vm_int32 value) {
		return (((++value * 10) + (value++)) - --value) * 2;
	}

	void complex1()
	{
		static const auto source = R"(
package main

func Complex(value int32) int32 {
	return (((++value * 10) + (value++)) - --value) * 2
}
)";
		add_source_code(source, "/main.zpp");
		compile();

		auto t = thread();

		static constexpr auto value = 123;
		*(vm_int32*)vmi_thread_push_stack(t, sizeof(vm_int32)) = value;

		invoke(t, "Complex");

		verify_stack_size(t, sizeof(vm_int32) * 2);
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(t, sizeof(vm_int32));
		verify_value(ret, c_complex1(value));
		const auto in = *(vm_int32*)vmi_thread_pop_stack(t, sizeof(vm_int32));
		verify_value(in, value);

		destroy(t);
	}

	static vm_int32 c_complex2(vm_int32 value) {
		return (vm_int32)((((++value / 2.0f) + (value++)) - --value) * 10);
	}

	void complex2()
	{
		static const auto source = R"(
package main

func Complex(value int32) int32 {
	return (int32)((((++value / 2.0f) + (value++)) - --value) * 10)
}
)";
		add_source_code(source, "/main.zpp");
		compile();

		auto t = thread();

		static constexpr auto value = 123;
		*(vm_int32*)vmi_thread_push_stack(t, sizeof(vm_int32)) = value;

		invoke(t, "Complex");

		verify_stack_size(t, sizeof(vm_int32) * 2);
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(t, sizeof(vm_int32));
		verify_value(ret, c_complex2(value));
		const auto in = *(vm_int32*)vmi_thread_pop_stack(t, sizeof(vm_int32));
		verify_value(in, value);

		destroy(t);
	}

	void func_refs()
	{
		static const auto source = R"(
package main

func add(i int32) int32 {
	return i + 4 
}

func sub(i int32) int32 {
	return i - 2
}

func FuncRef(value int32) int32 {
	f := add
	value = f(value)
	f = sub
	return f(value)
}
)";
		add_source_code(source, "/main.zpp");
		compile();

		auto t = thread();

		static constexpr auto value = 10;
		*(vm_int32*)vmi_thread_push_stack(t, sizeof(vm_int32)) = value;

		invoke(t, "FuncRef");

		verify_stack_size(t, sizeof(vm_int32) * 2);
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(t, sizeof(vm_int32));
		verify_value(ret, value + 4 - 2);
		const auto in = *(vm_int32*)vmi_thread_pop_stack(t, sizeof(vm_int32));
		verify_value(in, value);

		destroy(t);
	}

	void quicksort()
	{
		static const auto source = R"(
package main

func swap(a *int32, b *int32) {
	t := *a
	*a = *b
	*b = t
}

func partition(arr *int32, low int32, high int32) {
	pivot := arr{high]
	i := low - 1
	for j := 0; j <= high; ++j) {
		if arr[i] < pivot) {
			++i
			swap(&arr[i], &arr[j])
		}
	}
	swap(&arr[i + 1], &arr[high])
	return i + 1
}

// QuickSort the supplied array. low is the starting index and high is the ending index
func QuickSort(arr *int32, low int32, high int32) {
	if low < high {
		pi := partition(arr, low, high)
		QuickSort(arr, low, pi - 1)
		QuickSort(arr, pi + 1, high)
	}
}
)";
		add_source_code(source, "/main.zpp");
		compile();

		auto t = thread();

		static constexpr auto COUNT = 10000;

		// 10000 values to sort
		auto arr = new vm_int32[COUNT];
		srand(0);
		for (int i = 0; i < COUNT; ++i) {
			arr[i] = rand() % 1000;
		}
		*(vm_int32**)vmi_thread_push_stack(t, sizeof(vm_int32*)) = arr;
		*(vm_int32*)vmi_thread_push_stack(t, sizeof(vm_int32)) = 0;
		*(vm_int32*)vmi_thread_push_stack(t, sizeof(vm_int32)) = COUNT - 1;
		invoke(t, "QuickSort");
		destroy(t);
		vm_int32 tmp = 0;
		for (int i = 0; i < COUNT; ++i) {
			if (arr[i] < tmp) {
				throw_(error() << "failed to quicksort values");
			}
			tmp = arr[i];
		}
		delete[] arr;
		verify_stack_size(t, sizeof(vm_int32*) + sizeof(vm_int32) + sizeof(vm_int32));
	}
	void operator()()
	{
		TEST_BEGIN_END(return1);
		TEST_BEGIN_END(return2);
		TEST_BEGIN_END(return3);
		TEST_BEGIN_END(return4);
		TEST_BEGIN_END(return5);
		TEST_BEGIN_END(return6);
		TEST_BEGIN_END(return7);
		TEST_BEGIN_END(return8);
		TEST_BEGIN_END(return9);

		TEST_BEGIN_END(local1);
		TEST_BEGIN_END(local2);
		TEST_BEGIN_END(local3);

		TEST_BEGIN_END(arg1);
		TEST_BEGIN_END(arg2);
	}
};

void suite_zpp()
{
	SUITE(suite_zpp_tests);
}
