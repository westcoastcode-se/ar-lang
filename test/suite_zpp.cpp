#include "utils.hpp"

extern "C" {
#	include "zpp/compiler.h"
}

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

	bool add_source_code(const vm_byte* source_code, const vm_byte* filename)
	{
		const vm_string src = to_vm_string(source_code);
		const vm_string fn = to_vm_string(filename);
		return zpp_compiler_add_source_code(compiler, zpp_source_code_new(&src, &fn)) == TRUE;
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

	void operator()()
	{
		TEST_BEGIN_END(return1);
		TEST_BEGIN_END(return2);
		TEST_BEGIN_END(return3);
		TEST_BEGIN_END(return4);
		TEST_BEGIN_END(return5);
		TEST_BEGIN_END(return6);
	}
};

void suite_zpp()
{
	SUITE(suite_zpp_tests);
}
