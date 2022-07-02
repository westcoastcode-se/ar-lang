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

	vmp_constant vmp_const(vm_int8 value) {
		return vmp_const_i1(value);
	}

	vmp_constant vmp_const(vm_int16 value) {
		return vmp_const_i2(value);
	}

	vmp_constant vmp_const(vm_int32 value) {
		return vmp_const_i4(value);
	}

	vmp_constant vmp_const(vm_int64 value) {
		return vmp_const_i8(value);
	}

	vmp_constant vmp_const(vm_float32 value) {
		return vmp_const_f4(value);
	}

	vmp_constant vmp_const(vm_float64 value) {
		return vmp_const_f8(value);
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
		vmp_arg_set_namesz(arg1, "lhs", 3);
		auto arg2 = vmp_func_new_arg(add, get_type("vm", string(name<T>())));
		vmp_arg_set_namesz(arg2, "rhs", 3);
		auto ret1 = vmp_func_new_return(add, get_type("vm", string(name<T>())));
		vmp_package_add_func(main_package, add);

		// {
		//	lda 1
		//	lda 0
		//	add int32
		//	lda 1
		//	add int32
		//	str 0
		//	ret
		// }
		vmp_func_begin_body(add);
		vmp_func_add_instr(add, vmp_instr_lda(1));
		vmp_func_add_instr(add, vmp_instr_lda(0));
		vmp_func_add_instr(add, vmp_instr_add(props1<T>()));
		vmp_func_add_instr(add, vmp_instr_lda(1));
		vmp_func_add_instr(add, vmp_instr_add(props1<T>()));
		vmp_func_add_instr(add, vmp_instr_str(0));
		vmp_func_add_instr(add, vmp_instr_ret());
		vmp_func_begin_end(add);

		compile();

		auto t = thread();
		vmi_thread_reserve_stack(t, sizeof(T));
		push_value(t, (T)rhs);
		push_value(t, (T)lhs);
		invoke(t, "Add");

		verify_stack_size(t, sizeof(T));
		verify_stack(t, 0, (T)(lhs + rhs + rhs));

		destroy(t);

		end();
	}

	void add()
	{
		TEST_FN(add_test<vm_int32>(10, 20));
		TEST_FN(add_test<vm_int16>(10, 20));
	}

	template<typename T>
	void ldc_T(T value)
	{
		begin();

		// Create the main package
		auto main_package = vmp_package_newsz("main", 4);
		vmp_pipeline_add_package(pipeline, main_package);

		auto primitive_type = get_type("vm", string(name<T>()));

		// Create the Get function that returns a constant value
		auto add = vmp_func_newsz("Get", 3);
		auto ret1 = vmp_func_new_return(add, primitive_type);
		vmp_package_add_func(main_package, add);

		// {
		//	ldc_<T> $value
		//	str 0
		//	ret
		// }
		vmp_func_begin_body(add);
		vmp_func_add_instr(add, vmp_instr_ldc(primitive_type, vmp_const((T)value)));
		vmp_func_add_instr(add, vmp_instr_str(0));
		vmp_func_add_instr(add, vmp_instr_ret());
		vmp_func_begin_end(add);

		compile();

		auto t = thread();
		vmi_thread_reserve_stack(t, sizeof(T));
		invoke(t, "Get");

		verify_stack_size(t, sizeof(T));
		verify_stack(t, 0, (T)(value));

		destroy(t);

		end();
	}

	void ldc()
	{
		TEST_FN(ldc_T<vm_int8>(12));
		TEST_FN(ldc_T<vm_int16>(INT16_MAX - 10));
		TEST_FN(ldc_T<vm_int32>(INT32_MAX - 1234));
		TEST_FN(ldc_T<vm_int64>(INT64_MAX - 123456));
		TEST_FN(ldc_T<vm_float32>(123.456f));
		TEST_FN(ldc_T<vm_float64>(12345.6789));
	}

	void call()
	{
		begin();

		// Create the main package
		auto main_package = vmp_package_newsz("main", 4);
		vmp_pipeline_add_package(pipeline, main_package);

		// Create the Add function and add two integer types
		auto add = vmp_func_newsz("Add", 3);
		auto add_arg1 = vmp_func_new_arg(add, get_type("vm", string(name<vm_int32>())));
		vmp_arg_set_namesz(add_arg1, "lhs", 3);
		auto add_arg2 = vmp_func_new_arg(add, get_type("vm", string(name<vm_int32>())));
		vmp_arg_set_namesz(add_arg2, "rhs", 3);
		auto add_ret1 = vmp_func_new_return(add, get_type("vm", string(name<vm_int32>())));
		vmp_package_add_func(main_package, add);

		// {
		//	lda 1
		//	lda 0
		//	add int32
		//	str 0
		//	ret
		// }
		vmp_func_begin_body(add);
		vmp_func_add_instr(add, vmp_instr_lda(1));
		vmp_func_add_instr(add, vmp_instr_lda(0));
		vmp_func_add_instr(add, vmp_instr_add(props1<vm_int32>()));
		vmp_func_add_instr(add, vmp_instr_str(0));
		vmp_func_add_instr(add, vmp_instr_ret());
		vmp_func_begin_end(add);


		// Create the Add function and add two integer types
		auto get = vmp_func_newsz("Get", 3);
		auto get_ret1 = vmp_func_new_return(get, get_type("vm", string(name<vm_int32>())));
		vmp_package_add_func(main_package, get);

		// {
		//	allocs 4
		//	call Add()(int32)
		//	str 0
		//	ret
		// }
		auto const_val1 = 10;
		auto const_val2 = 20;
		vmp_func_begin_body(get);
		vmp_func_add_instr(get, vmp_instr_allocs_const(4));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type("vm", string(name<vm_int32>())), vmp_const(const_val2)));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type("vm", string(name<vm_int32>())), vmp_const(const_val1)));
		vmp_func_add_instr(get, vmp_instr_call(add));
		vmp_func_add_instr(get, vmp_instr_str(0));
		vmp_func_add_instr(get, vmp_instr_ret());
		vmp_func_begin_end(get);

		compile();

		auto t = thread();
		vmi_thread_reserve_stack(t, sizeof(vm_int32));
		invoke(t, "Get");

		verify_stack_size(t, sizeof(vm_int32));
		verify_stack(t, 0, const_val1 + const_val2);

		destroy(t);

		end();
	}

	void operator()()
	{
		TEST(add);
		TEST(ldc);
		TEST(call);
	}
};


void suite_vmp()
{
	SUITE(suite_vmp_tests);
}
