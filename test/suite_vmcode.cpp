#include "utils.hpp"

struct utils_vmcode : utils_vm
{
	vmp_pipeline* pipeline;
	vmp_builder* builder;
	vmi_process* process;

	void begin()
	{
		// Create a new pipeline to put bytecode into
		pipeline = vmp_pipeline_new();
		builder = NULL;
		process = NULL;
	}

	void end()
	{
		if (process != NULL) {
			vmi_process_destroy(process);
			process = NULL;
		}

		if (builder != NULL) {
			vmp_builder_destroy(builder);
			builder = NULL;
		}

		if (pipeline != NULL) {
			vmp_pipeline_destroy(pipeline);
			pipeline = NULL;
		}

		if (vmp_memory_test_bytes_left() == FALSE) {
			throw_(error() << "not all memory was released");
		}
	}

	void compile()
	{
		
		process = vmi_process_new();
		if (process == nullptr)
			throw_(error() << "could not create a new VM process");

		const auto result = vmi_process_load(process, vmp_builder_get_bytecode(builder));
		if (result != 0)
			throw_(error() << "failed to load bytecode because: " << result);
	}

	vmi_thread* thread()
	{
		auto const t = vmi_thread_new(process);
		if (t == nullptr)
			throw_(error() << "could not spawn a new VM thread");
		return t;
	}

	void destroy(vmi_thread* p)
	{
		vmi_thread_destroy(p);
	}

	void invoke(vmi_thread* t, const char* entry_point)
	{
		const vmi_package* package = vmi_process_find_package_by_name(process, "main", 4);
		if (package == NULL)
			throw_(error() << "expected 'main' package but was not found");

		const auto func = vmi_package_find_function_by_name(package, entry_point, (vm_int32)strlen(entry_point));
		if (func == NULL)
			throw_(error() << "could not find function '" << entry_point << "'");

		const auto result = vmi_process_exec(process, t, func);
		if (result != 0)
			throw_(error() << "error occurred when executing thread: " << result << ". Message: " << t->exit_reason);
	}
};

struct suite_vmcode_tests : utils_vmcode
{
	void get_const()
	{
		static const auto source = R"(
package main

func Get() int32 {
	return 12345
}
)";
	}

	void operator()()
	{
	}
};

void suite_vmcode()
{
	SUITE(suite_vmcode_tests);
}
