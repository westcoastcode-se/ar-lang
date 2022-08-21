#include "utils.hpp"

struct utils_zpp : utils_vm
{
	zpp_compiler* compiler;

	void beforeEach()
	{
		// Create a new pipeline to put bytecode into
		compiler = zpp_compiler_new();
	}

	void afterEach()
	{
		if (compiler != NULL) {
			zpp_compiler_destroy(compiler);
			compiler = NULL;
		}
	}

	vm_string to_vm_string(const vm_byte* source_code)
	{
		return { source_code, source_code + strlen(source_code) };
	}

	bool add_source_code(const vm_byte* source_code, const vm_byte* filename)
	{
		const vm_string src = to_vm_string(source_code);
		const vm_string fn = to_vm_string(filename);
		return zpp_compiler_add_source_code(compiler, zpp_source_code_new(&src, &fn)) == TRUE;
	}
	
	const char* format_string(const char* format, ...)
	{
		static char memory[4096];
		va_list argptr;
		va_start(argptr, format);
		vsprintf(memory, format, argptr);
		va_end(argptr);
		return memory;
	}
};

struct utils_zpp_errors : utils_zpp
{
	void beforeEach()
	{
		utils_zpp::beforeEach();
	}

	void afterEach()
	{
		utils_zpp::afterEach();
		vm_memory_test_clear();
	}

	void verify_error_count(int expected) {
		auto message = compiler->messages.first;
		int count = 0;
		while (message) {
			message = message->next;
			count++;
		}
		if (expected != count) {
			throw_(error() << "expected " << expected << " errors but was " << count);
		}
	}

	const vm_message* verify_error(const vm_message* message, int code, int line, int line_offset, const char* str)
	{
		if (code != message->code)
			throw_(error() << "expected code=" << code << " but was " << message->code);

		if (line != message->line)
			throw_(error() << "expected line=" << line << " but was " << message->line);

		if (line_offset != message->line_offset)
			throw_(error() << "expected line_offset=" << line_offset << " but was " << message->line_offset);

		if (strcmp(message->message, str) != 0)
			throw_(error() << "expected message='" << str << "' but was '" << message->message << "'");

		return message->next;
	}
};

struct utils_zpp_success : utils_zpp
{
	vmi_process* process;
	vmi_thread* thread;

	void beforeEach()
	{
		utils_zpp::beforeEach();
		process = NULL;
	}

	void afterEach()
	{
		if (thread != NULL) {
			vmi_thread_destroy(thread);
			thread = NULL;
		}

		if (process != NULL) {
			vmi_process_destroy(process);
			process = NULL;
		}

		utils_zpp::afterEach();

		if (vm_memory_test_bytes_left() == FALSE) {
			throw_(error() << "not all memory was released");
		}
	}

	void add_source_code(const vm_byte* source_code, const vm_byte* filename)
	{
		if (!utils_zpp::add_source_code(source_code, filename)) {
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

		thread = vmi_thread_new(process);
		if (thread == nullptr) {
			throw_(error() << "could not spawn a new VM thread");
		}
	}

	void invoke(const char* entry_point)
	{
		const vmi_package* package = vmi_process_find_package_by_name(process, "main", 4);
		if (package == NULL) {
			throw_(error() << "expected 'main' package but was not found");
		}

		const auto func = vmi_package_find_function_by_name(package, entry_point, (vm_int32)strlen(entry_point));
		if (func == NULL) {
			throw_(error() << "could not find function '" << entry_point << "'");
		}

		const auto result = vmi_process_exec(process, thread, func);
		if (result != 0) {
			throw_(error() << "error occurred when executing thread: " << result << ". Message: " << thread->exit_reason);
		}
	}

	template<typename T>
	T push_value(T value) {
		*(T*)vmi_thread_push_stack(thread, sizeof(T)) = value;
		return value;
	}
};

struct suite_zpp_get_const : utils_zpp_success
{
	template<typename T>
	void const_T(T value)
	{
		const char* source = format_string(R"(
package main

func Get() %s {
	return %s
}
)", name<T>(), to_string((T)value));

		add_source_code(source, "/main.zpp");
		compile();

		invoke("Get");

		verify_stack_size(thread, sizeof(T));
		const auto ret = *(T*)vmi_thread_pop_stack(thread, sizeof(T));
		verify_value(ret, value);
	}

	template<typename T>
	void const_add_T(T value1, T value2)
	{
		const char* source = format_string(R"(
package main

func Get() %s {
	return %s + %s
}
)", name<T>(), to_string((T)value1), to_string((T)value2));

		add_source_code(source, "/main.zpp");
		compile();

		invoke("Get");

		verify_stack_size(thread, sizeof(T));
		const auto ret = *(T*)vmi_thread_pop_stack(thread, sizeof(T));
		verify_value(ret, (T)(value1 + value2));
	}

	void operator()()
	{
		TEST(const_T<vm_int32>(12345));
		TEST(const_T<vm_float32>(123.45f));

		TEST(const_add_T<vm_int32>(12345, 123));
		//TEST(const_add_T<vm_float32>(123.45f));
	}
};

struct suite_zpp_tests : utils_zpp_success
{
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

		invoke("Get");

		verify_stack_size(thread, sizeof(vm_int32));
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(ret, 12345 + 10);
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
		
		invoke("Get");

		verify_stack_size(thread, sizeof(vm_int32));
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(ret, 12345 - 10);
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

		invoke("Get");

		verify_stack_size(thread, sizeof(vm_int32));
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(ret, -12345);
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

		invoke("Get");

		verify_stack_size(thread, sizeof(vm_int32));
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(ret, 12345 - -10);
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

		invoke("Get");

		verify_stack_size(thread, sizeof(vm_int32));
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(ret, -12345 - -10);
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

		invoke("Get");

		verify_stack_size(thread, sizeof(vm_int32));
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(ret, 10 + 20 + 30);
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

		invoke("Get");

		verify_stack_size(thread, sizeof(vm_int32));
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(ret, 10 + 20 + 30 - 40);
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

		invoke("Get");

		verify_stack_size(thread, sizeof(vm_int32));
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(ret, 10 + 20 + 30 - 40);
	}

	void return10()
	{
		static const auto source = R"(
package main

func Get() int32 {
	return 10 * (20 + 30) - 40
}
)";
		add_source_code(source, "/main.zpp");
		compile();

		invoke("Get");

		verify_stack_size(thread, sizeof(vm_int32));
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(ret, 10 * (20 + 30) - 40);
	}

	void return11()
	{
		static const auto source = R"(
package main

func Get() int32 {
	return (10 * (20 + 30)) / 2
}
)";
		add_source_code(source, "/main.zpp");
		compile();

		invoke("Get");

		verify_stack_size(thread, sizeof(vm_int32));
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(ret, (10 * (20 + 30)) / 2);
	}

	void return12()
	{
		static const auto source = R"(
package main

func Get() (int32, int32) {
	return (10 * (20 + 30)) / 2, 123
}
)";
		add_source_code(source, "/main.zpp");
		compile();

		invoke("Get");

		verify_stack_size(thread, sizeof(vm_int32) * 2);
		auto ret = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(ret, 123);
		ret = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(ret, (10 * (20 + 30)) / 2);
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

		invoke("Get");

		verify_stack_size(thread, sizeof(vm_int32));
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(ret, 10 + 20 + 30 - 40);
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

		invoke("Get");

		verify_stack_size(thread, sizeof(vm_int32));
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(ret, 10 + 20 + 30 - 40 + 10);
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

		invoke("Get");

		verify_stack_size(thread, sizeof(vm_int32));
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(ret, 10 + 20 + 30 - 40 + 10);
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

		const auto value = push_value<int>(10000);

		invoke("Arg");

		verify_stack_size(thread, sizeof(vm_int32) * 2);
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(ret, value);
		const auto in = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(in, value);
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

		const auto value = push_value<int>(10000);

		invoke("Arg");

		verify_stack_size(thread, sizeof(vm_int32) * 2);
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(ret, value + 10 + 20 - 30);
		const auto in = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(in, value);
	}

	void arg3()
	{
		static const auto source = R"(
package main

func Arg(value int32) int32 {
	return (value + 10 + 20) - 30
}
)";
		add_source_code(source, "/main.zpp");
		compile();

		const auto value = push_value<int>(10000);

		invoke("Arg");

		verify_stack_size(thread, sizeof(vm_int32) * 2);
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(ret, (value + 10 + 20) - 30);
		const auto in = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(in, value);
	}

	void arg4()
	{
		static const auto source = R"(
package main

func Args(value1 int32, value2 int32) int32 {
	return value1 + value2
}
)";
		add_source_code(source, "/main.zpp");
		compile();

		const auto value1 = push_value<int>(10000);
		const auto value2 = push_value<int>(123);

		invoke("Args");

		verify_stack_size(thread, sizeof(vm_int32) * 3);
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(ret, value1 + value2);
		auto in = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(in, value2);
		in = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(in, value1);
	}

	void arg5()
	{
		static const auto source = R"(
package main

func Args(value1 int32, value2 int32) (int32, int32) {
	return value2, value1
}
)";
		add_source_code(source, "/main.zpp");
		compile();

		const auto value1 = push_value<int>(10000);
		const auto value2 = push_value<int>(123);

		invoke("Args");

		verify_stack_size(thread, sizeof(vm_int32) * 4);
		auto ret = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(ret, value1);
		ret = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(ret, value2);
		ret = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(ret, value2);
		ret = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(ret, value1);
	}

	void compare_lt1()
	{
		static const auto source = R"(
package main

func Arg(value int32) int32 {
	return value < 10
}
)";
		add_source_code(source, "/main.zpp");
		compile();

		const auto value = push_value<int>(5);

		invoke("Arg");

		verify_stack_size(thread, sizeof(vm_int32) + sizeof(vm_bool));
		const auto ret = *(vm_bool*)vmi_thread_pop_stack(thread, sizeof(vm_bool));
		verify_value(ret, value < 10 ? TRUE : 0);
		const auto in = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(in, value);
	}

	void compare_lt2()
	{
		static const auto source = R"(
package main

func Arg(value int32) int32 {
	return value < 100
}
)";
		add_source_code(source, "/main.zpp");
		compile();

		const auto value = push_value<int>(5);

		invoke("Arg");

		verify_stack_size(thread, sizeof(vm_int32) + sizeof(vm_bool));
		const auto ret = *(vm_bool*)vmi_thread_pop_stack(thread, sizeof(vm_bool));
		verify_value(ret, value < 100 ? TRUE : 0);
		const auto in = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(in, value);
	}

	void compare_lt3()
	{
		static const auto source = R"(
package main

func Arg(value int32) int32 {
	return (value + 10) < 100
}
)";
		add_source_code(source, "/main.zpp");
		compile();

		const auto value = push_value<int>(95);

		invoke("Arg");

		verify_stack_size(thread, sizeof(vm_int32) + sizeof(vm_bool));
		const auto ret = *(vm_bool*)vmi_thread_pop_stack(thread, sizeof(vm_bool));
		verify_value(ret, (value + 10) < 100 ? TRUE : 0);
		const auto in = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(in, value);
	}

	void compare_gt1()
	{
		static const auto source = R"(
package main

func Arg(value int32) int32 {
	return value > 10
}
)";
		add_source_code(source, "/main.zpp");
		compile();

		const auto value = push_value<int>(5);

		invoke("Arg");

		verify_stack_size(thread, sizeof(vm_int32) + sizeof(vm_bool));
		const auto ret = *(vm_bool*)vmi_thread_pop_stack(thread, sizeof(vm_bool));
		verify_value(ret, value > 10 ? TRUE : 0);
		const auto in = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(in, value);
	}

	void compare_gt2()
	{
		static const auto source = R"(
package main

func Arg(value int32) int32 {
	return value > 100
}
)";
		add_source_code(source, "/main.zpp");
		compile();

		const auto value = push_value<int>(5);

		invoke("Arg");

		verify_stack_size(thread, sizeof(vm_int32) + sizeof(vm_bool));
		const auto ret = *(vm_bool*)vmi_thread_pop_stack(thread, sizeof(vm_bool));
		verify_value(ret, value > 100 ? TRUE : 0);
		const auto in = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(in, value);
	}

	void compare_gt3()
	{
		static const auto source = R"(
package main

func Arg(value int32) int32 {
	return (value + 10) > 100
}
)";
		add_source_code(source, "/main.zpp");
		compile();

		const auto value = push_value<int>(95);

		invoke("Arg");

		verify_stack_size(thread, sizeof(vm_int32) + sizeof(vm_bool));
		const auto ret = *(vm_bool*)vmi_thread_pop_stack(thread, sizeof(vm_bool));
		verify_value(ret, (value + 10) > 100 ? TRUE : 0);
		const auto in = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(in, value);
	}

	void bit_not1()
	{
		static const auto source = R"(
package main

func BitNot(value int32) int32 {
	return ~value
}
)";
		add_source_code(source, "/main.zpp");
		compile();

		const auto value = push_value<int>(100);

		invoke("BitNot");

		verify_stack_size(thread, sizeof(vm_int32) + sizeof(vm_int32));
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(ret, ~value);
		const auto in = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(in, value);
	}

	void bit_not2()
	{
		static const auto source = R"(
package main

func BitNot(value int32) int32 {
	return ~10 - value
}
)";
		add_source_code(source, "/main.zpp");
		compile();

		const auto value = push_value<int>(100);

		invoke("BitNot");

		verify_stack_size(thread, sizeof(vm_int32) + sizeof(vm_int32));
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(ret, ~10 - value);
		const auto in = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(in, value);
	}

	void bit_not3()
	{
		static const auto source = R"(
package main

func BitNot(value int32) int32 {
	return ~(10 - value)
}
)";
		add_source_code(source, "/main.zpp");
		compile();

		const auto value = push_value<int>(100);

		invoke("BitNot");

		verify_stack_size(thread, sizeof(vm_int32) + sizeof(vm_int32));
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(ret, ~(10 - value));
		const auto in = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(in, value);
	}

	void bit_not4()
	{
		static const auto source = R"(
package main

func BitNot(value int32) int32 {
	return -~value
}
)";
		add_source_code(source, "/main.zpp");
		compile();

		const auto value = push_value<int>(100);

		invoke("BitNot");

		verify_stack_size(thread, sizeof(vm_int32) + sizeof(vm_int32));
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(ret, -~value);
		const auto in = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(in, value);
	}

	void bit_not5()
	{
		static const auto source = R"(
package main

func BitNot(value int32) int32 {
	return ~-value
}
)";
		add_source_code(source, "/main.zpp");
		compile();

		const auto value = push_value<int>(100);

		invoke("BitNot");

		verify_stack_size(thread, sizeof(vm_int32) + sizeof(vm_int32));
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(ret, ~-value);
		const auto in = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(in, value);
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

		const auto value = push_value<int>(123);

		invoke("Complex");

		verify_stack_size(thread, sizeof(vm_int32) * 2);
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(ret, c_complex1(value));
		const auto in = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(in, value);
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

		const auto value = push_value<int>(123);

		invoke("Complex");

		verify_stack_size(thread, sizeof(vm_int32) * 2);
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(ret, c_complex2(value));
		const auto in = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(in, value);
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

		const auto value = push_value<int>(10);

		invoke("FuncRef");

		verify_stack_size(thread, sizeof(vm_int32) * 2);
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(ret, value + 4 - 2);
		const auto in = *(vm_int32*)vmi_thread_pop_stack(thread, sizeof(vm_int32));
		verify_value(in, value);
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

		static constexpr auto COUNT = 10000;

		// 10000 values to sort
		auto arr = new vm_int32[COUNT];
		srand(0);
		for (int i = 0; i < COUNT; ++i) {
			arr[i] = rand() % 1000;
		}
		push_value<vm_int32*>(arr);
		push_value<vm_int32>(0);
		push_value<vm_int32>(COUNT - 1);
		invoke("QuickSort");

		vm_int32 tmp = 0;
		for (int i = 0; i < COUNT; ++i) {
			if (arr[i] < tmp) {
				throw_(error() << "failed to quicksort values");
			}
			tmp = arr[i];
		}
		delete[] arr;
		verify_stack_size(thread, sizeof(vm_int32*) + sizeof(vm_int32) + sizeof(vm_int32));
	}

	void operator()()
	{
		TEST(return2());
		TEST(return3());
		TEST(return4());
		TEST(return5());
		TEST(return6());
		TEST(return7());
		TEST(return8());
		TEST(return9());
		TEST(return10());
		TEST(return11());
		TEST(return12());

		TEST(local1());
		TEST(local2());
		TEST(local3());

		TEST(arg1());
		TEST(arg2());
		TEST(arg3());
		TEST(arg4());
		TEST(arg5());

		TEST(compare_lt1());
		TEST(compare_lt2());
		TEST(compare_lt3());
		TEST(compare_gt1());
		TEST(compare_gt2());
		TEST(compare_gt3());

		TEST(bit_not1());
		TEST(bit_not2());
		TEST(bit_not3());
		TEST(bit_not4());
		TEST(bit_not5());
	}
};

struct suite_zpp_errors : utils_zpp_errors
{
	void no_package_error()
	{
		static const auto source = R"(
func Get() int32 {
	return 0
}
)";
		verify_false(add_source_code(source, "/main.zpp"));
		verify_error_count(1);
		
		const vm_message* message = compiler->messages.first;
		message = verify_error(message,ZPP_MESSAGE_EXPECTED_PACKAGE, 1, 0, "C004: expected package but was 'func' at 1:0");
	}

	void missing_package_identifier()
	{
		static const auto source = R"(
package *

func Get() int32 {
	return 0
}
)";
		verify_false(add_source_code(source, "/main.zpp"));
		verify_error_count(1);

		const vm_message* message = compiler->messages.first;
		message = verify_error(message, ZPP_MESSAGE_EXPECTED_IDENTIFIER, 1, 8, "C005: expected identifier but was '*' at 1:8");
	}

	void missing_function_identifier()
	{
		static const auto source = R"(
package main

func () int32 {
	return 0
}
)";
		verify_false(add_source_code(source, "/main.zpp"));
		verify_error_count(1);

		const vm_message* message = compiler->messages.first;
		message = verify_error(message, ZPP_MESSAGE_EXPECTED_IDENTIFIER, 3, 5, "C005: expected identifier but was '(' at 3:5");
	}

	void operator()()
	{
		TEST(no_package_error());
		TEST(missing_package_identifier());
		TEST(missing_function_identifier());
	}
};

void suite_zpp()
{
	SUITE(suite_zpp_tests);
	SUITE(suite_zpp_get_const);
	SUITE(suite_zpp_errors);
}
