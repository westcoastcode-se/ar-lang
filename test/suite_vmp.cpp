#include "utils.hpp"

struct utils_vmp : utils_vm
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

		vmp_memory_test_bytes_left();
	}

	void compile(vm_int32 opt_level = 0)
	{
		if (!vmp_pipeline_resolve(pipeline)) {
			end();
			throw_(error() << "failed to resolve pipeline information");
		}

		builder = vmp_builder_new(pipeline);
		vmp_builder_set_opt_level(builder, opt_level);
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
		if (result != 0) {
			end();
			throw_(error() << "failed to load bytecode because: " << result);
		}
	}

	vmi_thread* thread()
	{
		auto const t = vmi_thread_new(process);
		if (t == nullptr) {
			end();
			throw_(error() << "could not spawn a new VM thread");
		}
		return t;
	}

	void destroy(vmi_thread* p)
	{
		vmi_thread_destroy(p);
	}

	template<typename T>
	void push_value(vmi_thread* t, T value)
	{
		*(T*)vmi_thread_push_stack(t, sizeof(T)) = (T)value;
	}

	template<typename T, int SIZE>
	void push_value(vmi_thread* t, T value[SIZE])
	{
		T* p = (T*)vmi_thread_push_stack(t, sizeof(T[SIZE]));
		for (int i = 0; i < SIZE; ++i) 
			p[i] = value[i];
	}

	template<typename T>
	T pop_value(vmi_thread* t)
	{
		return *(T*)vmi_thread_pop_stack(t, sizeof(T));
	}

	template<typename T, int SIZE>
	vector<T> pop_value(vmi_thread* t)
	{
		T* arr = (T*)vmi_thread_pop_stack(t, sizeof(T[2]));
		return std::vector<T>(arr, arr + SIZE);
	}

	vmp_constant vmp_const(vm_int8 value) {
		return vmp_const_i1(value);
	}

	vmp_constant vmp_const(vm_uint8 value) {
		return vmp_const_ui1(value);
	}

	vmp_constant vmp_const(vm_int16 value) {
		return vmp_const_i2(value);
	}

	vmp_constant vmp_const(vm_uint16 value) {
		return vmp_const_ui2(value);
	}

	vmp_constant vmp_const(vm_int32 value) {
		return vmp_const_i4(value);
	}

	vmp_constant vmp_const(vm_uint32 value) {
		return vmp_const_ui4(value);
	}

	vmp_constant vmp_const(vm_int64 value) {
		return vmp_const_i8(value);
	}

	vmp_constant vmp_const(vm_uint64 value) {
		return vmp_const_ui8(value);
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
		if (package == NULL) {
			end();
			throw_(error() << "expected 'main' package but was not found");
		}

		const auto func = vmi_package_find_function_by_name(package, entry_point, (vm_int32)strlen(entry_point));
		if (func == NULL) {
			end();
			throw_(error() << "could not find function '" << entry_point << "'");
		}

		const auto result = vmi_process_exec(process, t, func);
		if (result != 0) {
			end();
			throw_(error() << "error occurred when executing thread: " << result << ". Message: " << t->exit_reason);
		}
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
};

struct suite_vmp_tests : utils_vmp
{
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
		//	ret
		// }
		vmp_func_begin_body(add);
		vmp_func_add_instr(add, vmp_instr_lda(1));
		vmp_func_add_instr(add, vmp_instr_lda(0));
		vmp_func_add_instr(add, vmp_instr_add(props1<T>()));
		vmp_func_add_instr(add, vmp_instr_lda(1));
		vmp_func_add_instr(add, vmp_instr_add(props1<T>()));
		vmp_func_add_instr(add, vmp_instr_ret());
		vmp_func_begin_end(add);

		compile();

		auto t = thread();
		push_value(t, (T)rhs);
		push_value(t, (T)lhs);
		invoke(t, "Add");

		verify_stack_size(t, sizeof(T) * 3);
		verify_value(pop_value<T>(t), (T)(lhs + rhs + rhs));
		verify_value(pop_value<T>(t), (T)(lhs));
		verify_value(pop_value<T>(t), (T)(rhs));

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
		//	ret
		// }
		vmp_func_begin_body(add);
		vmp_func_add_instr(add, vmp_instr_ldc(primitive_type, vmp_const((T)value)));
		vmp_func_add_instr(add, vmp_instr_ret());
		vmp_func_begin_end(add);

		compile();

		auto t = thread();
		invoke(t, "Get");

		verify_stack_size(t, sizeof(T));
		verify_value(pop_value<T>(t), (T)value);

		destroy(t);

		end();
	}

	void ldc()
	{
		TEST_FN(ldc_T<vm_int8>(12));
		TEST_FN(ldc_T<vm_uint8>(UINT8_MAX - 10));
		TEST_FN(ldc_T<vm_int16>(INT16_MAX - 10));
		TEST_FN(ldc_T<vm_uint16>(UINT16_MAX - 10));
		TEST_FN(ldc_T<vm_int32>(INT32_MAX - 1234));
		TEST_FN(ldc_T<vm_uint32>(UINT32_MAX - 1234));
		TEST_FN(ldc_T<vm_int64>(INT64_MAX - 123456));
		TEST_FN(ldc_T<vm_uint64>(UINT64_MAX - 123456));
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
		vmp_func_new_return(add, get_type("vm", string(name<vm_int32>())));
		vmp_package_add_func(main_package, add);

		// {
		//	lda 1
		//	lda 0
		//	add int32
		//	ret
		// }
		vmp_func_begin_body(add);
		vmp_func_add_instr(add, vmp_instr_lda(1));
		vmp_func_add_instr(add, vmp_instr_lda(0));
		vmp_func_add_instr(add, vmp_instr_add(props1<vm_int32>()));
		vmp_func_add_instr(add, vmp_instr_ret());
		vmp_func_begin_end(add);

		// Create the Get function and add two integer types
		auto get = vmp_func_newsz("Get", 3);
		vmp_func_new_return(get, get_type("vm", string(name<vm_int32>())));
		vmp_func_new_local(get, get_type("vm", string(name<vm_int32>())));
		vmp_package_add_func(main_package, get);

		// {
		//	locals (_1 int32)
		//	ldc_i4 <const_val2>
		//  ldc_i4 <const_val1>
		//	call Add()(int32)
		//	stl 0
		//	frees int32 * 2
		//  ldl 0
		//	ret
		// }
		auto const_val1 = 10;
		auto const_val2 = 20;
		vmp_func_begin_body(get);
		vmp_func_add_instr(get, vmp_instr_ldc(get_type("vm", string(name<vm_int32>())), vmp_const(const_val2)));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type("vm", string(name<vm_int32>())), vmp_const(const_val1)));
		vmp_func_add_instr(get, vmp_instr_call(add));
		vmp_func_add_instr(get, vmp_instr_stl(0));
		vmp_func_add_instr(get, vmp_instr_frees(get_type("vm", string(name<vm_int32>()))));
		vmp_func_add_instr(get, vmp_instr_frees(get_type("vm", string(name<vm_int32>()))));
		vmp_func_add_instr(get, vmp_instr_ldl(0));
		vmp_func_add_instr(get, vmp_instr_ret());
		vmp_func_begin_end(get);

		compile();

		auto t = thread();
		invoke(t, "Get");

		verify_stack_size(t, sizeof(vm_int32));
		verify_value(pop_value<vm_int32>(t), (vm_int32)(const_val1 + const_val2));

		destroy(t);

		end();
	}

	static int callnative_add(vmi_thread* t)
	{
		const auto rhs = *(vm_int32*)vmi_thread_getarg(t, 0);
		const auto lhs = *(vm_int32*)vmi_thread_getarg(t, 4);
		*(vm_int32*)vmi_thread_push_stack(t, sizeof(vm_int32)) = lhs + rhs;
		return 0;
	}

	void callnative()
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
		vmp_func_new_return(add, get_type("vm", string(name<vm_int32>())));
		vmp_func_set_nativefunc(add, &callnative_add);
		vmp_package_add_func(main_package, add);

		// Create the Get function and add two integer types
		auto get = vmp_func_newsz("Get", 3);
		vmp_func_new_return(get, get_type("vm", string(name<vm_int32>())));
		vmp_func_new_local(get, get_type("vm", string(name<vm_int32>())));
		vmp_package_add_func(main_package, get);

		// {
		//	locals (_1 int32)
		//	ldc_i4 <const_val2>
		//  ldc_i4 <const_val1>
		//	callnative Add()(int32)
		//	stl 0
		//	frees int32 * 2
		//  ldl 0
		//	ret
		// }
		auto const_val1 = 10;
		auto const_val2 = 20;
		vmp_func_begin_body(get);
		vmp_func_add_instr(get, vmp_instr_ldc(get_type("vm", string(name<vm_int32>())), vmp_const(const_val2)));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type("vm", string(name<vm_int32>())), vmp_const(const_val1)));
		vmp_func_add_instr(get, vmp_instr_callnative(add));
		vmp_func_add_instr(get, vmp_instr_stl(0));
		vmp_func_add_instr(get, vmp_instr_frees(get_type("vm", string(name<vm_int32>()))));
		vmp_func_add_instr(get, vmp_instr_frees(get_type("vm", string(name<vm_int32>()))));
		vmp_func_add_instr(get, vmp_instr_ldl(0));
		vmp_func_add_instr(get, vmp_instr_ret());
		vmp_func_begin_end(get);

		compile();

		auto t = thread();
		invoke(t, "Get");

		verify_stack_size(t, sizeof(vm_int32));
		verify_value(pop_value<vm_int32>(t), (vm_int32)(const_val1 + const_val2));

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
		vmp_func_add_instr(get, vmp_instr_ret());
		vmp_func_begin_end(get);

		compile();

		auto t = thread();
		invoke(t, "Get");

		verify_stack_size(t, sizeof(vm_int32));
		verify_value(pop_value<vm_int32>(t), 100 + 1);

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
		//	ret
		// }
		vmp_func_begin_body(get);
		vmp_func_add_instr(get, vmp_instr_lda(0));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type("vm", string(name<vm_int32>())), vmp_const(10)));
		vmp_func_add_instr(get, vmp_instr_cgt(get_type("vm", string(name<vm_int32>()))));
		vmp_func_add_instr(get, vmp_instr_ret());
		vmp_func_begin_end(get);

		compile();

		auto t = thread();
		vmi_thread_push_i32(t, const_val);
		invoke(t, "Get");

		verify_stack_size(t, sizeof(vm_int32) * 2);
		verify_value(pop_value<vm_int32>(t), 10 > const_val ? TRUE : FALSE);
		verify_value(pop_value<vm_int32>(t), const_val);

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
		//	ret
		// }
		vmp_func_begin_body(get);
		vmp_func_add_instr(get, vmp_instr_lda(0));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type("vm", string(name<vm_int32>())), vmp_const(10)));
		vmp_func_add_instr(get, vmp_instr_clt(get_type("vm", string(name<vm_int32>()))));
		vmp_func_add_instr(get, vmp_instr_ret());
		vmp_func_begin_end(get);

		compile();

		auto t = thread();
		vmi_thread_push_i32(t, const_val);
		invoke(t, "Get");

		verify_stack_size(t, sizeof(vm_int32) * 2);
		verify_value(pop_value<vm_int32>(t), 10 < const_val ? TRUE : FALSE);
		verify_value(pop_value<vm_int32>(t), const_val);

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
		//  ret
		//	#marker
		//	ldc_i4 150
		//	ret
		// }
		vmp_func_begin_body(get);
		auto marker = vmp_func_new_marker(get);
		vmp_func_add_instr(get, vmp_instr_lda(0));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type("vm", string(name<vm_int32>())), vmp_const(10)));
		vmp_func_add_instr(get, vmp_instr_cgt(get_type("vm", string(name<vm_int32>()))));
		vmp_func_add_instr(get, vmp_instr_jmpt(marker));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type("vm", string(name<vm_int32>())), vmp_const(50)));
		vmp_func_add_instr(get, vmp_instr_ret());
		vmp_marker_set_instr(marker,
			vmp_func_add_instr(get, vmp_instr_ldc(get_type("vm", string(name<vm_int32>())), vmp_const(150)))
		);
		vmp_func_add_instr(get, vmp_instr_ret());
		vmp_func_begin_end(get);

		compile();

		auto t = thread();
		push_value(t, const_val);
		invoke(t, "Get");

		verify_stack_size(t, sizeof(vm_int32) * 2);
		verify_value(pop_value<vm_int32>(t), 10 > const_val ? 150 : 50);
		verify_value(pop_value<vm_int32>(t), const_val);

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
		//  ret
		//	#marker
		//	ldc_i4 150
		//	ret
		// }
		vmp_func_begin_body(get);
		auto marker = vmp_func_new_marker(get);
		vmp_func_add_instr(get, vmp_instr_lda(0));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type("vm", string(name<vm_int32>())), vmp_const(10)));
		vmp_func_add_instr(get, vmp_instr_cgt(get_type("vm", string(name<vm_int32>()))));
		vmp_func_add_instr(get, vmp_instr_jmpf(marker));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type("vm", string(name<vm_int32>())), vmp_const(50)));
		vmp_func_add_instr(get, vmp_instr_ret());
		vmp_marker_set_instr(marker,
			vmp_func_add_instr(get, vmp_instr_ldc(get_type("vm", string(name<vm_int32>())), vmp_const(150)))
		);
		vmp_func_add_instr(get, vmp_instr_ret());
		vmp_func_begin_end(get);

		compile();

		auto t = thread();
		push_value(t, const_val);
		invoke(t, "Get");

		verify_stack_size(t, sizeof(vm_int32) * 2);
		verify_value(pop_value<vm_int32>(t), 10 < const_val ? 150 : 50);
		verify_value(pop_value<vm_int32>(t), const_val);

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
		//	ret
		// }
		vmp_func_begin_body(get);
		vmp_func_add_instr(get, vmp_instr_lda(0));
		vmp_func_add_instr(get, vmp_instr_conv(get_type("vm", string(name<FROM>())), get_type("vm", string(name<TO>()))));
		vmp_func_add_instr(get, vmp_instr_ret());
		vmp_func_begin_end(get);

		compile();

		auto t = thread();
		push_value(t, from);
		invoke(t, "Get");

		verify_stack_size(t, sizeof(FROM) + sizeof(TO));
		verify_value(pop_value<TO>(t), (TO)to);
		verify_value(pop_value<FROM>(t), (FROM)from);

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
		//  frees *T
		//	ldl 0
		//	ret
		// }
		vmp_func_begin_body(get);
		vmp_func_add_instr(get, vmp_instr_ldl_a(0));
		vmp_func_add_instr(get, vmp_instr_call(inner_get));
		vmp_func_add_instr(get, vmp_instr_frees(get_type("vm", string(ptr<T>()))));
		vmp_func_add_instr(get, vmp_instr_ldl(0));
		vmp_func_add_instr(get, vmp_instr_ret());
		vmp_func_begin_end(get);

		compile();

		auto t = thread();
		invoke(t, "Get");

		verify_stack_size(t, sizeof(T));
		verify_value(pop_value<T>(t), (T)value);

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

	template<typename T>
	void stelem_T(const array<T, 2>& values)
	{
		begin();

		// Create the main package
		auto main_package = vmp_package_newsz("main", 4);
		vmp_pipeline_add_package(pipeline, main_package);

		const auto array_type_name = string("[2]") + string(name<T>());

		// Create array type
		auto array_type = vmp_type_new_from_props(
			vmp_type_props_get(
				vmp_pipeline_get_string(pipeline, array_type_name.c_str(), (vm_int32)array_type_name.length()),
				sizeof(T[2]),
				VMP_TYPE_FLAGS_ARRAY,
				props1<T>(),
				get_type("vm", string(name<T>()))
			)
		);
		vmp_package_add_type(vmp_pipeline_find_packagesz(pipeline, "vm", 2), array_type);

		// Create the Get function and add two integer types
		auto get = vmp_func_newsz("Get", 3);
		vmp_func_new_return(get, array_type);
		vmp_func_new_local(get, array_type);
		vmp_package_add_func(main_package, get);

		// {
		//	locals (_1 [2]T)
		//	ldl_a 0
		//	ldc_i4 0
		//	ldc_i4 values[0]
		//	stelem [2]T
		//	ldl_a 0
		//	ldc_i4 1
		//	ldc_i4 values[0]
		//	stelem [2]T
		//	ldl [2]T
		//	ret
		// }
		vmp_func_begin_body(get);
		vmp_func_add_instr(get, vmp_instr_ldl_a(0));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type("vm", string(name<vm_int32>())), vmp_const(0)));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type("vm", string(name<T>())), vmp_const((T)values[0])));
		vmp_func_add_instr(get, vmp_instr_stelem(array_type));
		vmp_func_add_instr(get, vmp_instr_ldl_a(0));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type("vm", string(name<vm_int32>())), vmp_const(1)));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type("vm", string(name<T>())), vmp_const((T)values[1])));
		vmp_func_add_instr(get, vmp_instr_stelem(array_type));
		vmp_func_add_instr(get, vmp_instr_ldl(0));
		vmp_func_add_instr(get, vmp_instr_ret());
		vmp_func_begin_end(get);

		compile();

		auto t = thread();
		invoke(t, "Get");

		verify_stack_size(t, sizeof(T[2]));
		auto ret = pop_value<T, 2>(t);
		verify_value(ret[0], values[0]);
		verify_value(ret[1], values[1]);

		destroy(t);

		end();
	}

	template<typename T>
	void stelem_ptr_T(const array<T, 2>& values)
	{
		begin();

		// Create the main package
		auto main_package = vmp_package_newsz("main", 4);
		vmp_pipeline_add_package(pipeline, main_package);
		
		// Create array type
		auto ptr_type = get_type("vm", string(ptr<T>()));

		// Create the Get function and add two integer types
		auto get = vmp_func_newsz("Get", 3);
		auto get_arg1 = vmp_func_new_arg(get, ptr_type);
		vmp_package_add_func(main_package, get);

		// {
		//	lda 0
		//	ldc_i4 0
		//	ldc_T values[0]
		//	stelem [2]T
		//	lda 0
		//	ldc_i4 1
		//	ldc_T values[0]
		//	stelem [2]T
		//	ret
		// }
		vmp_func_begin_body(get);
		vmp_func_add_instr(get, vmp_instr_lda(0));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type("vm", string(name<vm_int32>())), vmp_const(0)));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type("vm", string(name<T>())), vmp_const((T)values[0])));
		vmp_func_add_instr(get, vmp_instr_stelem(ptr_type));
		vmp_func_add_instr(get, vmp_instr_lda(0));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type("vm", string(name<vm_int32>())), vmp_const(1)));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type("vm", string(name<T>())), vmp_const((T)values[1])));
		vmp_func_add_instr(get, vmp_instr_stelem(ptr_type));
		vmp_func_add_instr(get, vmp_instr_ret());
		vmp_func_begin_end(get);

		compile();

		auto t = thread();
		push_value<T*>(t, new T[values.size()]);
		invoke(t, "Get");

		verify_stack_size(t, sizeof(T*));
		auto ret_as_arg = pop_value<T*>(t);
		verify_value(ret_as_arg[0], values[0]);
		verify_value(ret_as_arg[1], values[1]);
		delete[] ret_as_arg;

		destroy(t);

		end();
	}

	void stelem()
	{
		TEST_FN(stelem_T<vm_int8>({ 1, -1 }));
		TEST_FN(stelem_T<vm_uint8>({ INT8_MAX + 1, UINT8_MAX - 1 }));
		TEST_FN(stelem_T<vm_int16>({ UINT8_MAX + 1, INT16_MAX - 100 }));
		TEST_FN(stelem_T<vm_uint16>({ INT16_MAX + 1, UINT16_MAX - 1 }));
		TEST_FN(stelem_T<vm_int32>({ UINT16_MAX + 1, INT32_MAX - 100 }));
		TEST_FN(stelem_T<vm_uint32>({ INT32_MAX + 1u, UINT32_MAX - 100u }));
		TEST_FN(stelem_T<vm_int64>({ UINT32_MAX + 1ll, INT64_MAX - 100ll }));
		TEST_FN(stelem_T<vm_uint64>({ (vm_uint64)INT64_MAX + (vm_uint64)1, (vm_uint64)UINT64_MAX - (vm_uint64)100 }));
		TEST_FN(stelem_T<vm_float32>({ 123.43f, 657.123f }));
		TEST_FN(stelem_T<vm_float64>({ 123.4345, 657.12312 }));

		TEST_FN(stelem_ptr_T<vm_int8>({ 1, -1 }));
		TEST_FN(stelem_ptr_T<vm_uint8>({ INT8_MAX + 1, UINT8_MAX - 1 }));
		TEST_FN(stelem_ptr_T<vm_int16>({ UINT8_MAX + 1, INT16_MAX - 100 }));
		TEST_FN(stelem_ptr_T<vm_uint16>({ INT16_MAX + 1, UINT16_MAX - 1 }));
		TEST_FN(stelem_ptr_T<vm_int32>({ UINT16_MAX + 1, INT32_MAX - 100 }));
		TEST_FN(stelem_ptr_T<vm_uint32>({ INT32_MAX + 1u, UINT32_MAX - 100u }));
		TEST_FN(stelem_ptr_T<vm_int64>({ UINT32_MAX + 1ll, INT64_MAX - 100ll }));
		TEST_FN(stelem_ptr_T<vm_uint64>({ (vm_uint64)INT64_MAX + (vm_uint64)1, (vm_uint64)UINT64_MAX - (vm_uint64)100 }));
		TEST_FN(stelem_ptr_T<vm_float32>({ 123.43f, 657.123f }));
		TEST_FN(stelem_ptr_T<vm_float64>({ 123.4345, 657.12312 }));
	}

	template<typename T>
	void ldelem_T(const array<T, 2>& values)
	{
		begin();

		// Create the main package
		auto main_package = vmp_package_newsz("main", 4);
		vmp_pipeline_add_package(pipeline, main_package);

		const auto array_type_name = string("[2]") + string(name<T>());

		// Create array type
		auto array_type = vmp_type_new_from_props(
			vmp_type_props_get(
				vmp_pipeline_get_string(pipeline, array_type_name.c_str(), (vm_int32)array_type_name.length()),
				sizeof(T[2]),
				VMP_TYPE_FLAGS_ARRAY,
				props1<T>(),
				get_type("vm", string(name<T>()))
			)
		);
		vmp_package_add_type(vmp_pipeline_find_packagesz(pipeline, "vm", 2), array_type);

		// Create the Get function and add two integer types
		auto get = vmp_func_newsz("Get", 3);
		auto get_arg1 = vmp_func_new_arg(get, array_type);
		auto get_ret1 = vmp_func_new_return(get, get_type("vm", string(name<T>())));
		auto get_ret2 = vmp_func_new_return(get, get_type("vm", string(name<T>())));
		vmp_package_add_func(main_package, get);

		// {
		//	lda_a 0
		//	ldc_i4 0
		//	ldelem [2]T
		//	lda_a 0
		//	ldc_i4 1
		//	ldelem [2]T
		//	ret
		// }
		vmp_func_begin_body(get);
		vmp_func_add_instr(get, vmp_instr_lda_a(0));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type("vm", string(name<vm_int32>())), vmp_const(0)));
		vmp_func_add_instr(get, vmp_instr_ldelem(array_type));
		vmp_func_add_instr(get, vmp_instr_lda_a(0));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type("vm", string(name<vm_int32>())), vmp_const(1)));
		vmp_func_add_instr(get, vmp_instr_ldelem(array_type));
		vmp_func_add_instr(get, vmp_instr_ret());
		vmp_func_begin_end(get);

		compile();

		auto t = thread();
		T values_as_array[2] = { values[0], values[1] };
		push_value<T, 2>(t, values_as_array);
		invoke(t, "Get");

		verify_stack_size(t, sizeof(T) * 4);
		verify_value(pop_value<T>(t), values[1]);
		verify_value(pop_value<T>(t), values[0]);
		auto popped_in = pop_value<T, 2>(t);
		verify_value(popped_in[1], values[1]);
		verify_value(popped_in[0], values[0]);

		destroy(t);

		end();
	}

	template<typename T>
	void ldelem_ptr_T(const vector<T>& values)
	{
		begin();

		// Create the main package
		auto main_package = vmp_package_newsz("main", 4);
		vmp_pipeline_add_package(pipeline, main_package);

		// Create array type
		auto ptr_type = get_type("vm", string(ptr<T>()));

		// Create the Get function and add two integer types
		auto get = vmp_func_newsz("Get", 3);
		auto get_arg1 = vmp_func_new_arg(get, ptr_type);
		auto get_ret1 = vmp_func_new_return(get, get_type("vm", string(name<T>())));
		auto get_ret2 = vmp_func_new_return(get, get_type("vm", string(name<T>())));
		vmp_package_add_func(main_package, get);

		// {
		//	lda 0
		//	ldc_i4 0
		//	ldelem *T
		//	lda 0
		//	ldc_i4 1
		//	ldelem *T
		//	ret
		// }
		vmp_func_begin_body(get);
		vmp_func_add_instr(get, vmp_instr_lda(0));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type("vm", string(name<vm_int32>())), vmp_const(0)));
		vmp_func_add_instr(get, vmp_instr_ldelem(ptr_type));
		vmp_func_add_instr(get, vmp_instr_lda(0));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type("vm", string(name<vm_int32>())), vmp_const(1)));
		vmp_func_add_instr(get, vmp_instr_ldelem(ptr_type));
		vmp_func_add_instr(get, vmp_instr_ret());
		vmp_func_begin_end(get);

		compile();

		auto t = thread();
		push_value<T*>(t, (T*)values.data());
		invoke(t, "Get");

		verify_stack_size(t, sizeof(T*) + sizeof(T) * 2);
		verify_value(pop_value<T>(t), values[1]);
		verify_value(pop_value<T>(t), values[0]);
		T* popped_in = pop_value<T*>(t);
		verify_value(popped_in[1], values[1]);
		verify_value(popped_in[0], values[0]);

		destroy(t);

		end();
	}

	void ldelem()
	{
		TEST_FN(ldelem_T<vm_int8>({ 1, -1 }));
		TEST_FN(ldelem_T<vm_uint8>({ INT8_MAX + 1, UINT8_MAX - 1 }));
		TEST_FN(ldelem_T<vm_int16>({ UINT8_MAX + 1, INT16_MAX - 100 }));
		TEST_FN(ldelem_T<vm_uint16>({ INT16_MAX + 1, UINT16_MAX - 1 }));
		TEST_FN(ldelem_T<vm_int32>({ UINT16_MAX + 1, INT32_MAX - 100 }));
		TEST_FN(ldelem_T<vm_uint32>({ INT32_MAX + 1u, UINT32_MAX - 100u }));
		TEST_FN(ldelem_T<vm_int64>({ UINT32_MAX + 1ll, INT64_MAX - 100ll }));
		TEST_FN(ldelem_T<vm_uint64>({ (vm_uint64)INT64_MAX + (vm_uint64)1, (vm_uint64)UINT64_MAX - (vm_uint64)100 }));
		TEST_FN(ldelem_T<vm_float32>({ 123.43f, 657.123f }));
		TEST_FN(ldelem_T<vm_float64>({ 123.4345, 657.12312 }));

		TEST_FN(ldelem_ptr_T<vm_int8>({ 1, -1 }));
		TEST_FN(ldelem_ptr_T<vm_uint8>({ INT8_MAX + 1, UINT8_MAX - 1 }));
		TEST_FN(ldelem_ptr_T<vm_int16>({ UINT8_MAX + 1, INT16_MAX - 100 }));
		TEST_FN(ldelem_ptr_T<vm_uint16>({ INT16_MAX + 1, UINT16_MAX - 1 }));
		TEST_FN(ldelem_ptr_T<vm_int32>({ UINT16_MAX + 1, INT32_MAX - 100 }));
		TEST_FN(ldelem_ptr_T<vm_uint32>({ INT32_MAX + 1u, UINT32_MAX - 100u }));
		TEST_FN(ldelem_ptr_T<vm_int64>({ UINT32_MAX + 1ll, INT64_MAX - 100ll }));
		TEST_FN(ldelem_ptr_T<vm_uint64>({ (vm_uint64)INT64_MAX + (vm_uint64)1, (vm_uint64)UINT64_MAX - (vm_uint64)100 }));
		TEST_FN(ldelem_ptr_T<vm_float32>({ 123.43f, 657.123f }));
		TEST_FN(ldelem_ptr_T<vm_float64>({ 123.4345, 657.12312 }));
	}

	void allocs_const()
	{
		begin();

		// Create the main package
		auto main_package = vmp_package_newsz("main", 4);
		vmp_pipeline_add_package(pipeline, main_package);

		// Create the Get function and add two integer types
		auto _do = vmp_func_newsz("Do", 2);
		vmp_package_add_func(main_package, _do);

		// {
		//	allocs 4
		//	frees 4
		//	ret
		// }
		vmp_func_begin_body(_do);
		vmp_func_add_instr(_do, vmp_instr_allocs_const(4));
		vmp_func_add_instr(_do, vmp_instr_frees_const(4));
		vmp_func_add_instr(_do, vmp_instr_ret());
		vmp_func_begin_end(_do);

		compile();

		auto t = thread();
		invoke(t, "Do");

		verify_stack_size(t, 0);

		destroy(t);

		end();
	}

	template<typename T>
	void allocs_T()
	{
		begin();

		// Create the main package
		auto main_package = vmp_package_newsz("main", 4);
		vmp_pipeline_add_package(pipeline, main_package);

		auto type = get_type("vm", string(name<T>()));

		// Create the Get function and add two integer types
		auto _do = vmp_func_newsz("Do", 2);
		vmp_package_add_func(main_package, _do);

		// {
		//	allocs T
		//	frees T
		//	ret
		// }
		vmp_func_begin_body(_do);
		vmp_func_add_instr(_do, vmp_instr_allocs(type));
		vmp_func_add_instr(_do, vmp_instr_frees(type));
		vmp_func_add_instr(_do, vmp_instr_ret());
		vmp_func_begin_end(_do);

		compile();

		auto t = thread();
		invoke(t, "Do");

		verify_stack_size(t, 0);

		destroy(t);

		end();
	}

	void allocs()
	{
		TEST_FN(allocs_T<vm_int8>());
		TEST_FN(allocs_T<vm_uint8>());
		TEST_FN(allocs_T<vm_int16>());
		TEST_FN(allocs_T<vm_uint16>());
		TEST_FN(allocs_T<vm_int32>());
		TEST_FN(allocs_T<vm_uint32>());
		TEST_FN(allocs_T<vm_int64>());
		TEST_FN(allocs_T<vm_uint64>());
		TEST_FN(allocs_T<vm_float32>());
		TEST_FN(allocs_T<vm_float64>());
	}

	void alloch_const()
	{
		begin();

		// Create the main package
		auto main_package = vmp_package_newsz("main", 4);
		vmp_pipeline_add_package(pipeline, main_package);

		// Create the Get function and add two integer types
		auto _do = vmp_func_newsz("Do", 2);
		vmp_package_add_func(main_package, _do);

		// {
		//	alloch 4
		//	freeh
		//	ret
		// }
		vmp_func_begin_body(_do);
		vmp_func_add_instr(_do, vmp_instr_alloch_const(4));
		vmp_func_add_instr(_do, vmp_instr_freeh());
		vmp_func_add_instr(_do, vmp_instr_ret());
		vmp_func_begin_end(_do);

		compile();

		auto t = thread();
		invoke(t, "Do");

		verify_stack_size(t, 0);

		destroy(t);

		end();
	}

	template<typename T>
	void alloch_T()
	{
		begin();

		// Create the main package
		auto main_package = vmp_package_newsz("main", 4);
		vmp_pipeline_add_package(pipeline, main_package);

		auto type = get_type("vm", string(name<T>()));

		// Create the Get function and add two integer types
		auto _do = vmp_func_newsz("Do", 2);
		vmp_package_add_func(main_package, _do);

		// {
		//	ldc_i4 sizeof(T)
		//	alloch T
		//	freeh T
		//	ret
		// }
		vmp_func_begin_body(_do);
		vmp_func_add_instr(_do, vmp_instr_ldc(get_type("vm", string("int32")), vmp_const((vm_int32)sizeof(T))));
		vmp_func_add_instr(_do, vmp_instr_alloch());
		vmp_func_add_instr(_do, vmp_instr_freeh());
		vmp_func_add_instr(_do, vmp_instr_ret());
		vmp_func_begin_end(_do);

		compile();

		auto t = thread();
		invoke(t, "Do");

		verify_stack_size(t, 0);

		destroy(t);

		end();
	}

	void alloch()
	{
		TEST_FN(alloch_T<vm_int8>());
		TEST_FN(alloch_T<vm_uint8>());
		TEST_FN(alloch_T<vm_int16>());
		TEST_FN(alloch_T<vm_uint16>());
		TEST_FN(alloch_T<vm_int32>());
		TEST_FN(alloch_T<vm_uint32>());
		TEST_FN(alloch_T<vm_int64>());
		TEST_FN(alloch_T<vm_uint64>());
		TEST_FN(alloch_T<vm_float32>());
		TEST_FN(alloch_T<vm_float64>());
	}

	template<typename T>
	void alloch_type_T()
	{
		begin();

		// Create the main package
		auto main_package = vmp_package_newsz("main", 4);
		vmp_pipeline_add_package(pipeline, main_package);

		auto type = get_type("vm", string(name<T>()));

		// Create the Get function and add two integer types
		auto _do = vmp_func_newsz("Do", 2);
		vmp_package_add_func(main_package, _do);

		// {
		//	alloch T
		//	freeh T
		//	ret
		// }
		vmp_func_begin_body(_do);
		vmp_func_add_instr(_do, vmp_instr_alloch_type(type));
		vmp_func_add_instr(_do, vmp_instr_freeh());
		vmp_func_add_instr(_do, vmp_instr_ret());
		vmp_func_begin_end(_do);

		compile();

		auto t = thread();
		invoke(t, "Do");

		verify_stack_size(t, 0);

		destroy(t);

		end();
	}

	void alloch_type()
	{
		TEST_FN(alloch_type_T<vm_int8>());
		TEST_FN(alloch_type_T<vm_uint8>());
		TEST_FN(alloch_type_T<vm_int16>());
		TEST_FN(alloch_type_T<vm_uint16>());
		TEST_FN(alloch_type_T<vm_int32>());
		TEST_FN(alloch_type_T<vm_uint32>());
		TEST_FN(alloch_type_T<vm_int64>());
		TEST_FN(alloch_type_T<vm_uint64>());
		TEST_FN(alloch_type_T<vm_float32>());
		TEST_FN(alloch_type_T<vm_float64>());
	}

	void operator()()
	{
		TEST(add);
		TEST(ldc);
		TEST(call);
		TEST(callnative);
		TEST(locals);
		TEST(cgt);
		TEST(clt);
		TEST(jmpt);
		TEST(jmpf);
		TEST(conv);
		TEST(ldl_a);
		TEST(stelem);
		TEST(ldelem);
		TEST(allocs_const);
		TEST(allocs);
		TEST(alloch_type);
		TEST(alloch_const);
		TEST(alloch);
	}
};

void suite_vmp()
{
	SUITE(suite_vmp_tests);
}
