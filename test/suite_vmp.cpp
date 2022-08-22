#include "utils.hpp"

struct utils_vmp : utils_vm
{
	vmp_pipeline* pipeline;
	vmp_builder* builder;

	void beforeEach()
	{
		pipeline = vmp_pipeline_new();
		if (pipeline == nullptr)
			throw_(error() << "could not create a new VM pipeline");
		vmp_pipeline_add_vm_package(pipeline);
		builder = NULL;
	}

	void afterEach()
	{
		if (builder != NULL) {
			vmp_builder_destroy(builder);
			builder = NULL;
		}

		if (pipeline != NULL) {
			vmp_pipeline_destroy(pipeline);
			pipeline = NULL;
		}

		vm_memory_test_bytes_left();
	}

	vmp_const to_const(vm_int8 value) {
		return vmp_const_i1(value);
	}

	vmp_const to_const(vm_uint8 value) {
		return vmp_const_ui1(value);
	}

	vmp_const to_const(vm_int16 value) {
		return vmp_const_i2(value);
	}

	vmp_const to_const(vm_uint16 value) {
		return vmp_const_ui2(value);
	}

	vmp_const to_const(vm_int32 value) {
		return vmp_const_i4(value);
	}

	vmp_const to_const(vm_uint32 value) {
		return vmp_const_ui4(value);
	}

	vmp_const to_const(vm_int64 value) {
		return vmp_const_i8(value);
	}

	vmp_const to_const(vm_uint64 value) {
		return vmp_const_ui8(value);
	}

	vmp_const to_const(vm_float32 value) {
		return vmp_const_f4(value);
	}

	vmp_const to_const(vm_float64 value) {
		return vmp_const_f8(value);
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

	vmp_type* get_type(const string type_name) {
		return get_type(string("vm"), type_name);
	}

	template<typename T>
	vmp_type* get_type() {
		return get_type(string("vm"), name<T>());
	}

	vm_int32 verify_instr(vmp_func* func, vmp_instr* instr, vmp_instr_type type, vm_int32 offset, size_t size)
	{
		verify_value<const vmp_func*>(instr->func, func);
		verify_value(instr->instr_type, type);
		verify_value(instr->instr_offset, offset);
		verify_value(instr->instr_size, (vm_int32)size);
		return (vm_int32)(offset + size);
	}
};

struct utils_vmp_vmi : utils_vmp
{
	vmi_process* process;
	vmi_thread* thread;

	void beforeEach()
	{
		utils_vmp::beforeEach();
	}

	void afterEach()
	{
		if (thread != nullptr) {
			vmi_thread_destroy(thread);
			thread = nullptr;
		}

		if (process != nullptr) {
			vmi_process_destroy(process);
			process = nullptr;
		}

		utils_vmp::afterEach();
	}

	void compile(vm_int32 opt_level = 0)
	{
		if (!vmp_pipeline_resolve(pipeline)) {
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
			throw_(e);
		}

		process = vmi_process_new();
		if (process == nullptr)
			throw_(error() << "could not create a new VM process");

		const auto result = vmi_process_load(process, vmp_builder_get_bytecode(builder));
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
	T push_value(T value)
	{
		*(T*)vmi_thread_push_stack(thread, sizeof(T)) = (T)value;
		return value;
	}

	template<typename T, int SIZE>
	T* push_value(T value[SIZE])
	{
		T* p = (T*)vmi_thread_push_stack(thread, sizeof(T[SIZE]));
		for (int i = 0; i < SIZE; ++i)
			p[i] = value[i];
		return p;
	}

	template<typename T>
	T pop_value()
	{
		return *(T*)vmi_thread_pop_stack(thread, sizeof(T));
	}

	template<typename T, int SIZE>
	vector<T> pop_value()
	{
		T* arr = (T*)vmi_thread_pop_stack(thread, sizeof(T[2]));
		return std::vector<T>(arr, arr + SIZE);
	}


	void verify_stack_size(size_t expected_size)
	{
		const size_t size = (size_t)(thread->stack.top - thread->stack.blocks);
		if (expected_size != size) {
			throw_(error() << "expected stack size to be " << expected_size << " but was " << size);
		}
	}

};

struct suite_vmp_tests : utils_vmp
{
	void instr_offset()
	{
		vm_string name;
		name.start = "test";
		name.end = name.start + 4;
		vmp_func* const func = vmp_func_new(&name);
		verify_not_null(func);
		vmp_func_add_instr(func, vmp_instr_lda(0));
		vmp_func_add_instr(func, vmp_instr_lda(0));
		vmp_func_add_instr(func, vmp_instr_lda(0));

		verify_value(func->body_size, (vm_uint32)(sizeof(vmi_instr_lda) * 3));
		vm_int32 offset = 0;
		offset = verify_instr(func, func->first_instr, VMP_INSTR_LDA, offset, sizeof(vmi_instr_lda));
		offset = verify_instr(func, func->first_instr->next, VMP_INSTR_LDA, offset, sizeof(vmi_instr_lda));
		offset = verify_instr(func, func->first_instr->next->next, VMP_INSTR_LDA, offset, sizeof(vmi_instr_lda));
		verify_null(func->first_instr->next->next->next);

		vmp_func_destroy(func);
	}

	void instr_offset_inject_start()
	{
		vm_string name;
		name.start = "test";
		name.end = name.start + 4;
		vmp_func* const func = vmp_func_new(&name);
		verify_not_null(func);
		vmp_func_add_instr(func, vmp_instr_lda(0));
		vmp_func_add_instr(func, vmp_instr_lda(0));
		vmp_func_add_instr(func, vmp_instr_lda(0));
		vmp_func_inject_after(func, NULL, vmp_instr_lda_a(0));

		verify_value(func->body_size, (vm_uint32)(sizeof(vmi_instr_lda) * 3 + sizeof(vmi_instr_lda_a)));

		vm_int32 offset = 0;
		offset = verify_instr(func, func->first_instr, VMP_INSTR_LDA_A, offset, sizeof(vmi_instr_lda_a));
		offset = verify_instr(func, func->first_instr->next, VMP_INSTR_LDA, offset, sizeof(vmi_instr_lda));
		offset = verify_instr(func, func->first_instr->next->next, VMP_INSTR_LDA, offset, sizeof(vmi_instr_lda));
		offset = verify_instr(func, func->first_instr->next->next->next, VMP_INSTR_LDA, offset, sizeof(vmi_instr_lda));
		verify_null(func->first_instr->next->next->next->next);

		vmp_func_destroy(func);
	}

	void instr_offset_inject_start_multiple()
	{
		vm_string name;
		name.start = "test";
		name.end = name.start + 4;
		vmp_func* const func = vmp_func_new(&name);
		verify_not_null(func);
		vmp_func_add_instr(func, vmp_instr_lda(0));
		vmp_func_add_instr(func, vmp_instr_lda(0));
		vmp_func_add_instr(func, vmp_instr_lda(0));
		vmp_func_inject_after(func, NULL, vmp_instr_link(vmp_instr_lda_a(0), vmp_instr_stl(0)));

		verify_value(func->body_size, (vm_uint32)(sizeof(vmi_instr_lda) * 3 + sizeof(vmi_instr_lda_a) + sizeof(vmi_instr_stl)));

		vm_int32 offset = 0;
		offset = verify_instr(func, func->first_instr, VMP_INSTR_LDA_A, offset, sizeof(vmi_instr_lda_a));
		offset = verify_instr(func, func->first_instr->next, VMP_INSTR_STL, offset, sizeof(vmi_instr_stl));
		offset = verify_instr(func, func->first_instr->next->next, VMP_INSTR_LDA, offset, sizeof(vmi_instr_lda));
		offset = verify_instr(func, func->first_instr->next->next->next, VMP_INSTR_LDA, offset, sizeof(vmi_instr_lda));
		offset = verify_instr(func, func->first_instr->next->next->next->next, VMP_INSTR_LDA, offset, sizeof(vmi_instr_lda));
		verify_null(func->first_instr->next->next->next->next->next);

		vmp_func_destroy(func);
	}

	void instr_offset_inject_end()
	{
		vm_string name;
		name.start = "test";
		name.end = name.start + 4;
		vmp_func* const func = vmp_func_new(&name);
		verify_not_null(func);
		vmp_func_add_instr(func, vmp_instr_lda(0));
		vmp_func_add_instr(func, vmp_instr_lda(0));
		vmp_func_add_instr(func, vmp_instr_lda(0));
		vmp_func_inject_after(func, func->last_instr, vmp_instr_lda_a(0));

		verify_value(func->body_size, (vm_uint32)(sizeof(vmi_instr_lda) * 3 + sizeof(vmi_instr_lda_a)));

		vm_int32 offset = 0;
		offset = verify_instr(func, func->first_instr, VMP_INSTR_LDA, offset, sizeof(vmi_instr_lda));
		offset = verify_instr(func, func->first_instr->next, VMP_INSTR_LDA, offset, sizeof(vmi_instr_lda));
		offset = verify_instr(func, func->first_instr->next->next, VMP_INSTR_LDA, offset, sizeof(vmi_instr_lda));
		offset = verify_instr(func, func->first_instr->next->next->next, VMP_INSTR_LDA_A, offset, sizeof(vmi_instr_lda_a));
		verify_null(func->first_instr->next->next->next->next);

		vmp_func_destroy(func);
	}

	void instr_offset_inject_end_multiple()
	{
		vm_string name;
		name.start = "test";
		name.end = name.start + 4;
		vmp_func* const func = vmp_func_new(&name);
		verify_not_null(func);
		vmp_func_add_instr(func, vmp_instr_lda(0));
		vmp_func_add_instr(func, vmp_instr_lda(0));
		vmp_func_add_instr(func, vmp_instr_lda(0));
		vmp_func_inject_after(func, func->last_instr, vmp_instr_link(vmp_instr_lda_a(0), vmp_instr_stl(0)));

		verify_value(func->body_size, (vm_uint32)(sizeof(vmi_instr_lda) * 3 + sizeof(vmi_instr_lda_a) + sizeof(vmi_instr_stl)));

		vm_int32 offset = 0;
		offset = verify_instr(func, func->first_instr, VMP_INSTR_LDA, offset, sizeof(vmi_instr_lda));
		offset = verify_instr(func, func->first_instr->next, VMP_INSTR_LDA, offset, sizeof(vmi_instr_lda));
		offset = verify_instr(func, func->first_instr->next->next, VMP_INSTR_LDA, offset, sizeof(vmi_instr_lda));
		offset = verify_instr(func, func->first_instr->next->next->next, VMP_INSTR_LDA_A, offset, sizeof(vmi_instr_lda_a));
		offset = verify_instr(func, func->first_instr->next->next->next->next, VMP_INSTR_STL, offset, sizeof(vmi_instr_stl));
		verify_null(func->first_instr->next->next->next->next->next);

		vmp_func_destroy(func);
	}

	void instr_offset_inject_middle()
	{
		vm_string name;
		name.start = "test";
		name.end = name.start + 4;
		vmp_func* const func = vmp_func_new(&name);
		verify_not_null(func);
		vmp_func_add_instr(func, vmp_instr_lda(0));
		vmp_func_add_instr(func, vmp_instr_lda(0));
		vmp_func_add_instr(func, vmp_instr_lda(0));
		vmp_func_inject_after(func, func->first_instr, vmp_instr_lda_a(0));

		verify_value(func->body_size, (vm_uint32)(sizeof(vmi_instr_lda) * 3 + sizeof(vmi_instr_lda_a)));

		vm_int32 offset = 0;
		offset = verify_instr(func, func->first_instr, VMP_INSTR_LDA, offset, sizeof(vmi_instr_lda));
		offset = verify_instr(func, func->first_instr->next, VMP_INSTR_LDA_A, offset, sizeof(vmi_instr_lda_a));
		offset = verify_instr(func, func->first_instr->next->next, VMP_INSTR_LDA, offset, sizeof(vmi_instr_lda));
		offset = verify_instr(func, func->first_instr->next->next->next, VMP_INSTR_LDA, offset, sizeof(vmi_instr_lda));
		verify_null(func->first_instr->next->next->next->next);

		vmp_func_destroy(func);
	}

	void instr_offset_inject_middle_multiple()
	{
		vm_string name;
		name.start = "test";
		name.end = name.start + 4;
		vmp_func* const func = vmp_func_new(&name);
		verify_not_null(func);
		vmp_func_add_instr(func, vmp_instr_lda(0));
		vmp_func_add_instr(func, vmp_instr_lda(0));
		vmp_func_add_instr(func, vmp_instr_lda(0));
		vmp_func_inject_after(func, func->first_instr, vmp_instr_link(vmp_instr_lda_a(0), vmp_instr_stl(0)));

		verify_value(func->body_size, (vm_uint32)(sizeof(vmi_instr_lda) * 3 + sizeof(vmi_instr_lda_a) + sizeof(vmi_instr_stl)));

		vm_int32 offset = 0;
		offset = verify_instr(func, func->first_instr, VMP_INSTR_LDA, offset, sizeof(vmi_instr_lda));
		offset = verify_instr(func, func->first_instr->next, VMP_INSTR_LDA_A, offset, sizeof(vmi_instr_lda_a));
		offset = verify_instr(func, func->first_instr->next->next, VMP_INSTR_STL, offset, sizeof(vmi_instr_stl));
		offset = verify_instr(func, func->first_instr->next->next->next, VMP_INSTR_LDA, offset, sizeof(vmi_instr_lda));
		offset = verify_instr(func, func->first_instr->next->next->next->next, VMP_INSTR_LDA, offset, sizeof(vmi_instr_lda));
		verify_null(func->first_instr->next->next->next->next->next);

		vmp_func_destroy(func);
	}

	void operator()()
	{
		TEST(instr_offset());
		TEST(instr_offset_inject_start());
		TEST(instr_offset_inject_start_multiple());
		TEST(instr_offset_inject_end());
		TEST(instr_offset_inject_end_multiple());
		TEST(instr_offset_inject_middle());
		TEST(instr_offset_inject_middle_multiple());
	}
};

struct suite_vmp_neg : utils_vmp_vmi
{
	template<typename T>
	void neg_T(T value)
	{
		// Create the main package
		auto main_package = vmp_package_newsz("main", 4);
		vmp_pipeline_add_package(pipeline, main_package);

		// Create the Add function and add two integer types
		auto add = vmp_func_newsz("Neg", 3);
		auto arg1 = vmp_func_new_arg(add, get_type<T>());
		vmp_arg_set_namesz(arg1, "lhs", 3);
		auto ret1 = vmp_func_new_return(add, get_type<T>());
		vmp_package_add_func(main_package, add);

		// {
		//	lda 0
		//	neg T
		//	ret
		// }
		vmp_func_begin_body(add);
		vmp_func_add_instr(add, vmp_instr_lda(arg1));
		vmp_func_add_instr(add, vmp_instr_neg(get_type<T>()));
		vmp_func_add_instr(add, vmp_instr_ret());
		vmp_func_begin_end(add);

		compile();

		push_value<T>(value);
		invoke("Neg");

		verify_stack_size(sizeof(T) * 2);
		verify_value(pop_value<T>(), (T)(-value));
		verify_value(pop_value<T>(), (T)(value));
	}

	void operator()()
	{
		TEST(neg_T<vm_int8>(1));
		TEST(neg_T<vm_uint8>(15));
		TEST(neg_T<vm_int16>(10));
		TEST(neg_T<vm_uint16>(INT8_MAX));
		TEST(neg_T<vm_int32>(INT16_MAX));
		TEST(neg_T<vm_uint32>((vm_uint32)INT16_MAX + 10));
		TEST(neg_T<vm_int64>((vm_int64)((vm_int64)INT32_MAX + 5i64)));
		TEST(neg_T<vm_uint64>((vm_uint64)(INT32_MAX + 1000i64)));
		TEST(neg_T<vm_float32>(1.0f));
		TEST(neg_T<vm_float64>(-10.0));
	}
};

struct suite_vmp_consts : utils_vmp_vmi
{
	template<typename T>
	void ldc_T(T value)
	{
		// Create the main package
		auto main_package = vmp_package_newsz("main", 4);
		vmp_pipeline_add_package(pipeline, main_package);

		auto primitive_type = get_type<T>();

		// Create the Get function that returns a constant value
		auto add = vmp_func_newsz("Get", 3);
		auto ret1 = vmp_func_new_return(add, primitive_type);
		vmp_package_add_func(main_package, add);

		// {
		//	ldc_<T> $value
		//	ret
		// }
		vmp_func_begin_body(add);
		vmp_func_add_instr(add, vmp_instr_ldc(primitive_type, to_const((T)value)));
		vmp_func_add_instr(add, vmp_instr_ret());
		vmp_func_begin_end(add);

		compile();

		invoke("Get");

		verify_stack_size(sizeof(T));
		verify_value(pop_value<T>(), (T)value);
	}

	template<typename LHS, typename RHS>
	void vmp_const_add_T(LHS lhs, RHS rhs)
	{
		vmp_const lhs_const = to_const(lhs);
		vmp_const rhs_const = to_const(rhs);
		vmp_const_add(&lhs_const, &rhs_const);

		auto expected = lhs + rhs;

		if (std::is_same<decltype(expected), vm_int8>::value) {
			assert_equals(lhs_const.i1, (vm_int8)expected);
		}
		else if (std::is_same<decltype(expected), vm_uint8>::value) {
			assert_equals(lhs_const.ui1, (vm_uint8)expected);
		}
		else if (std::is_same<decltype(expected), vm_int16>::value) {
			assert_equals(lhs_const.i2, (vm_int16)expected);
		}
		else if (std::is_same<decltype(expected), vm_uint16>::value) {
			assert_equals(lhs_const.ui2, (vm_uint16)expected);
		}
		else if (std::is_same<decltype(expected), vm_int32>::value) {
			assert_equals(lhs_const.i4, (vm_int32)expected);
		}
		else if (std::is_same<decltype(expected), vm_uint32>::value) {
			assert_equals(lhs_const.ui4, (vm_uint32)expected);
		}
		else if (std::is_same<decltype(expected), vm_int64>::value) {
			assert_equals(lhs_const.i8, (vm_int64)expected);
		}
		else if (std::is_same<decltype(expected), vm_uint64>::value) {
			assert_equals(lhs_const.ui8, (vm_uint64)expected);
		}
		else if (std::is_same<decltype(expected), vm_float32>::value) {
			assert_equals(lhs_const.f4, (vm_float32)expected);
		}
		else if (std::is_same<decltype(expected), vm_float64>::value) {
			assert_equals(lhs_const.f8, (vm_float64)expected);
		}

		assert_equals(lhs_const.type, (vm_int32)props1_of(expected));
	}

	template<typename LHS, typename RHS>
	void vmp_const_sub_T(LHS lhs, RHS rhs)
	{
		vmp_const lhs_const = to_const(lhs);
		vmp_const rhs_const = to_const(rhs);
		vmp_const_sub(&lhs_const, &rhs_const);

		auto expected = lhs - rhs;

		if (std::is_same<decltype(expected), vm_int8>::value) {
			assert_equals(lhs_const.i1, (vm_int8)expected);
		}
		else if (std::is_same<decltype(expected), vm_uint8>::value) {
			assert_equals(lhs_const.ui1, (vm_uint8)expected);
		}
		else if (std::is_same<decltype(expected), vm_int16>::value) {
			assert_equals(lhs_const.i2, (vm_int16)expected);
		}
		else if (std::is_same<decltype(expected), vm_uint16>::value) {
			assert_equals(lhs_const.ui2, (vm_uint16)expected);
		}
		else if (std::is_same<decltype(expected), vm_int32>::value) {
			assert_equals(lhs_const.i4, (vm_int32)expected);
		}
		else if (std::is_same<decltype(expected), vm_uint32>::value) {
			assert_equals(lhs_const.ui4, (vm_uint32)expected);
		}
		else if (std::is_same<decltype(expected), vm_int64>::value) {
			assert_equals(lhs_const.i8, (vm_int64)expected);
		}
		else if (std::is_same<decltype(expected), vm_uint64>::value) {
			assert_equals(lhs_const.ui8, (vm_uint64)expected);
		}
		else if (std::is_same<decltype(expected), vm_float32>::value) {
			assert_equals(lhs_const.f4, (vm_float32)expected);
		}
		else if (std::is_same<decltype(expected), vm_float64>::value) {
			assert_equals(lhs_const.f8, (vm_float64)expected);
		}

		assert_equals(lhs_const.type, (vm_int32)props1_of(expected));
	}

	void operator()()
	{
		TEST(ldc_T<vm_int8>(12));
		TEST(ldc_T<vm_uint8>(UINT8_MAX - 10));
		TEST(ldc_T<vm_int16>(INT16_MAX - 10));
		TEST(ldc_T<vm_uint16>(UINT16_MAX - 10));
		TEST(ldc_T<vm_int32>(INT32_MAX - 1234));
		TEST(ldc_T<vm_uint32>(UINT32_MAX - 1234));
		TEST(ldc_T<vm_int64>(INT64_MAX - 123456));
		TEST(ldc_T<vm_uint64>(UINT64_MAX - 123456));
		TEST(ldc_T<vm_float32>(123.456f));
		TEST(ldc_T<vm_float64>(12345.6789));

		TEST(vmp_const_add_T((vm_int8)-123, (vm_int8)54));
		TEST(vmp_const_add_T((vm_int8)-123, (vm_uint8)244));
		TEST(vmp_const_add_T((vm_int8)-123, (vm_int16)1234));
		TEST(vmp_const_add_T((vm_int8)-123, (vm_uint16)30000));
		TEST(vmp_const_add_T((vm_int8)-123, (vm_int32)34768));
		TEST(vmp_const_add_T((vm_int8)-123, (vm_uint32)INT32_MAX + 100u));
		TEST(vmp_const_add_T((vm_int8)-123, (vm_int64)UINT32_MAX + 100ll));
		TEST(vmp_const_add_T((vm_int8)-123, (vm_uint64)INT64_MAX + 100ull));
		TEST(vmp_const_add_T((vm_int8)-123, 123.12f));
		TEST(vmp_const_add_T((vm_int8)-123, 123.12));

		TEST(vmp_const_add_T((vm_uint8)129, (vm_int8)-12));
		TEST(vmp_const_add_T((vm_uint8)129, (vm_uint8)244));
		TEST(vmp_const_add_T((vm_uint8)129, (vm_int16)1234));
		TEST(vmp_const_add_T((vm_uint8)129, (vm_uint16)30000));
		TEST(vmp_const_add_T((vm_uint8)129, (vm_int32)34768));
		TEST(vmp_const_add_T((vm_uint8)129, (vm_uint32)INT32_MAX + 100u));
		TEST(vmp_const_add_T((vm_uint8)129, (vm_int64)UINT32_MAX + 100ll));
		TEST(vmp_const_add_T((vm_uint8)129, (vm_uint64)INT64_MAX + 100ull));
		TEST(vmp_const_add_T((vm_uint8)129, 123.12f));
		TEST(vmp_const_add_T((vm_int8)129, 123.12));

		TEST(vmp_const_sub_T((vm_uint8)129, (vm_int8)-12));
		TEST(vmp_const_sub_T((vm_uint8)129, (vm_uint8)244));
		TEST(vmp_const_sub_T((vm_uint8)129, (vm_int16)1234));
		TEST(vmp_const_sub_T((vm_uint8)129, (vm_uint16)30000));
		TEST(vmp_const_sub_T((vm_uint8)129, (vm_int32)34768));
		TEST(vmp_const_sub_T((vm_uint8)129, (vm_uint32)INT32_MAX + 100u));
		TEST(vmp_const_sub_T((vm_uint8)129, (vm_int64)UINT32_MAX + 100ll));
		TEST(vmp_const_sub_T((vm_uint8)129, (vm_uint64)INT64_MAX + 100ull));
		TEST(vmp_const_sub_T((vm_uint8)129, 123.12f));
		TEST(vmp_const_sub_T((vm_int8)129, 123.12));

		TEST(vmp_const_add_T((vm_int32)1234, (vm_int32)5678));
	}
};

struct suite_vmp_locals : utils_vmp_vmi
{
	void locals()
	{
		// Create the main package
		auto main_package = vmp_package_newsz("main", 4);
		vmp_pipeline_add_package(pipeline, main_package);

		// Create the Get function and add two integer types
		auto get = vmp_func_newsz("Get", 3);
		auto get_ret1 = vmp_func_new_return(get, get_type<vm_int32>());
		auto get_local1 = vmp_func_new_local(get, get_type<vm_int32>());
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
		//	retvmp_instr_stl
		// }
		vmp_func_begin_body(get);
		vmp_func_add_instr(get, vmp_instr_ldc(get_type<vm_int32>(), to_const((vm_int32)100)));
		vmp_func_add_instr(get, vmp_instr_stl(get_local1));
		vmp_func_add_instr(get, vmp_instr_ldl(get_local1));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type<vm_int32>(), to_const((vm_int32)1)));
		vmp_func_add_instr(get, vmp_instr_add(get_type<vm_int32>()));
		vmp_func_add_instr(get, vmp_instr_stl(get_local1));
		vmp_func_add_instr(get, vmp_instr_ldl(get_local1));
		vmp_func_add_instr(get, vmp_instr_ret());
		vmp_func_begin_end(get);

		compile();

		invoke("Get");

		verify_stack_size(sizeof(vm_int32));
		verify_value(pop_value<vm_int32>(), 100 + 1);
	}

	template<typename T>
	void ldl_a_T(T value)
	{
		// Create the main package
		auto main_package = vmp_package_newsz("main", 4);
		vmp_pipeline_add_package(pipeline, main_package);

		// Create the Get function and add two integer types
		auto inner_get = vmp_func_newsz("innerGet", 8);
		auto inner_get_arg1 = vmp_func_new_arg(inner_get, get_type<T*>());
		vmp_package_add_func(main_package, inner_get);

		// {
		//	lda 0
		//	ldc_T 100
		//	stunref int32
		//	ret
		// }
		vmp_func_begin_body(inner_get);
		vmp_func_add_instr(inner_get, vmp_instr_lda(inner_get_arg1));
		vmp_func_add_instr(inner_get, vmp_instr_ldc(get_type<T>(), to_const((T)value)));
		vmp_func_add_instr(inner_get, vmp_instr_sturef(get_type<T>()));
		vmp_func_add_instr(inner_get, vmp_instr_ret());
		vmp_func_begin_end(inner_get);

		// Create the Get function and add two integer types
		auto get = vmp_func_newsz("Get", 3);
		auto get_ret1 = vmp_func_new_return(get, get_type<T>());
		auto get_local1 = vmp_func_new_local(get, get_type<T>());
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
		vmp_func_add_instr(get, vmp_instr_ldl_a(get_local1));
		vmp_func_add_instr(get, vmp_instr_call(inner_get));
		vmp_func_add_instr(get, vmp_instr_frees(get_type<T*>()));
		vmp_func_add_instr(get, vmp_instr_ldl(get_local1));
		vmp_func_add_instr(get, vmp_instr_ret());
		vmp_func_begin_end(get);

		compile();

		invoke("Get");

		verify_stack_size(sizeof(T));
		verify_value(pop_value<T>(), (T)value);
	}

	void operator()()
	{
		TEST(locals());

		TEST(ldl_a_T<vm_int8>((vm_int8)-120));
		TEST(ldl_a_T<vm_int16>(INT16_MAX - 100));
		TEST(ldl_a_T<vm_int32>((vm_int32)(INT32_MAX * 0.98)));
		TEST(ldl_a_T<vm_int64>(INT64_MAX - INT16_MAX));
		TEST(ldl_a_T<vm_float32>(123.45f));
		TEST(ldl_a_T<vm_float64>(1234.4567));
	}
};

struct suite_vmp_globals : utils_vmp_vmi
{
	void globals()
	{
		// Create the main package
		auto main_package = vmp_package_newsz("main", 4);
		vmp_pipeline_add_package(pipeline, main_package);
		auto global = vmp_package_new_global(main_package, get_type<vm_int32>());

		// Create the Set function that sets the global variable
		auto set = vmp_func_newsz("Set", 3);
		auto set_arg1 = vmp_func_new_arg(set, get_type<vm_int32>());
		vmp_package_add_func(main_package, set);

		// {
		//	lda 0
		//	stg <var>
		//	ret
		// }
		vmp_func_begin_body(set);
		vmp_func_add_instr(set, vmp_instr_lda(set_arg1));
		vmp_func_add_instr(set, vmp_instr_stg(global));
		vmp_func_add_instr(set, vmp_instr_ret());
		vmp_func_begin_end(set);

		// Create the Get function that returns the global variable
		auto get = vmp_func_newsz("Get", 3);
		vmp_func_new_return(get, get_type<vm_int32>());
		vmp_package_add_func(main_package, get);

		// {
		//	ldg <var>
		//	ret
		// }
		vmp_func_begin_body(get);
		vmp_func_add_instr(get, vmp_instr_ldg(global));
		vmp_func_add_instr(get, vmp_instr_ret());
		vmp_func_begin_end(get);

		compile();

		push_value<vm_int32>(123);
		invoke("Set");

		verify_stack_size(sizeof(vm_int32));
		verify_value(pop_value<vm_int32>(), 123);

		invoke("Get");
		verify_stack_size(sizeof(vm_int32));
		verify_value(pop_value<vm_int32>(), 123);
	}

	void operator()()
	{
		TEST(globals());
	}
};

struct suite_vmp_convertions : utils_vmp_vmi
{
	template<typename FROM, typename TO>
	void conv_T_T(const FROM from, const TO to)
	{
		// Create the main package
		auto main_package = vmp_package_newsz("main", 4);
		vmp_pipeline_add_package(pipeline, main_package);

		// Create the Get function and add two integer types
		auto get = vmp_func_newsz("Get", 3);
		auto get_arg1 = vmp_func_new_arg(get, get_type<FROM>());
		vmp_func_new_return(get, get_type<TO>());
		vmp_package_add_func(main_package, get);

		// {
		//	lda 0
		//  conv_%FROM_%TO
		//	ret
		// }
		vmp_func_begin_body(get);
		vmp_func_add_instr(get, vmp_instr_lda(get_arg1));
		vmp_func_add_instr(get, vmp_instr_conv(get_type<FROM>(), get_type<TO>()));
		vmp_func_add_instr(get, vmp_instr_ret());
		vmp_func_begin_end(get);

		compile();

		push_value<FROM>(from);
		invoke("Get");

		verify_stack_size(sizeof(FROM) + sizeof(TO));
		verify_value(pop_value<TO>(), (TO)to);
		verify_value(pop_value<FROM>(), (FROM)from);
	}

	void operator()()
	{
		TEST(conv_T_T((vm_int32)INT32_MAX, (vm_int16)INT32_MAX));
		TEST(conv_T_T((vm_int16)INT16_MAX, (vm_int32)INT16_MAX));
	}
};

struct suite_vmp_add_sub_mult_div : utils_vmp_vmi
{
	template<typename T>
	void add_test(T lhs, T rhs)
	{
		// Create the main package
		auto main_package = vmp_package_newsz("main", 4);
		vmp_pipeline_add_package(pipeline, main_package);

		// Create the Add function and add two integer types
		auto add = vmp_func_newsz("Add", 3);
		auto arg1 = vmp_func_new_arg(add, get_type<T>());
		vmp_arg_set_namesz(arg1, "lhs", 3);
		auto arg2 = vmp_func_new_arg(add, get_type<T>());
		vmp_arg_set_namesz(arg2, "rhs", 3);
		auto ret1 = vmp_func_new_return(add, get_type<T>());
		vmp_package_add_func(main_package, add);

		// {
		//	lda 0
		//	lda 1
		//	add T
		//	lda 1
		//	add T
		//	ret
		// }
		vmp_func_begin_body(add);
		vmp_func_add_instr(add, vmp_instr_lda(arg1));
		vmp_func_add_instr(add, vmp_instr_lda(arg2));
		vmp_func_add_instr(add, vmp_instr_add(get_type<T>()));
		vmp_func_add_instr(add, vmp_instr_lda(arg2));
		vmp_func_add_instr(add, vmp_instr_add(get_type<T>()));
		vmp_func_add_instr(add, vmp_instr_ret());
		vmp_func_begin_end(add);

		compile();

		push_value<T>(lhs);
		push_value<T>(rhs);
		invoke("Add");

		verify_stack_size(sizeof(T) * 3);
		verify_value(pop_value<T>(), (T)(lhs + rhs + rhs));
		verify_value(pop_value<T>(), (T)(rhs));
		verify_value(pop_value<T>(), (T)(lhs));
	}

	template<typename T>
	void sub_T(T lhs, T rhs)
	{
		// Create the main package
		auto main_package = vmp_package_newsz("main", 4);
		vmp_pipeline_add_package(pipeline, main_package);

		// Create the Add function and add two integer types
		auto add = vmp_func_newsz("Add", 3);
		auto arg1 = vmp_func_new_arg(add, get_type<T>());
		vmp_arg_set_namesz(arg1, "lhs", 3);
		auto arg2 = vmp_func_new_arg(add, get_type<T>());
		vmp_arg_set_namesz(arg2, "rhs", 3);
		auto ret1 = vmp_func_new_return(add, get_type<T>());
		vmp_package_add_func(main_package, add);

		// {
		//	lda 0
		//	lda 1
		//	sub T
		//	lda 1
		//	sub T
		//	ret
		// }
		vmp_func_begin_body(add);
		vmp_func_add_instr(add, vmp_instr_lda(arg1));
		vmp_func_add_instr(add, vmp_instr_lda(arg2));
		vmp_func_add_instr(add, vmp_instr_sub(get_type<T>()));
		vmp_func_add_instr(add, vmp_instr_lda(arg2));
		vmp_func_add_instr(add, vmp_instr_sub(get_type<T>()));
		vmp_func_add_instr(add, vmp_instr_ret());
		vmp_func_begin_end(add);

		compile();

		push_value<T>(lhs);
		push_value<T>(rhs);
		invoke("Add");

		verify_stack_size(sizeof(T) * 3);
		verify_value(pop_value<T>(), (T)(lhs - rhs - rhs));
		verify_value(pop_value<T>(), (T)(rhs));
		verify_value(pop_value<T>(), (T)(lhs));
	}

	template<typename T>
	void mul_T(T lhs, T rhs)
	{
		// Create the main package
		auto main_package = vmp_package_newsz("main", 4);
		vmp_pipeline_add_package(pipeline, main_package);

		// Create the Add function and add two integer types
		auto func = vmp_func_newsz("Mul", 3);
		auto arg1 = vmp_func_new_arg(func, get_type<T>());
		vmp_arg_set_namesz(arg1, "lhs", 3);
		auto arg2 = vmp_func_new_arg(func, get_type<T>());
		vmp_arg_set_namesz(arg2, "rhs", 3);
		auto ret1 = vmp_func_new_return(func, get_type<T>());
		vmp_package_add_func(main_package, func);

		// {
		//	lda 0
		//	lda 1
		//	mul T
		//	ret
		// }
		vmp_func_begin_body(func);
		vmp_func_add_instr(func, vmp_instr_lda(arg1));
		vmp_func_add_instr(func, vmp_instr_lda(arg2));
		vmp_func_add_instr(func, vmp_instr_mul(get_type<T>()));
		vmp_func_add_instr(func, vmp_instr_ret());
		vmp_func_begin_end(func);

		compile();

		push_value<T>(lhs);
		push_value<T>(rhs);
		invoke("Mul");

		verify_stack_size(sizeof(T) * 3);
		verify_value(pop_value<T>(), (T)(lhs * rhs));
		verify_value(pop_value<T>(), (T)(rhs));
		verify_value(pop_value<T>(), (T)(lhs));
	}

	template<typename T>
	void div_T(T lhs, T rhs)
	{
		// Create the main package
		auto main_package = vmp_package_newsz("main", 4);
		vmp_pipeline_add_package(pipeline, main_package);

		// Create the Add function and add two integer types
		auto func = vmp_func_newsz("Div", 3);
		auto arg1 = vmp_func_new_arg(func, get_type<T>());
		vmp_arg_set_namesz(arg1, "lhs", 3);
		auto arg2 = vmp_func_new_arg(func, get_type<T>());
		vmp_arg_set_namesz(arg2, "rhs", 3);
		auto ret1 = vmp_func_new_return(func, get_type<T>());
		vmp_package_add_func(main_package, func);

		// {
		//	lda 0
		//	lda 1
		//	mul T
		//	ret
		// }
		vmp_func_begin_body(func);
		vmp_func_add_instr(func, vmp_instr_lda(arg1));
		vmp_func_add_instr(func, vmp_instr_lda(arg2));
		vmp_func_add_instr(func, vmp_instr_div(get_type<T>()));
		vmp_func_add_instr(func, vmp_instr_ret());
		vmp_func_begin_end(func);

		compile();

		push_value<T>(lhs);
		push_value<T>(rhs);
		invoke("Div");

		verify_stack_size(sizeof(T) * 3);
		verify_value(pop_value<T>(), (T)(lhs / rhs));
		verify_value(pop_value<T>(), (T)(rhs));
		verify_value(pop_value<T>(), (T)(lhs));
	}

	void operator()()
	{
		TEST(add_test<vm_int8>(1, 5));
		TEST(add_test<vm_uint8>(1, 5));
		TEST(add_test<vm_int16>(10, INT8_MAX));
		TEST(add_test<vm_uint16>(10, INT8_MAX));
		TEST(add_test<vm_int32>(INT16_MAX, 20));
		TEST(add_test<vm_uint32>(INT16_MAX, INT16_MAX));
		TEST(add_test<vm_int64>(102, INT32_MAX));
		TEST(add_test<vm_uint64>(INT32_MAX, INT32_MAX));
		TEST(add_test<vm_float32>(1.0f, 20.0f));
		TEST(add_test<vm_float64>(-10.0, 203.0));

		TEST(sub_T<vm_int8>(1, 5));
		TEST(sub_T<vm_uint8>(1, 5));
		TEST(sub_T<vm_int16>(10, INT8_MAX));
		TEST(sub_T<vm_uint16>(10, INT8_MAX));
		TEST(sub_T<vm_int32>(INT16_MAX, 20));
		TEST(sub_T<vm_uint32>(INT16_MAX, INT16_MAX));
		TEST(sub_T<vm_int64>(102, INT32_MAX));
		TEST(sub_T<vm_uint64>(INT32_MAX, INT32_MAX));
		TEST(sub_T<vm_float32>(1.0f, 20.0f));
		TEST(sub_T<vm_float64>(-10.0, 203.0));

		TEST(mul_T<vm_int8>(2, 5));
		TEST(mul_T<vm_uint8>(3, 5));
		TEST(mul_T<vm_int16>(INT8_MAX, 10));
		TEST(mul_T<vm_uint16>(INT16_MAX, 2));
		TEST(mul_T<vm_int32>(INT16_MAX, 3));
		TEST(mul_T<vm_uint32>(INT16_MAX, 4));
		TEST(mul_T<vm_int64>(INT32_MAX, 2));
		TEST(mul_T<vm_uint64>(INT32_MAX, 4));
		TEST(mul_T<vm_float32>(1.0f, 20.0f));
		TEST(mul_T<vm_float64>(-10.0, 203.0));

		TEST(div_T<vm_int8>(2, 5));
		TEST(div_T<vm_uint8>(3, 5));
		TEST(div_T<vm_int16>(INT8_MAX, 10));
		TEST(div_T<vm_uint16>(INT16_MAX, 2));
		TEST(div_T<vm_int32>(INT16_MAX, 3));
		TEST(div_T<vm_uint32>(INT16_MAX, 4));
		TEST(div_T<vm_int64>(INT32_MAX, 2));
		TEST(div_T<vm_uint64>(INT32_MAX, 4));
		TEST(div_T<vm_float32>(1.0f, 20.0f));
		TEST(div_T<vm_float64>(-10.0, 203.0));
	}
};

struct suite_vmp_bits : utils_vmp_vmi
{
	template<typename T>
	void bit_not_T(T value)
	{
		// Create the main package
		auto main_package = vmp_package_newsz("main", 4);
		vmp_pipeline_add_package(pipeline, main_package);

		// Create the Add function and add two integer types
		auto func = vmp_func_newsz("BitNot", 6);
		auto arg1 = vmp_func_new_arg(func, get_type<T>());
		vmp_arg_set_namesz(arg1, "lhs", 3);
		auto ret1 = vmp_func_new_return(func, get_type<T>());
		vmp_package_add_func(main_package, func);

		// {
		//	lda 0
		//	bit not T
		//	ret
		// }
		vmp_func_begin_body(func);
		vmp_func_add_instr(func, vmp_instr_lda(arg1));
		vmp_func_add_instr(func, vmp_instr_bit_not(get_type<T>()));
		vmp_func_add_instr(func, vmp_instr_ret());
		vmp_func_begin_end(func);

		compile();

		push_value<T>(value);
		invoke("BitNot");

		verify_stack_size(sizeof(T) * 2);
		verify_value(pop_value<T>(), (T)(~value));
		verify_value(pop_value<T>(), (T)(value));
	}

	void operator()()
	{
		TEST(bit_not_T<vm_int8>(1));
		TEST(bit_not_T<vm_uint8>(15));
		TEST(bit_not_T<vm_int16>(10));
		TEST(bit_not_T<vm_uint16>(INT8_MAX));
		TEST(bit_not_T<vm_int32>(INT16_MAX));
		TEST(bit_not_T<vm_uint32>((vm_uint32)INT16_MAX + 10));
		TEST(bit_not_T<vm_int64>((vm_int64)((vm_int64)INT32_MAX + 5i64)));
		TEST(bit_not_T<vm_uint64>((vm_uint64)(INT32_MAX + 1000i64)));
	}
};

struct suite_vmp_memory : utils_vmp_vmi
{
	void allocs_const()
	{
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

		invoke("Do");

		verify_stack_size(0);
	}

	template<typename T>
	void allocs_T()
	{
		// Create the main package
		auto main_package = vmp_package_newsz("main", 4);
		vmp_pipeline_add_package(pipeline, main_package);

		auto type = get_type<T>();

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

		invoke("Do");

		verify_stack_size(0);
	}

	void alloch_const()
	{
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

		invoke("Do");

		verify_stack_size(0);
	}

	template<typename T>
	void alloch_T()
	{
		// Create the main package
		auto main_package = vmp_package_newsz("main", 4);
		vmp_pipeline_add_package(pipeline, main_package);

		auto type = get_type<T>();

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
		vmp_func_add_instr(_do, vmp_instr_ldc(get_type<vm_int32>(), to_const((vm_int32)sizeof(T))));
		vmp_func_add_instr(_do, vmp_instr_alloch());
		vmp_func_add_instr(_do, vmp_instr_freeh());
		vmp_func_add_instr(_do, vmp_instr_ret());
		vmp_func_begin_end(_do);

		compile();

		invoke("Do");

		verify_stack_size(0);
	}

	template<typename T>
	void alloch_type_T()
	{
		// Create the main package
		auto main_package = vmp_package_newsz("main", 4);
		vmp_pipeline_add_package(pipeline, main_package);

		auto type = get_type<T>();

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

		invoke("Do");

		verify_stack_size(0);
	}

	void operator()()
	{
		TEST(allocs_const());

		TEST(allocs_T<vm_int8>());
		TEST(allocs_T<vm_uint8>());
		TEST(allocs_T<vm_int16>());
		TEST(allocs_T<vm_uint16>());
		TEST(allocs_T<vm_int32>());
		TEST(allocs_T<vm_uint32>());
		TEST(allocs_T<vm_int64>());
		TEST(allocs_T<vm_uint64>());
		TEST(allocs_T<vm_float32>());
		TEST(allocs_T<vm_float64>());

		TEST(alloch_const());

		TEST(alloch_T<vm_int8>());
		TEST(alloch_T<vm_uint8>());
		TEST(alloch_T<vm_int16>());
		TEST(alloch_T<vm_uint16>());
		TEST(alloch_T<vm_int32>());
		TEST(alloch_T<vm_uint32>());
		TEST(alloch_T<vm_int64>());
		TEST(alloch_T<vm_uint64>());
		TEST(alloch_T<vm_float32>());
		TEST(alloch_T<vm_float64>());

		TEST(alloch_type_T<vm_int8>());
		TEST(alloch_type_T<vm_uint8>());
		TEST(alloch_type_T<vm_int16>());
		TEST(alloch_type_T<vm_uint16>());
		TEST(alloch_type_T<vm_int32>());
		TEST(alloch_type_T<vm_uint32>());
		TEST(alloch_type_T<vm_int64>());
		TEST(alloch_type_T<vm_uint64>());
		TEST(alloch_type_T<vm_float32>());
		TEST(alloch_type_T<vm_float64>());
	}
};

struct suite_vmp_element : utils_vmp_vmi
{
	template<typename T>
	void stelem_T(const array<T, 2>& values)
	{
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
				get_type<T>()
			)
		);
		vmp_package_add_type(vmp_pipeline_find_packagesz(pipeline, "vm", 2), array_type);

		// Create the Get function and add two integer types
		auto get = vmp_func_newsz("Get", 3);
		vmp_func_new_return(get, array_type);
		auto get_local_1 = vmp_func_new_local(get, array_type);
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
		vmp_func_add_instr(get, vmp_instr_ldl_a(get_local_1));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type<vm_int32>(), to_const(0)));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type<T>(), to_const((T)values[0])));
		vmp_func_add_instr(get, vmp_instr_stelem(array_type));
		vmp_func_add_instr(get, vmp_instr_ldl_a(get_local_1));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type<vm_int32>(), to_const(1)));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type<T>(), to_const((T)values[1])));
		vmp_func_add_instr(get, vmp_instr_stelem(array_type));
		vmp_func_add_instr(get, vmp_instr_ldl(get_local_1));
		vmp_func_add_instr(get, vmp_instr_ret());
		vmp_func_begin_end(get);

		compile();

		invoke("Get");

		verify_stack_size(sizeof(T[2]));
		auto ret = pop_value<T, 2>();
		verify_value(ret[0], values[0]);
		verify_value(ret[1], values[1]);
	}

	template<typename T>
	void stelem_ptr_T(const array<T, 2>& values)
	{
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
		vmp_func_add_instr(get, vmp_instr_lda(get_arg1));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type<vm_int32>(), to_const(0)));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type<T>(), to_const((T)values[0])));
		vmp_func_add_instr(get, vmp_instr_stelem(ptr_type));
		vmp_func_add_instr(get, vmp_instr_lda(get_arg1));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type<vm_int32>(), to_const(1)));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type<T>(), to_const((T)values[1])));
		vmp_func_add_instr(get, vmp_instr_stelem(ptr_type));
		vmp_func_add_instr(get, vmp_instr_ret());
		vmp_func_begin_end(get);

		compile();

		push_value<T*>(new T[values.size()]);
		invoke("Get");

		verify_stack_size(sizeof(T*));
		auto ret_as_arg = pop_value<T*>();
		verify_value(ret_as_arg[0], values[0]);
		verify_value(ret_as_arg[1], values[1]);
		delete[] ret_as_arg;
	}

	template<typename T>
	void ldelem_T(const array<T, 2>& values)
	{
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
				get_type<T>()
			)
		);
		vmp_package_add_type(vmp_pipeline_find_packagesz(pipeline, "vm", 2), array_type);

		// Create the Get function and add two integer types
		auto get = vmp_func_newsz("Get", 3);
		auto get_arg1 = vmp_func_new_arg(get, array_type);
		auto get_ret1 = vmp_func_new_return(get, get_type<T>());
		auto get_ret2 = vmp_func_new_return(get, get_type<T>());
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
		vmp_func_add_instr(get, vmp_instr_lda_a(get_arg1));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type<vm_int32>(), to_const(0)));
		vmp_func_add_instr(get, vmp_instr_ldelem(array_type));
		vmp_func_add_instr(get, vmp_instr_lda_a(get_arg1));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type<vm_int32>(), to_const(1)));
		vmp_func_add_instr(get, vmp_instr_ldelem(array_type));
		vmp_func_add_instr(get, vmp_instr_ret());
		vmp_func_begin_end(get);

		compile();

		T values_as_array[2] = { values[0], values[1] };
		push_value<T, 2>(values_as_array);
		invoke("Get");

		verify_stack_size(sizeof(T) * 4);
		verify_value(pop_value<T>(), values[1]);
		verify_value(pop_value<T>(), values[0]);
		auto popped_in = pop_value<T, 2>();
		verify_value(popped_in[1], values[1]);
		verify_value(popped_in[0], values[0]);
	}

	template<typename T>
	void ldelem_ptr_T(const vector<T>& values)
	{
		// Create the main package
		auto main_package = vmp_package_newsz("main", 4);
		vmp_pipeline_add_package(pipeline, main_package);

		// Create array type
		auto ptr_type = get_type("vm", string(ptr<T>()));

		// Create the Get function and add two integer types
		auto get = vmp_func_newsz("Get", 3);
		auto get_arg1 = vmp_func_new_arg(get, ptr_type);
		auto get_ret1 = vmp_func_new_return(get, get_type<T>());
		auto get_ret2 = vmp_func_new_return(get, get_type<T>());
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
		vmp_func_add_instr(get, vmp_instr_lda(get_arg1));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type<vm_int32>(), to_const(0)));
		vmp_func_add_instr(get, vmp_instr_ldelem(ptr_type));
		vmp_func_add_instr(get, vmp_instr_lda(get_arg1));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type<vm_int32>(), to_const(1)));
		vmp_func_add_instr(get, vmp_instr_ldelem(ptr_type));
		vmp_func_add_instr(get, vmp_instr_ret());
		vmp_func_begin_end(get);

		compile();

		push_value<T*>((T*)values.data());
		invoke("Get");

		verify_stack_size(sizeof(T*) + sizeof(T) * 2);
		verify_value(pop_value<T>(), values[1]);
		verify_value(pop_value<T>(), values[0]);
		T* popped_in = pop_value<T*>();
		verify_value(popped_in[1], values[1]);
		verify_value(popped_in[0], values[0]);
	}

	void operator()()
	{
		TEST(stelem_T<vm_int8>({ 1, -1 }));
		TEST(stelem_T<vm_uint8>({ INT8_MAX + 1, UINT8_MAX - 1 }));
		TEST(stelem_T<vm_int16>({ UINT8_MAX + 1, INT16_MAX - 100 }));
		TEST(stelem_T<vm_uint16>({ INT16_MAX + 1, UINT16_MAX - 1 }));
		TEST(stelem_T<vm_int32>({ UINT16_MAX + 1, INT32_MAX - 100 }));
		TEST(stelem_T<vm_uint32>({ INT32_MAX + 1u, UINT32_MAX - 100u }));
		TEST(stelem_T<vm_int64>({ UINT32_MAX + 1ll, INT64_MAX - 100i64 }));
		TEST(stelem_T<vm_uint64>({ (vm_uint64)INT64_MAX + 1i64, (vm_uint64)UINT64_MAX - 100i64 }));
		TEST(stelem_T<vm_float32>({ 123.43f, 657.123f }));
		TEST(stelem_T<vm_float64>({ 123.4345, 657.12312 }));

		TEST(stelem_ptr_T<vm_int8>({ 1, -1 }));
		TEST(stelem_ptr_T<vm_uint8>({ INT8_MAX + 1, UINT8_MAX - 1 }));
		TEST(stelem_ptr_T<vm_int16>({ UINT8_MAX + 1, INT16_MAX - 100 }));
		TEST(stelem_ptr_T<vm_uint16>({ INT16_MAX + 1, UINT16_MAX - 1 }));
		TEST(stelem_ptr_T<vm_int32>({ UINT16_MAX + 1, INT32_MAX - 100 }));
		TEST(stelem_ptr_T<vm_uint32>({ INT32_MAX + 1u, UINT32_MAX - 100u }));
		TEST(stelem_ptr_T<vm_int64>({ UINT32_MAX + 1ll, INT64_MAX - 100ll }));
		TEST(stelem_ptr_T<vm_uint64>({ (vm_uint64)INT64_MAX + 1i64, (vm_uint64)UINT64_MAX - 100i64 }));
		TEST(stelem_ptr_T<vm_float32>({ 123.43f, 657.123f }));
		TEST(stelem_ptr_T<vm_float64>({ 123.4345, 657.12312 }));

		TEST(ldelem_T<vm_int8>({ 1, -1 }));
		TEST(ldelem_T<vm_uint8>({ INT8_MAX + 1, UINT8_MAX - 1 }));
		TEST(ldelem_T<vm_int16>({ UINT8_MAX + 1, INT16_MAX - 100 }));
		TEST(ldelem_T<vm_uint16>({ INT16_MAX + 1, UINT16_MAX - 1 }));
		TEST(ldelem_T<vm_int32>({ UINT16_MAX + 1, INT32_MAX - 100 }));
		TEST(ldelem_T<vm_uint32>({ INT32_MAX + 1u, UINT32_MAX - 100u }));
		TEST(ldelem_T<vm_int64>({ UINT32_MAX + 1ll, INT64_MAX - 100ll }));
		TEST(ldelem_T<vm_uint64>({ (vm_uint64)INT64_MAX + (vm_uint64)1, (vm_uint64)UINT64_MAX - (vm_uint64)100 }));
		TEST(ldelem_T<vm_float32>({ 123.43f, 657.123f }));
		TEST(ldelem_T<vm_float64>({ 123.4345, 657.12312 }));

		TEST(ldelem_ptr_T<vm_int8>({ 1, -1 }));
		TEST(ldelem_ptr_T<vm_uint8>({ INT8_MAX + 1, UINT8_MAX - 1 }));
		TEST(ldelem_ptr_T<vm_int16>({ UINT8_MAX + 1, INT16_MAX - 100 }));
		TEST(ldelem_ptr_T<vm_uint16>({ INT16_MAX + 1, UINT16_MAX - 1 }));
		TEST(ldelem_ptr_T<vm_int32>({ UINT16_MAX + 1, INT32_MAX - 100 }));
		TEST(ldelem_ptr_T<vm_uint32>({ INT32_MAX + 1u, UINT32_MAX - 100u }));
		TEST(ldelem_ptr_T<vm_int64>({ UINT32_MAX + 1ll, INT64_MAX - 100ll }));
		TEST(ldelem_ptr_T<vm_uint64>({ (vm_uint64)INT64_MAX + (vm_uint64)1, (vm_uint64)UINT64_MAX - (vm_uint64)100 }));
		TEST(ldelem_ptr_T<vm_float32>({ 123.43f, 657.123f }));
		TEST(ldelem_ptr_T<vm_float64>({ 123.4345, 657.12312 }));
	}
};

struct suite_vmp_compares : utils_vmp_vmi
{
	void cgt(const vm_int32 const_val)
	{
		// Create the main package
		auto main_package = vmp_package_newsz("main", 4);
		vmp_pipeline_add_package(pipeline, main_package);

		// Create the Get function and add two integer types
		auto get = vmp_func_newsz("Get", 3);
		auto get_arg1 = vmp_func_new_arg(get, get_type("vm", string(name<vm_int32>())));
		vmp_func_new_return(get, get_type("vm", string(name<vm_int32>())));
		vmp_package_add_func(main_package, get);

		// {
		//	lda 0
		//	ldc_i4 10
		//	cgt
		//	ret
		// }
		vmp_func_begin_body(get);
		vmp_func_add_instr(get, vmp_instr_lda(get_arg1));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type<vm_int32>(), to_const(10)));
		vmp_func_add_instr(get, vmp_instr_cgt(get_type<vm_int32>()));
		vmp_func_add_instr(get, vmp_instr_ret());
		vmp_func_begin_end(get);

		compile();

		push_value<vm_int32>(const_val);
		invoke("Get");

		verify_stack_size(sizeof(vm_int32) * 2);
		verify_value(pop_value<vm_int32>(), const_val > 10 ? TRUE : FALSE);
		verify_value(pop_value<vm_int32>(), const_val);
	}

	void clt(const vm_int32 const_val)
	{
		// Create the main package
		auto main_package = vmp_package_newsz("main", 4);
		vmp_pipeline_add_package(pipeline, main_package);

		// Create the Get function and add two integer types
		auto get = vmp_func_newsz("Get", 3);
		auto get_arg1 = vmp_func_new_arg(get, get_type<vm_int32>());
		vmp_func_new_return(get, get_type<vm_int32>());
		vmp_package_add_func(main_package, get);

		// {
		//	lda 0
		//	ldc_i4 10
		//	clt
		//	ret
		// }
		vmp_func_begin_body(get);
		vmp_func_add_instr(get, vmp_instr_lda(get_arg1));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type<vm_int32>(), to_const(10)));
		vmp_func_add_instr(get, vmp_instr_clt(get_type<vm_int32>()));
		vmp_func_add_instr(get, vmp_instr_ret());
		vmp_func_begin_end(get);

		compile();

		push_value<vm_int32>(const_val);
		invoke("Get");

		verify_stack_size(sizeof(vm_int32) * 2);
		verify_value(pop_value<vm_int32>(), const_val < 10 ? TRUE : FALSE);
		verify_value(pop_value<vm_int32>(), const_val);
	}

	void operator()()
	{
		TEST(cgt(100));
		TEST(cgt(1));
		TEST(cgt(-1));

		TEST(clt(100));
		TEST(clt(1));
		TEST(clt(-1));
	}
};

struct suite_vmp_jumps : utils_vmp_vmi
{
	void jmpt(const vm_int32 const_val)
	{
		// Create the main package
		auto main_package = vmp_package_newsz("main", 4);
		vmp_pipeline_add_package(pipeline, main_package);

		// Create the Get function and add two integer types
		auto get = vmp_func_newsz("Get", 3);
		auto get_arg1 = vmp_func_new_arg(get, get_type<vm_int32>());
		vmp_func_new_return(get, get_type<vm_int32>());
		vmp_package_add_func(main_package, get);

		// {
		//	lda 0
		//	ldc_i4 10
		//	clt
		//	jmpt #marker
		//	ldc_i4 50
		//  ret
		//	#marker
		//	ldc_i4 150
		//	ret
		// }
		vmp_func_begin_body(get);
		auto marker = vmp_func_new_marker(get);
		vmp_func_add_instr(get, vmp_instr_lda(get_arg1));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type<vm_int32>(), to_const(10)));
		vmp_func_add_instr(get, vmp_instr_clt(get_type<vm_int32>()));
		vmp_func_add_instr(get, vmp_instr_jmpt(marker));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type<vm_int32>(), to_const(50)));
		vmp_func_add_instr(get, vmp_instr_ret());
		vmp_marker_set_instr(marker,
			vmp_func_add_instr(get, vmp_instr_ldc(get_type<vm_int32>(), to_const(150)))
		);
		vmp_func_add_instr(get, vmp_instr_ret());
		vmp_func_begin_end(get);

		compile();

		push_value<vm_int32>(const_val);
		invoke("Get");

		verify_stack_size(sizeof(vm_int32) * 2);
		verify_value(pop_value<vm_int32>(), const_val < 10 ? 150 : 50);
		verify_value(pop_value<vm_int32>(), const_val);
	}

	void jmpf(const vm_int32 const_val)
	{
		// Create the main package
		auto main_package = vmp_package_newsz("main", 4);
		vmp_pipeline_add_package(pipeline, main_package);

		// Create the Get function and add two integer types
		auto get = vmp_func_newsz("Get", 3);
		auto get_arg1 = vmp_func_new_arg(get, get_type<vm_int32>());
		vmp_func_new_return(get, get_type<vm_int32>());
		vmp_package_add_func(main_package, get);

		// {
		//	lda 0
		//	ldc_i4 10
		//	clt
		//	jmpf #marker
		//	ldc_i4 50
		//  ret
		//	#marker
		//	ldc_i4 150
		//	ret
		// }
		vmp_func_begin_body(get);
		auto marker = vmp_func_new_marker(get);
		vmp_func_add_instr(get, vmp_instr_lda(get_arg1));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type<vm_int32>(), to_const(10)));
		vmp_func_add_instr(get, vmp_instr_clt(get_type<vm_int32>()));
		vmp_func_add_instr(get, vmp_instr_jmpf(marker));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type<vm_int32>(), to_const(50)));
		vmp_func_add_instr(get, vmp_instr_ret());
		vmp_marker_set_instr(marker,
			vmp_func_add_instr(get, vmp_instr_ldc(get_type<vm_int32>(), to_const(150)))
		);
		vmp_func_add_instr(get, vmp_instr_ret());
		vmp_func_begin_end(get);

		compile();

		push_value<vm_int32>(const_val);
		invoke("Get");

		verify_stack_size(sizeof(vm_int32) * 2);
		verify_value(pop_value<vm_int32>(), !(const_val < 10) ? 150 : 50);
		verify_value(pop_value<vm_int32>(), const_val);
	}

	void operator()()
	{
		TEST(jmpt(1));
		TEST(jmpt(100));
		TEST(jmpt(-1));

		TEST(jmpf(1));
		TEST(jmpf(100));
		TEST(jmpf(-1));
	}
};

struct suite_vmp_calls : utils_vmp_vmi
{
	void call()
	{
		// Create the main package
		auto main_package = vmp_package_newsz("main", 4);
		vmp_pipeline_add_package(pipeline, main_package);

		// Create the Add function and add two integer types
		auto add = vmp_func_newsz("Add", 3);
		auto add_arg1 = vmp_func_new_arg(add, get_type<vm_int32>());
		vmp_arg_set_namesz(add_arg1, "lhs", 3);
		auto add_arg2 = vmp_func_new_arg(add, get_type<vm_int32>());
		vmp_arg_set_namesz(add_arg2, "rhs", 3);
		vmp_func_new_return(add, get_type<vm_int32>());
		vmp_package_add_func(main_package, add);

		// {
		//	lda 0
		//	lda 1
		//	add int32
		//	ret
		// }
		vmp_func_begin_body(add);
		vmp_func_add_instr(add, vmp_instr_lda(add_arg1));
		vmp_func_add_instr(add, vmp_instr_lda(add_arg2));
		vmp_func_add_instr(add, vmp_instr_add(get_type<vm_int32>()));
		vmp_func_add_instr(add, vmp_instr_ret());
		vmp_func_begin_end(add);

		// Create the Get function and add two integer types
		auto get = vmp_func_newsz("Get", 3);
		vmp_func_new_return(get, get_type<vm_int32>());
		auto get_local_1 = vmp_func_new_local(get, get_type<vm_int32>());
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
		vmp_func_add_instr(get, vmp_instr_ldc(get_type<vm_int32>(), to_const(const_val1)));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type<vm_int32>(), to_const(const_val2)));
		vmp_func_add_instr(get, vmp_instr_call(add));
		vmp_func_add_instr(get, vmp_instr_stl(get_local_1));
		vmp_func_add_instr(get, vmp_instr_frees(get_type<vm_int32>()));
		vmp_func_add_instr(get, vmp_instr_frees(get_type<vm_int32>()));
		vmp_func_add_instr(get, vmp_instr_ldl(get_local_1));
		vmp_func_add_instr(get, vmp_instr_ret());
		vmp_func_begin_end(get);

		compile();

		invoke("Get");

		verify_stack_size(sizeof(vm_int32));
		verify_value(pop_value<vm_int32>(), (vm_int32)(const_val1 + const_val2));
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
		// Create the main package
		auto main_package = vmp_package_newsz("main", 4);
		vmp_pipeline_add_package(pipeline, main_package);

		// Create the Add function and add two integer types
		auto add = vmp_func_newsz("Add", 3);
		auto add_arg1 = vmp_func_new_arg(add, get_type<vm_int32>());
		vmp_arg_set_namesz(add_arg1, "lhs", 3);
		auto add_arg2 = vmp_func_new_arg(add, get_type<vm_int32>());
		vmp_arg_set_namesz(add_arg2, "rhs", 3);
		vmp_func_new_return(add, get_type<vm_int32>());
		vmp_func_set_nativefunc(add, &callnative_add);
		vmp_package_add_func(main_package, add);

		// Create the Get function and add two integer types
		auto get = vmp_func_newsz("Get", 3);
		vmp_func_new_return(get, get_type<vm_int32>());
		auto get_local_1 = vmp_func_new_local(get, get_type<vm_int32>());
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
		vmp_func_add_instr(get, vmp_instr_ldc(get_type<vm_int32>(), to_const(const_val1)));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type<vm_int32>(), to_const(const_val2)));
		vmp_func_add_instr(get, vmp_instr_callnative(add));
		vmp_func_add_instr(get, vmp_instr_stl(get_local_1));
		vmp_func_add_instr(get, vmp_instr_frees(get_type<vm_int32>()));
		vmp_func_add_instr(get, vmp_instr_frees(get_type<vm_int32>()));
		vmp_func_add_instr(get, vmp_instr_ldl(get_local_1));
		vmp_func_add_instr(get, vmp_instr_ret());
		vmp_func_begin_end(get);

		compile();

		invoke("Get");

		verify_stack_size(sizeof(vm_int32));
		verify_value(pop_value<vm_int32>(), (vm_int32)(const_val1 + const_val2));
	}

	void calluref()
	{
		// Create the main package
		auto main_package = vmp_package_newsz("main", 4);
		vmp_pipeline_add_package(pipeline, main_package);

		// Create the Add function and add two integer types
		auto add = vmp_func_newsz("Add", 3);
		auto add_arg1 = vmp_func_new_arg(add, get_type<vm_int32>());
		vmp_arg_set_namesz(add_arg1, "lhs", 3);
		auto add_arg2 = vmp_func_new_arg(add, get_type<vm_int32>());
		vmp_arg_set_namesz(add_arg2, "rhs", 3);
		vmp_func_new_return(add, get_type<vm_int32>());
		vmp_package_add_func(main_package, add);

		// {
		//	lda 0
		//	lda 1
		//	add int32
		//	ret
		// }
		vmp_func_begin_body(add);
		vmp_func_add_instr(add, vmp_instr_lda(add_arg1));
		vmp_func_add_instr(add, vmp_instr_lda(add_arg2));
		vmp_func_add_instr(add, vmp_instr_add(get_type<vm_int32>()));
		vmp_func_add_instr(add, vmp_instr_ret());
		vmp_func_begin_end(add);

		// Create the Get function and add two integer types
		auto get = vmp_func_newsz("Get", 3);
		vmp_func_new_return(get, get_type<vm_int32>());
		auto get_local_1 = vmp_func_new_local(get, get_type<vm_int32>());
		vmp_package_add_func(main_package, get);

		// {
		//	locals (_1 int32)
		//	ldc_i4 <const_val2>
		//  ldc_i4 <const_val1>
		//	ldf Add()(int32)
		//	calluref ()(int32)
		//	stl 0
		//	frees int32 * 2
		//  ldl 0
		//	ret
		// }
		auto const_val1 = 10;
		auto const_val2 = 20;
		vmp_func_begin_body(get);
		vmp_func_add_instr(get, vmp_instr_ldc(get_type<vm_int32>(), to_const(const_val1)));
		vmp_func_add_instr(get, vmp_instr_ldc(get_type<vm_int32>(), to_const(const_val2)));
		vmp_func_add_instr(get, vmp_instr_ldf(add));
		vmp_func_add_instr(get, vmp_instr_calluref(add));
		vmp_func_add_instr(get, vmp_instr_stl(get_local_1));
		vmp_func_add_instr(get, vmp_instr_frees(get_type<vm_int32>()));
		vmp_func_add_instr(get, vmp_instr_frees(get_type<vm_int32>()));
		vmp_func_add_instr(get, vmp_instr_ldl(get_local_1));
		vmp_func_add_instr(get, vmp_instr_ret());
		vmp_func_begin_end(get);

		compile();

		invoke("Get");

		verify_stack_size(sizeof(vm_int32));
		verify_value(pop_value<vm_int32>(), (vm_int32)(const_val1 + const_val2));
	}

	void operator()()
	{
		TEST(call());
		TEST(callnative());
		TEST(calluref());
	}
};

void suite_vmp()
{
	SUITE(suite_vmp_tests);
	SUITE(suite_vmp_consts);
	SUITE(suite_vmp_locals);
	SUITE(suite_vmp_globals);
	SUITE(suite_vmp_add_sub_mult_div);
	SUITE(suite_vmp_bits);
	SUITE(suite_vmp_neg);
	SUITE(suite_vmp_convertions);
	SUITE(suite_vmp_memory);
	SUITE(suite_vmp_element);
	SUITE(suite_vmp_compares);
	SUITE(suite_vmp_jumps);
	SUITE(suite_vmp_calls);
}
