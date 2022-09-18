#include "utils.hpp"

struct utils_compiler : utils_arlang
{
	arCompiler* compiler;

	void beforeEach()
	{
		// Create a new pipeline to put bytecode into
		compiler = arCompiler_new();
	}

	void afterEach()
	{
		if (compiler != NULL) {
			arCompiler_destroy(compiler);
			compiler = NULL;
		}
	}

	arString to_arString(const arByte* source_code)
	{
		return { source_code, source_code + strlen(source_code) };
	}

	bool add_source_code(const arByte* source_code, const arByte* filename)
	{
		const arString src = to_arString(source_code);
		const arString fn = to_arString(filename);
		return arCompiler_add_source_code(compiler, arC_source_code_new(&src, &fn)) == TRUE;
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

	const char* format_signature(const char* format, ...)
	{
		static char memory[4096];
		va_list argptr;
		va_start(argptr, format);
		vsprintf(memory, format, argptr);
		va_end(argptr);
		return memory;
	}
};

struct utils_compiler_errors : utils_compiler
{
	void beforeEach()
	{
		utils_compiler::beforeEach();
	}

	void afterEach()
	{
		utils_compiler::afterEach();
		arDebugMemory_clear();
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

	const arMessage* verify_error(const arMessage* message, int code, int line, int line_offset, const char* str)
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

struct utils_compiler_success : utils_compiler
{
	arProcess* process;
	arThread* thread;

	void beforeEach()
	{
		utils_compiler::beforeEach();
		process = NULL;
		thread = NULL;
	}

	void afterEach()
	{
		if (thread != NULL) {
			arThread_destroy(thread);
			thread = NULL;
		}

		if (process != NULL) {
			arProcess_destroy(process);
			process = NULL;
		}

		utils_compiler::afterEach();

		if (arDebugMemory_verify() == FALSE) {
			throw_(error() << "not all memory was released");
		}
	}

	void add_source_code(const arByte* source_code, const arByte* filename)
	{
		if (!utils_compiler::add_source_code(source_code, filename)) {
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
		arByte* bytecode = arCompiler_compile(compiler);
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

		process = arProcess_new();
		if (process == nullptr) {
			throw_(error() << "could not create a new VM process");
		}

		const auto result = arProcess_load(process, bytecode);
		if (result != 0) {
			throw_(error() << "failed to load bytecode because: " << result);
		}

		thread = arThread_new(process);
		if (thread == nullptr) {
			throw_(error() << "could not spawn a new VM thread");
		}
	}

	void invoke(const char* entry_point)
	{
		invoke("main", entry_point);
	}

	void invoke(const char* package_name, const char* entry_point)
	{
		const arPackage* package = arProcess_find_package(process, package_name, strlen(package_name));
		if (package == NULL) {
			throw_(error() << "expected '" << package_name << "' package but was not found");
		}

		const auto func = arPackage_find_function(package, entry_point, (arInt32)strlen(entry_point));
		if (func == NULL) {
			throw_(error() << "could not find function '" << entry_point << "'");
		}

		const auto result = arProcess_exec(process, thread, func);
		if (result != 0) {
			throw_(error() << "error occurred when executing thread: " << result << ". Message: " << thread->exit_reason);
		}
	}

	template<typename T>
	T push_value(T value) {
		*(T*)arThread_pushStack(thread, sizeof(T)) = value;
		return value;
	}
};

struct suite_compiler_get_const : utils_compiler_success
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
		const char* signature = format_signature("Get()");

		add_source_code(source, "/main.arl");
		compile();

		invoke(signature);

		verify_stack_size(thread, sizeof(T));
		const auto ret = *(T*)arThread_popStack(thread, sizeof(T));
		verify_value(ret, value);
	}

	template<typename T>
	void const_add_T(T lhs, T rhs)
	{
		const char* source = format_string(R"(
package main

func Get() %s {
	return %s + %s
}
)", name<T>(), to_string((T)lhs), to_string((T)rhs));
		const char* signature = format_signature("Get()");

		add_source_code(source, "/main.arl");
		compile();

		invoke(signature);

		verify_stack_size(thread, sizeof(T));
		const auto ret = *(T*)arThread_popStack(thread, sizeof(T));
		verify_value(ret, (T)(lhs + rhs));
	}

	template<typename T>
	void const_sub_T(T lhs, T rhs)
	{
		const char* source = format_string(R"(
package main

func Get() %s {
	return %s - %s
}
)", name<T>(), to_string((T)lhs), to_string((T)rhs));
		const char* signature = format_signature("Get()");

		add_source_code(source, "/main.arl");
		compile();

		invoke(signature);

		verify_stack_size(thread, sizeof(T));
		const auto ret = *(T*)arThread_popStack(thread, sizeof(T));
		verify_value(ret, (T)(lhs - rhs));
	}

	void operator()()
	{
		TEST(const_T<arInt32>(12345));
		TEST(const_T<arFloat64>(123.45));

		TEST(const_add_T<arInt32>(12345, 123));
		TEST(const_add_T<arFloat64>(123.45, 1.2));

		TEST(const_sub_T<arInt32>(12345, 123));
		TEST(const_sub_T<arFloat64>(123.45, 1.2));
	}
};

struct suite_compiler_functions : utils_compiler_success
{
	void calling_before()
	{
		static const auto source = R"(
package main

func innerGet() int32 {
	return -12345
}

func Get() int32 {
	return innerGet()
}
)";
		add_source_code(source, "/main.arl");
		compile();

		invoke("Get()");

		verify_stack_size(thread, sizeof(arInt32));
		const auto ret = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
		verify_value(ret, -12345);
	}

	void calling_after()
	{
		static const auto source = R"(
package main

func Get() int32 {
	return innerGet()
}

func innerGet() int32 {
	return -12345
}
)";
		add_source_code(source, "/main.arl");
		compile();

		invoke("Get()");

		verify_stack_size(thread, sizeof(arInt32));
		const auto ret = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
		verify_value(ret, -12345);
	}

	void operator()()
	{
		TEST(calling_before());
		TEST(calling_after());
	}
};

struct suite_compiler_tests : utils_compiler_success
{
	void return4()
	{
		static const auto source = R"(
package main

func Get() int32 {
	return -12345
}
)";
		add_source_code(source, "/main.arl");
		compile();

		invoke("Get()");

		verify_stack_size(thread, sizeof(arInt32));
		const auto ret = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
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
		add_source_code(source, "/main.arl");
		compile();

		invoke("Get()");

		verify_stack_size(thread, sizeof(arInt32));
		const auto ret = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
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
		add_source_code(source, "/main.arl");
		compile();

		invoke("Get()");

		verify_stack_size(thread, sizeof(arInt32));
		const auto ret = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
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
		add_source_code(source, "/main.arl");
		compile();

		invoke("Get()");

		verify_stack_size(thread, sizeof(arInt32));
		const auto ret = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
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
		add_source_code(source, "/main.arl");
		compile();

		invoke("Get()");

		verify_stack_size(thread, sizeof(arInt32));
		const auto ret = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
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
		add_source_code(source, "/main.arl");
		compile();

		invoke("Get()");

		verify_stack_size(thread, sizeof(arInt32));
		const auto ret = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
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
		add_source_code(source, "/main.arl");
		compile();

		invoke("Get()");

		verify_stack_size(thread, sizeof(arInt32));
		const auto ret = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
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
		add_source_code(source, "/main.arl");
		compile();

		invoke("Get()");

		verify_stack_size(thread, sizeof(arInt32));
		const auto ret = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
		verify_value(ret, (10 * (20 + 30)) / 2);
	}

	void return12()
	{
		static const auto source = R"(
package WestCoastCode.InnerPackage

func Get() (int32, int32) {
	return (10 * (20 + 30)) / 2, 123
}
)";
		add_source_code(source, "/main.arl");
		compile();

		invoke("WestCoastCode.InnerPackage", "Get()");

		verify_stack_size(thread, sizeof(arInt32) * 2);
		auto ret = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
		verify_value(ret, 123);
		ret = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
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
		add_source_code(source, "/main.arl");
		compile();

		invoke("Get()");

		verify_stack_size(thread, sizeof(arInt32));
		const auto ret = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
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
		add_source_code(source, "/main.arl");
		compile();

		invoke("Get()");

		verify_stack_size(thread, sizeof(arInt32));
		const auto ret = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
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
		add_source_code(source, "/main.arl");
		compile();

		invoke("Get()");

		verify_stack_size(thread, sizeof(arInt32));
		const auto ret = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
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
		add_source_code(source, "/main.arl");
		compile();

		const auto value = push_value<int>(10000);

		invoke("Arg(int32)");

		verify_stack_size(thread, sizeof(arInt32) * 2);
		const auto ret = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
		verify_value(ret, value);
		const auto in = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
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
		add_source_code(source, "/main.arl");
		compile();

		const auto value = push_value<int>(10000);

		invoke("Arg(int32)");

		verify_stack_size(thread, sizeof(arInt32) * 2);
		const auto ret = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
		verify_value(ret, value + 10 + 20 - 30);
		const auto in = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
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
		add_source_code(source, "/main.arl");
		compile();

		const auto value = push_value<int>(10000);

		invoke("Arg(int32)");

		verify_stack_size(thread, sizeof(arInt32) * 2);
		const auto ret = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
		verify_value(ret, (value + 10 + 20) - 30);
		const auto in = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
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
		add_source_code(source, "/main.arl");
		compile();

		const auto value1 = push_value<int>(10000);
		const auto value2 = push_value<int>(123);

		invoke("Args(int32,int32)");

		verify_stack_size(thread, sizeof(arInt32) * 3);
		const auto ret = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
		verify_value(ret, value1 + value2);
		auto in = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
		verify_value(in, value2);
		in = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
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
		add_source_code(source, "/main.arl");
		compile();

		const auto value1 = push_value<int>(10000);
		const auto value2 = push_value<int>(123);

		invoke("Args(int32,int32)");

		verify_stack_size(thread, sizeof(arInt32) * 4);
		auto ret = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
		verify_value(ret, value1);
		ret = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
		verify_value(ret, value2);
		ret = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
		verify_value(ret, value2);
		ret = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
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
		add_source_code(source, "/main.arl");
		compile();

		const auto value = push_value<int>(5);

		invoke("Arg(int32)");

		verify_stack_size(thread, sizeof(arInt32) + sizeof(arBool));
		const auto ret = *(arBool*)arThread_popStack(thread, sizeof(arBool));
		verify_value(ret, value < 10 ? TRUE : 0);
		const auto in = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
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
		add_source_code(source, "/main.arl");
		compile();

		const auto value = push_value<int>(5);

		invoke("Arg(int32)");

		verify_stack_size(thread, sizeof(arInt32) + sizeof(arBool));
		const auto ret = *(arBool*)arThread_popStack(thread, sizeof(arBool));
		verify_value(ret, value < 100 ? TRUE : 0);
		const auto in = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
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
		add_source_code(source, "/main.arl");
		compile();

		const auto value = push_value<int>(95);

		invoke("Arg(int32)");

		verify_stack_size(thread, sizeof(arInt32) + sizeof(arBool));
		const auto ret = *(arBool*)arThread_popStack(thread, sizeof(arBool));
		verify_value(ret, (value + 10) < 100 ? TRUE : 0);
		const auto in = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
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
		add_source_code(source, "/main.arl");
		compile();

		const auto value = push_value<int>(5);

		invoke("Arg(int32)");

		verify_stack_size(thread, sizeof(arInt32) + sizeof(arBool));
		const auto ret = *(arBool*)arThread_popStack(thread, sizeof(arBool));
		verify_value(ret, value > 10 ? TRUE : 0);
		const auto in = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
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
		add_source_code(source, "/main.arl");
		compile();

		const auto value = push_value<int>(5);

		invoke("Arg(int32)");

		verify_stack_size(thread, sizeof(arInt32) + sizeof(arBool));
		const auto ret = *(arBool*)arThread_popStack(thread, sizeof(arBool));
		verify_value(ret, value > 100 ? TRUE : 0);
		const auto in = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
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
		add_source_code(source, "/main.arl");
		compile();

		const auto value = push_value<int>(95);

		invoke("Arg(int32)");

		verify_stack_size(thread, sizeof(arInt32) + sizeof(arBool));
		const auto ret = *(arBool*)arThread_popStack(thread, sizeof(arBool));
		verify_value(ret, (value + 10) > 100 ? TRUE : 0);
		const auto in = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
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
		add_source_code(source, "/main.arl");
		compile();

		const auto value = push_value<int>(100);

		invoke("BitNot(int32)");

		verify_stack_size(thread, sizeof(arInt32) + sizeof(arInt32));
		const auto ret = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
		verify_value(ret, ~value);
		const auto in = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
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
		add_source_code(source, "/main.arl");
		compile();

		const auto value = push_value<int>(100);

		invoke("BitNot(int32)");

		verify_stack_size(thread, sizeof(arInt32) + sizeof(arInt32));
		const auto ret = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
		verify_value(ret, ~10 - value);
		const auto in = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
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
		add_source_code(source, "/main.arl");
		compile();

		const auto value = push_value<int>(100);

		invoke("BitNot(int32)");

		verify_stack_size(thread, sizeof(arInt32) + sizeof(arInt32));
		const auto ret = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
		verify_value(ret, ~(10 - value));
		const auto in = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
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
		add_source_code(source, "/main.arl");
		compile();

		const auto value = push_value<int>(100);

		invoke("BitNot(int32)");

		verify_stack_size(thread, sizeof(arInt32) + sizeof(arInt32));
		const auto ret = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
		verify_value(ret, -~value);
		const auto in = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
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
		add_source_code(source, "/main.arl");
		compile();

		const auto value = push_value<int>(100);

		invoke("BitNot(int32)");

		verify_stack_size(thread, sizeof(arInt32) + sizeof(arInt32));
		const auto ret = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
		verify_value(ret, ~-value);
		const auto in = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
		verify_value(in, value);
	}

	static arInt32 c_complex1(arInt32 value) {
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
		add_source_code(source, "/main.arl");
		compile();

		const auto value = push_value<int>(123);

		invoke("Complex(int32)");

		verify_stack_size(thread, sizeof(arInt32) * 2);
		const auto ret = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
		verify_value(ret, c_complex1(value));
		const auto in = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
		verify_value(in, value);
	}

	static arInt32 c_complex2(arInt32 value) {
		return (arInt32)((((++value / 2.0f) + (value++)) - --value) * 10);
	}

	void complex2()
	{
		static const auto source = R"(
package main

func Complex(value int32) int32 {
	return (int32)((((++value / 2.0f) + (value++)) - --value) * 10)
}
)";
		add_source_code(source, "/main.arl");
		compile();

		const auto value = push_value<int>(123);

		invoke("Complex(int32)");

		verify_stack_size(thread, sizeof(arInt32) * 2);
		const auto ret = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
		verify_value(ret, c_complex2(value));
		const auto in = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
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
		add_source_code(source, "/main.arl");
		compile();

		const auto value = push_value<int>(10);

		invoke("FuncRef(int32)");

		verify_stack_size(thread, sizeof(arInt32) * 2);
		const auto ret = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
		verify_value(ret, value + 4 - 2);
		const auto in = *(arInt32*)arThread_popStack(thread, sizeof(arInt32));
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
		add_source_code(source, "/main.arl");
		compile();

		static constexpr auto COUNT = 10000;

		// 10000 values to sort
		auto arr = new arInt32[COUNT];
		srand(0);
		for (int i = 0; i < COUNT; ++i) {
			arr[i] = rand() % 1000;
		}
		push_value<arInt32*>(arr);
		push_value<arInt32>(0);
		push_value<arInt32>(COUNT - 1);
		invoke("QuickSort(*int32,int32,int32)()");

		arInt32 tmp = 0;
		for (int i = 0; i < COUNT; ++i) {
			if (arr[i] < tmp) {
				throw_(error() << "failed to quicksort values");
			}
			tmp = arr[i];
		}
		delete[] arr;
		verify_stack_size(thread, sizeof(arInt32*) + sizeof(arInt32) + sizeof(arInt32));
	}

	void operator()()
	{
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

struct suite_compiler_errors : utils_compiler_errors
{
	void no_package_error()
	{
		static const auto source = R"(
func Get() int32 {
	return 0
}
)";
		verify_false(add_source_code(source, "/main.arl"));
		verify_error_count(1);
		
		const arMessage* message = compiler->messages.first;
		message = verify_error(message, arC_MESSAGE_EXPECTED_PACKAGE, 1, 0, "C004: expected package but was 'func' at 1:0");
	}

	void missing_package_identifier()
	{
		static const auto source = R"(
package *

func Get() int32 {
	return 0
}
)";
		verify_false(add_source_code(source, "/main.arl"));
		verify_error_count(1);

		const arMessage* message = compiler->messages.first;
		message = verify_error(message, arC_MESSAGE_EXPECTED_IDENTIFIER, 1, 8, "C005: expected identifier but was '*' at 1:8");
	}

	void missing_function_identifier()
	{
		static const auto source = R"(
package main

func () int32 {
	return 0
}
)";
		verify_false(add_source_code(source, "/main.arl"));
		verify_error_count(1);

		const arMessage* message = compiler->messages.first;
		message = verify_error(message, arC_MESSAGE_EXPECTED_IDENTIFIER, 3, 5, "C005: expected identifier but was '(' at 3:5");
	}

	void operator()()
	{
		TEST(no_package_error());
		TEST(missing_package_identifier());
		TEST(missing_function_identifier());
	}
};

void suite_compiler()
{
	SUITE(suite_compiler_get_const);
	SUITE(suite_compiler_functions);
	SUITE(suite_compiler_tests);
	SUITE(suite_compiler_errors);
}
