#include "utils.hpp"

extern "C"
{
#	include <vmp/vmp.h>
}

struct suite_vmp_tests : utils_vm
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

		if (vmc_memory_test_bytes_left() == FALSE) {
			throw_(error() << "not all memory was released");
		}
	}

	void compile()
	{
		if (!vmp_pipeline_resolve(pipeline)) {
			end();
			throw_(error() << "failed to resolve pipeline information");
		}

		builder = vmp_builder_new(pipeline);
		if (!vmp_builder_compile(builder)) {
			error_string_stream e;
			e << "could not compile pipeline: [";
			auto message = builder->messages.first;
			while (message != nullptr) {
				e << "\n" << message->message;
				message = message->next;
			}
			if (builder->panic_error_message.code != 0) {
				e << "\n" << builder->panic_error_message.message;
			}
			e << "\n]";
			end();
			throw_(e);
		}

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

	void invoke(vmi_thread* t, const char* entry_point)
	{
		const vmi_package* package = vmi_process_find_package_by_name(process, "main", 4);
		if (package == NULL)
			throw_(error() << "expected 'main' package but was not found");

		const vmi_package_func* func = vmi_package_find_function_by_name(package, entry_point, strlen(entry_point));
		if (func == NULL)
			throw_(error() << "could not find function '" << entry_point << "'");

		const auto result = vmi_process_exec(process, t, func);
		if (result != 0)
			throw_(error() << "error occurred when executing thread: " << result << ". Message: " << t->exit_reason);
	}

	vmp_type* get_type(const string package, const string type_name) {
		const vm_string package_name = { package.c_str(), package.c_str() + package.length() };
		const vm_string type_name_str = { type_name.c_str(), type_name.c_str() + type_name.length() };

		auto p = vmp_pipeline_find_package(pipeline, &package_name);
		if (p == NULL) {
			throw_(error() << "could not find '" << package << "' package");
		}
		return vmp_package_find_type(p, &type_name_str);
	}

	template<typename T>
	void add_test(T lhs, T rhs)
	{
		begin();

		// Create the main package
		auto main_package = vmp_package_newsz("main", 4);
		vmp_pipeline_add_package(pipeline, main_package);

		// Create the Add function and add two integer types
		auto add = vmp_func_newsz("Add", 3);
		auto arg1 = vmp_func_new_arg(add, get_type("vm", string(name<T>())));
		auto arg2 = vmp_func_new_arg(add, get_type("vm", string(name<T>())));
		auto ret1 = vmp_func_new_return(add, get_type("vm", string(name<T>())));
		vmp_package_add_func(main_package, add);

		// Add body:
		// lda 1
		// lda 0
		// add int32
		// str 0
		// ret
		vmp_func_begin_body(add);
		vmp_func_add_instr(add, vmp_instr_lda(1));
		vmp_func_add_instr(add, vmp_instr_lda(0));
		vmp_func_add_instr(add, vmp_instr_add(props1<T>()));
		vmp_func_add_instr(add, vmp_instr_str(0));
		vmp_func_add_instr(add, vmp_instr_ret());
		vmp_func_begin_end(add);

		compile();

		auto t = thread();
		vmi_thread_reserve_stack(t, sizeof(T));
		push_value(t, (T)lhs);
		push_value(t, (T)rhs);
		invoke(t, "Add");

		verify_stack_size(t, sizeof(T));
		verify_stack(t, 0, (T)(lhs + rhs));

		destroy(t);

		end();
	}

	void add()
	{
		TEST_FN(add_test<vm_int32>(10, 20));
		TEST_FN(add_test<vm_int16>(10, 20));
	}

	void operator()()
	{
		TEST(add);
	}
};


void suite_vmp()
{
	SUITE(suite_vmp_tests);
}
