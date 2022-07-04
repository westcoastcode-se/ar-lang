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

	template<typename T>
	void push_value(vmi_thread* t, T value)
	{
		*(T*)vmi_thread_reserve_stack(t, sizeof(T)) = (T)value;
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

		const auto func = vmi_package_find_function_by_name(package, entry_point, strlen(entry_point));
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
		//	add T
		//	lda 1
		//	add T
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
		TEST_FN(add_test<vm_int8>(1, 5));
		TEST_FN(add_test<vm_uint8>(1, 5));
		TEST_FN(add_test<vm_int16>(10, INT8_MAX));
		TEST_FN(add_test<vm_uint16>(10, INT8_MAX));
		TEST_FN(add_test<vm_int32>(INT16_MAX, 20));
		TEST_FN(add_test<vm_uint32>(INT16_MAX, INT16_MAX));
		TEST_FN(add_test<vm_int64>(102, INT32_MAX));
		TEST_FN(add_test<vm_uint64>(INT32_MAX, INT32_MAX));
		TEST_FN(add_test<vm_float32>(1.0f, 20.0f));
		TEST_FN(add_test<vm_float64>(-10.0, 203.0));
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

		// Create the Get function and add two integer types
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

	void locals()
	{
		begin();

		// Create the main package
		auto main_package = vmp_package_newsz("main", 4);
		vmp_pipeline_add_package(pipeline, main_package);

		// Create the Get function and add two integer types
		auto get = vmp_func_newsz("Get", 3);
		auto get_ret1 = vmp_func_new_return(get, get_type("vm", string(name<vm_int32>())));
		auto get_local1 = vmp_func_new_local(get, get_type("vm", string(name<vm_int32>())));
		vmp_local_set_namesz(get_local1, "i", 1);
		vmp_package_add_func(main_package, get);

		// {
		//	locals (i int32)
		//	ldc_i4 100
		//	stl 0
		//	ldl 0
		//	ldc_i4 1
		//	add int32
		//	stl 0
		//	ldl 0
		//	str 0
		//	ret
		// }
		vmp_func_begin_body(get);
		vmp_func_add_instr(get, vmp_instr_ldc(get_type("vm", string(name<vm_int32>())), vmp_const((vm_int32)100)));
		vmp_func_add_instr(get, vmp_instr_stl(0));
		vmp_func_add_instr(get, vmp_instr_ldl(0));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type("vm", string(name<vm_int32>())), vmp_const((vm_int32)1)));
		vmp_func_add_instr(get, vmp_instr_add(props1<vm_int32>()));
		vmp_func_add_instr(get, vmp_instr_stl(0));
		vmp_func_add_instr(get, vmp_instr_ldl(0));
		vmp_func_add_instr(get, vmp_instr_str(0));
		vmp_func_add_instr(get, vmp_instr_ret());
		vmp_func_begin_end(get);

		compile();

		auto t = thread();
		vmi_thread_reserve_stack(t, sizeof(vm_int32));
		invoke(t, "Get");

		verify_stack_size(t, sizeof(vm_int32));
		verify_stack(t, 0, 100 + 1);

		destroy(t);

		end();
	}

	void cgt(const vm_int32 const_val)
	{
		begin();

		// Create the main package
		auto main_package = vmp_package_newsz("main", 4);
		vmp_pipeline_add_package(pipeline, main_package);

		// Create the Get function and add two integer types
		auto get = vmp_func_newsz("Get", 3);
		vmp_func_new_arg(get, get_type("vm", string(name<vm_int32>())));
		vmp_func_new_return(get, get_type("vm", string(name<vm_int32>())));
		vmp_package_add_func(main_package, get);
		
		// {
		//	lda 0
		//	ldc_i4 10
		//	cgt
		//	str 0
		//	ret
		// }
		vmp_func_begin_body(get);
		vmp_func_add_instr(get, vmp_instr_lda(0));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type("vm", string(name<vm_int32>())), vmp_const(10)));
		vmp_func_add_instr(get, vmp_instr_cgt(get_type("vm", string(name<vm_int32>()))));
		vmp_func_add_instr(get, vmp_instr_str(0));
		vmp_func_add_instr(get, vmp_instr_ret());
		vmp_func_begin_end(get);

		compile();

		auto t = thread();
		vmi_thread_reserve_stack(t, sizeof(vm_int32));
		vmi_thread_push_i32(t, const_val);
		invoke(t, "Get");

		verify_stack_size(t, sizeof(vm_int32));
		verify_stack(t, 0, 10 > const_val ? TRUE : FALSE);

		destroy(t);

		end();
	}

	void cgt()
	{
		TEST_FN(cgt(100));
		TEST_FN(cgt(1));
		TEST_FN(cgt(-1));
	}

	void clt(const vm_int32 const_val)
	{
		begin();

		// Create the main package
		auto main_package = vmp_package_newsz("main", 4);
		vmp_pipeline_add_package(pipeline, main_package);

		// Create the Get function and add two integer types
		auto get = vmp_func_newsz("Get", 3);
		vmp_func_new_arg(get, get_type("vm", string(name<vm_int32>())));
		vmp_func_new_return(get, get_type("vm", string(name<vm_int32>())));
		vmp_package_add_func(main_package, get);

		// {
		//	lda 0
		//	ldc_i4 10
		//	clt
		//	str 0
		//	ret
		// }
		vmp_func_begin_body(get);
		vmp_func_add_instr(get, vmp_instr_lda(0));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type("vm", string(name<vm_int32>())), vmp_const(10)));
		vmp_func_add_instr(get, vmp_instr_clt(get_type("vm", string(name<vm_int32>()))));
		vmp_func_add_instr(get, vmp_instr_str(0));
		vmp_func_add_instr(get, vmp_instr_ret());
		vmp_func_begin_end(get);

		compile();

		auto t = thread();
		vmi_thread_reserve_stack(t, sizeof(vm_int32));
		vmi_thread_push_i32(t, const_val);
		invoke(t, "Get");

		verify_stack_size(t, sizeof(vm_int32));
		verify_stack(t, 0, 10 < const_val ? TRUE : FALSE);

		destroy(t);

		end();
	}

	void clt()
	{
		TEST_FN(clt(100));
		TEST_FN(clt(1));
		TEST_FN(clt(-1));
	}

	void jmpt(const vm_int32 const_val)
	{
		begin();

		// Create the main package
		auto main_package = vmp_package_newsz("main", 4);
		vmp_pipeline_add_package(pipeline, main_package);

		// Create the Get function and add two integer types
		auto get = vmp_func_newsz("Get", 3);
		vmp_func_new_arg(get, get_type("vm", string(name<vm_int32>())));
		vmp_func_new_return(get, get_type("vm", string(name<vm_int32>())));
		vmp_package_add_func(main_package, get);

		// {
		//	lda 0
		//	ldc_i4 10
		//	cgt
		//	jmpt #marker
		//	ldc_i4 50
		//	str 0
		//  ret
		//	#marker
		//	ldc_i4 150
		//	str 0
		//	ret
		// }
		vmp_func_begin_body(get);
		auto marker = vmp_func_new_marker(get);
		vmp_func_add_instr(get, vmp_instr_lda(0));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type("vm", string(name<vm_int32>())), vmp_const(10)));
		vmp_func_add_instr(get, vmp_instr_cgt(get_type("vm", string(name<vm_int32>()))));
		vmp_func_add_instr(get, vmp_instr_jmpt(marker));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type("vm", string(name<vm_int32>())), vmp_const(50)));
		vmp_func_add_instr(get, vmp_instr_str(0));
		vmp_func_add_instr(get, vmp_instr_ret());
		vmp_marker_set_instr(marker,
			vmp_func_add_instr(get, vmp_instr_ldc(get_type("vm", string(name<vm_int32>())), vmp_const(150)))
		);
		vmp_func_add_instr(get, vmp_instr_str(0));
		vmp_func_add_instr(get, vmp_instr_ret());
		vmp_func_begin_end(get);

		compile();

		auto t = thread();
		vmi_thread_reserve_stack(t, sizeof(vm_int32));
		vmi_thread_push_i32(t, const_val);
		invoke(t, "Get");

		verify_stack_size(t, sizeof(vm_int32));
		verify_stack(t, 0, 10 > const_val ? 150 : 50);

		destroy(t);

		end();
	}

	void jmpt()
	{
		TEST_FN(jmpt(1));
		TEST_FN(jmpt(100));
		TEST_FN(jmpt(-1));
	}

	void jmpf(const vm_int32 const_val)
	{
		begin();

		// Create the main package
		auto main_package = vmp_package_newsz("main", 4);
		vmp_pipeline_add_package(pipeline, main_package);

		// Create the Get function and add two integer types
		auto get = vmp_func_newsz("Get", 3);
		vmp_func_new_arg(get, get_type("vm", string(name<vm_int32>())));
		vmp_func_new_return(get, get_type("vm", string(name<vm_int32>())));
		vmp_package_add_func(main_package, get);

		// {
		//	lda 0
		//	ldc_i4 10
		//	cgt
		//	jmpt #marker
		//	ldc_i4 50
		//	str 0
		//  ret
		//	#marker
		//	ldc_i4 150
		//	str 0
		//	ret
		// }
		vmp_func_begin_body(get);
		auto marker = vmp_func_new_marker(get);
		vmp_func_add_instr(get, vmp_instr_lda(0));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type("vm", string(name<vm_int32>())), vmp_const(10)));
		vmp_func_add_instr(get, vmp_instr_cgt(get_type("vm", string(name<vm_int32>()))));
		vmp_func_add_instr(get, vmp_instr_jmpf(marker));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type("vm", string(name<vm_int32>())), vmp_const(50)));
		vmp_func_add_instr(get, vmp_instr_str(0));
		vmp_func_add_instr(get, vmp_instr_ret());
		vmp_marker_set_instr(marker,
			vmp_func_add_instr(get, vmp_instr_ldc(get_type("vm", string(name<vm_int32>())), vmp_const(150)))
		);
		vmp_func_add_instr(get, vmp_instr_str(0));
		vmp_func_add_instr(get, vmp_instr_ret());
		vmp_func_begin_end(get);

		compile();

		auto t = thread();
		vmi_thread_reserve_stack(t, sizeof(vm_int32));
		vmi_thread_push_i32(t, const_val);
		invoke(t, "Get");

		verify_stack_size(t, sizeof(vm_int32));
		verify_stack(t, 0, 10 < const_val ? 150 : 50);

		destroy(t);

		end();
	}

	void jmpf()
	{
		TEST_FN(jmpf(1));
		TEST_FN(jmpf(100));
		TEST_FN(jmpf(-1));
	}

	template<typename FROM, typename TO>
	void conv_T_T(const FROM from, const TO to)
	{
		begin();

		// Create the main package
		auto main_package = vmp_package_newsz("main", 4);
		vmp_pipeline_add_package(pipeline, main_package);

		// Create the Get function and add two integer types
		auto get = vmp_func_newsz("Get", 3);
		vmp_func_new_arg(get, get_type("vm", string(name<FROM>())));
		vmp_func_new_return(get, get_type("vm", string(name<TO>())));
		vmp_package_add_func(main_package, get);

		// {
		//	lda 0
		//  conv_%FROM_%TO
		//	str 0
		//	ret
		// }
		vmp_func_begin_body(get);
		vmp_func_add_instr(get, vmp_instr_lda(0));
		vmp_func_add_instr(get, vmp_instr_conv(get_type("vm", string(name<FROM>())), get_type("vm", string(name<TO>()))));
		vmp_func_add_instr(get, vmp_instr_str(0));
		vmp_func_add_instr(get, vmp_instr_ret());
		vmp_func_begin_end(get);

		compile();

		auto t = thread();
		vmi_thread_reserve_stack(t, sizeof(TO));
		*((FROM*)vmi_thread_reserve_stack(t, sizeof(FROM))) = from;
		invoke(t, "Get");

		verify_stack_size(t, sizeof(TO));
		verify_stack(t, 0, (TO)to);

		destroy(t);

		end();
	}

	void conv()
	{
		TEST_FN(conv_T_T((vm_int32)INT32_MAX, (vm_int16)INT32_MAX));
		TEST_FN(conv_T_T((vm_int16)INT16_MAX, (vm_int32)INT16_MAX));
	}

	template<typename T>
	void ldl_a_T(T value)
	{
		begin();

		// Create the main package
		auto main_package = vmp_package_newsz("main", 4);
		vmp_pipeline_add_package(pipeline, main_package);

		// Create the Get function and add two integer types
		auto inner_get = vmp_func_newsz("innerGet", 8);
		vmp_func_new_arg(inner_get, get_type("vm", string(ptr<T>())));
		vmp_package_add_func(main_package, inner_get);

		// {
		//	lda 0
		//	ldc_T 100
		//	stunref int32
		//	ret
		// }
		vmp_func_begin_body(inner_get);
		vmp_func_add_instr(inner_get, vmp_instr_lda(0));
		vmp_func_add_instr(inner_get, vmp_instr_ldc(get_type("vm", string(name<T>())), vmp_const((T)value)));
		vmp_func_add_instr(inner_get, vmp_instr_sturef(get_type("vm", string(name<T>()))));
		vmp_func_add_instr(inner_get, vmp_instr_ret());
		vmp_func_begin_end(inner_get);

		// Create the Get function and add two integer types
		auto get = vmp_func_newsz("Get", 3);
		auto get_ret1 = vmp_func_new_return(get, get_type("vm", string(name<T>())));
		auto get_local1 = vmp_func_new_local(get, get_type("vm", string(name<T>())));
		vmp_local_set_namesz(get_local1, "i", 1);
		vmp_package_add_func(main_package, get);

		// {
		//	locals (i T)
		//	ldl_a 0
		//	call innerGet(*T)()
		//	ldl 0
		//	str 0
		//	ret
		// }
		vmp_func_begin_body(get);
		vmp_func_add_instr(get, vmp_instr_ldl_a(0));
		vmp_func_add_instr(get, vmp_instr_call(inner_get));
		vmp_func_add_instr(get, vmp_instr_ldl(0));
		vmp_func_add_instr(get, vmp_instr_str(0));
		vmp_func_add_instr(get, vmp_instr_ret());
		vmp_func_begin_end(get);

		compile();

		auto t = thread();
		vmi_thread_reserve_stack(t, sizeof(T));
		invoke(t, "Get");

		verify_stack_size(t, sizeof(T));
		verify_stack(t, 0, (T)value);

		destroy(t);

		end();
	}

	void ldl_a()
	{
		TEST_FN(ldl_a_T<vm_int8>((vm_int8)-120));
		TEST_FN(ldl_a_T<vm_int16>(INT16_MAX - 100));
		TEST_FN(ldl_a_T<vm_int32>((vm_int32)(INT32_MAX * 0.98)));
		TEST_FN(ldl_a_T<vm_int64>(INT64_MAX - INT16_MAX));
		TEST_FN(ldl_a_T<vm_float32>(123.45f));
		TEST_FN(ldl_a_T<vm_float64>(1234.4567));
	}

	void operator()()
	{
		TEST(add);
		TEST(ldc);
		TEST(call);
		TEST(locals);
		TEST(cgt);
		TEST(clt);
		TEST(jmpt);
		TEST(jmpf);
		TEST(conv);
		TEST(ldl_a);
	}
};


void suite_vmp()
{
	SUITE(suite_vmp_tests);
}
