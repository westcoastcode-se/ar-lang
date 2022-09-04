#include "utils.hpp"

struct utils_vmp : utils_arlang
{
	arBuilder* pipeline;
	unordered_map<string, arB_package*> packages;
	unordered_map<string, arB_type*> types;

	void beforeEach()
	{
		utils_arlang::beforeEach();
		pipeline = arBuilder_new();
		if (pipeline == nullptr)
			throw_(error() << "could not create a new VM pipeline");

		// Create package containing the built-in types
		arB_package* const vm = arB_package_new(&to_string("vm"));
		arBuilder_add_package(pipeline, vm);
		packages["vm"] = vm;

		// Add system types
		auto type = arB_type_from_props(arB_type_props_get(&to_string("bool"), sizeof(arInt32), 0, ARLANG_PRIMITIVE_BOOL, NULL));
		arB_package_add_type(vm, type);
		types["bool"] = type;
		type = arB_type_from_props(arB_type_props_get(&to_string("*bool"), sizeof(BOOL*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type));
		arB_package_add_type(vm, type);
		types["*bool"] = type;

		type = arB_type_from_props(arB_type_props_get(&to_string("int8"), sizeof(arInt8), 0, ARLANG_PRIMITIVE_I8, NULL));
		arB_package_add_type(vm, type);
		types["int8"] = type;
		type = arB_type_from_props(arB_type_props_get(&to_string("*int8"), sizeof(arInt8*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type));
		arB_package_add_type(vm, type);
		types["*int8"] = type;

		type = arB_type_from_props(arB_type_props_get(&to_string("uint8"), sizeof(arUint8), 0, ARLANG_PRIMITIVE_UI8, NULL));
		arB_package_add_type(vm, type);
		types["uint8"] = type;
		type = arB_type_from_props(arB_type_props_get(&to_string("*uint8"), sizeof(arUint8*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type));
		arB_package_add_type(vm, type);
		types["*uint8"] = type;

		type = arB_type_from_props(arB_type_props_get(&to_string("int16"), sizeof(arInt16), 0, ARLANG_PRIMITIVE_I16, NULL));
		arB_package_add_type(vm, type);
		types["int16"] = type;
		type = arB_type_from_props(arB_type_props_get(&to_string("*int16"), sizeof(arInt16*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type));
		arB_package_add_type(vm, type);
		types["*int16"] = type;

		type = arB_type_from_props(arB_type_props_get(&to_string("uint16"), sizeof(arUint16), 0, ARLANG_PRIMITIVE_UI16, NULL));
		arB_package_add_type(vm, type);
		types["uint16"] = type;
		type = arB_type_from_props(arB_type_props_get(&to_string("*uint16"), sizeof(arUint16*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type));
		arB_package_add_type(vm, type);
		types["*uint16"] = type;

		type = arB_type_from_props(arB_type_props_get(&to_string("int32"), sizeof(arInt32), 0, ARLANG_PRIMITIVE_I32, NULL));
		arB_package_add_type(vm, type);
		types["int32"] = type;
		type = arB_type_from_props(arB_type_props_get(&to_string("*int32"), sizeof(arInt32*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type));
		arB_package_add_type(vm, type);
		types["*int32"] = type;

		type = arB_type_from_props(arB_type_props_get(&to_string("uint32"), sizeof(arUint32), 0, ARLANG_PRIMITIVE_UI32, NULL));
		arB_package_add_type(vm, type);
		types["uint32"] = type;
		type = arB_type_from_props(arB_type_props_get(&to_string("*uint32"), sizeof(arUint32*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type));
		arB_package_add_type(vm, type);
		types["*uint32"] = type;

		type = arB_type_from_props(arB_type_props_get(&to_string("int64"), sizeof(arInt64), 0, ARLANG_PRIMITIVE_I64, NULL));
		arB_package_add_type(vm, type);
		types["int64"] = type;
		type = arB_type_from_props(arB_type_props_get(&to_string("*int64"), sizeof(arInt64*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type));
		arB_package_add_type(vm, type);
		types["*int64"] = type;

		type = arB_type_from_props(arB_type_props_get(&to_string("uint64"), sizeof(arUint64), 0, ARLANG_PRIMITIVE_UI64, NULL));
		arB_package_add_type(vm, type);
		types["uint64"] = type;
		type = arB_type_from_props(arB_type_props_get(&to_string("*uint64"), sizeof(arUint64*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type));
		arB_package_add_type(vm, type);
		types["*uint64"] = type;

		type = arB_type_from_props(arB_type_props_get(&to_string("float32"), sizeof(arFloat32), 0, ARLANG_PRIMITIVE_F32, NULL));
		arB_package_add_type(vm, type);
		types["float32"] = type;
		type = arB_type_from_props(arB_type_props_get(&to_string("*float32"), sizeof(arFloat32*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type));
		arB_package_add_type(vm, type);
		types["*float32"] = type;

		type = arB_type_from_props(arB_type_props_get(&to_string("float64"), sizeof(arFloat64), 0, ARLANG_PRIMITIVE_F64, NULL));
		arB_package_add_type(vm, type);
		types["float64"] = type;
		type = arB_type_from_props(arB_type_props_get(&to_string("*float64"), sizeof(arFloat64*), arB_TYPE_FLAGS_PTR, ARLANG_PRIMITIVE_PTR, type));
		arB_package_add_type(vm, type);
		types["*float64"] = type;
	}

	void afterEach()
	{
		if (pipeline != NULL) {
			arBuilder_destroy(pipeline);
			pipeline = NULL;
		}

		types.clear();
		packages.clear();

		arDebugMemory_verify();
		utils_arlang::afterEach();
	}

	arString to_string(const char* str) {
		const int len = arStrlen(str);
		return { str, str + len };
	}

	arB_package* vm_package() {
		return packages["vm"];
	}

	arB_type* get_type(const string type_name) {
		return types[type_name];
	}

	template<typename T>
	arB_type* get_type() {
		return get_type(name<T>());
	}

	arInt32 verify_instr(arB_func* func, arB_definition* instr, arIncode type, arInt32 offset, size_t size)
	{
		verify_value<const arB_func*>(instr->func, func);
		verify_value(instr->instr_type, type);
		verify_value(instr->instr_offset, offset);
		verify_value(instr->instr_size, (arInt32)size);
		return (arInt32)(offset + size);
	}
};

struct utils_vmp_vmi : utils_vmp
{
	arProcess* process;
	arThread* thread;

	void beforeEach()
	{
		utils_vmp::beforeEach();
		thread = nullptr;
		process = nullptr;
	}

	void afterEach()
	{
		if (thread != nullptr) {
			arThread_destroy(thread);
			thread = nullptr;
		}

		if (process != nullptr) {
			arProcess_destroy(process);
			process = nullptr;
		}

		utils_vmp::afterEach();
	}

	void compile()
	{
		if (!arBuilder_resolve(pipeline)) {
			throw_(error() << "failed to resolve pipeline information");
		}

		if (!arBuilder_compile(pipeline)) {
			error_string_stream e;
			e << "could not compile pipeline: [";
			auto message = pipeline->messages.first;
			while (message != nullptr) {
				e << "\n" << message->message;
				message = message->next;
			}
			if (pipeline->panic_error_message.code != 0) {
				e << "\n" << pipeline->panic_error_message.message;
			}
			e << "\n]";
			throw_(e);
		}

		process = arProcess_new();
		if (process == nullptr)
			throw_(error() << "could not create a new VM process");

		const auto result = arProcess_load(process, arBuilder_get_bytecode(pipeline));
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
		const arPackage* package = arProcess_find_package(process, "main", 4);
		if (package == NULL) {
			throw_(error() << "expected 'main' package but was not found");
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
	T push_value(T value)
	{
		*(T*)arThread_pushStack(thread, sizeof(T)) = (T)value;
		return value;
	}

	template<typename T, int SIZE>
	T* push_value(T value[SIZE])
	{
		T* p = (T*)arThread_pushStack(thread, sizeof(T[SIZE]));
		for (int i = 0; i < SIZE; ++i)
			p[i] = value[i];
		return p;
	}

	template<typename T>
	T pop_value()
	{
		return *(T*)arThread_popStack(thread, sizeof(T));
	}

	template<typename T, int SIZE>
	vector<T> pop_value()
	{
		T* arr = (T*)arThread_popStack(thread, sizeof(T[2]));
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

struct suite_builder_tests : utils_vmp
{
	void instr_offset()
	{
		arString name;
		name.start = "test";
		name.end = name.start + 4;
		arB_func* const func = arB_func_new(&name);
		verify_not_null(func);
		arB_func_add_instr(func, arB_instr_lda(0));
		arB_func_add_instr(func, arB_instr_lda(0));
		arB_func_add_instr(func, arB_instr_lda(0));

		verify_value(func->body_size, (arUint32)(sizeof(arInstruction_lda) * 3));
		arInt32 offset = 0;
		offset = verify_instr(func, func->first_instr, VMI_LDA, offset, sizeof(arInstruction_lda));
		offset = verify_instr(func, func->first_instr->next, VMI_LDA, offset, sizeof(arInstruction_lda));
		offset = verify_instr(func, func->first_instr->next->next, VMI_LDA, offset, sizeof(arInstruction_lda));
		verify_null(func->first_instr->next->next->next);

		arB_func_destroy(func);
	}

	void instr_offset_inject_start()
	{
		arString name;
		name.start = "test";
		name.end = name.start + 4;
		arB_func* const func = arB_func_new(&name);
		verify_not_null(func);
		arB_func_add_instr(func, arB_instr_lda(0));
		arB_func_add_instr(func, arB_instr_lda(0));
		arB_func_add_instr(func, arB_instr_lda(0));
		arB_func_inject_after(func, NULL, arB_instr_lda_a(0));

		verify_value(func->body_size, (arUint32)(sizeof(arInstruction_lda) * 3 + sizeof(arInstruction_lda_a)));

		arInt32 offset = 0;
		offset = verify_instr(func, func->first_instr, VMI_LDA_A, offset, sizeof(arInstruction_lda_a));
		offset = verify_instr(func, func->first_instr->next, VMI_LDA, offset, sizeof(arInstruction_lda));
		offset = verify_instr(func, func->first_instr->next->next, VMI_LDA, offset, sizeof(arInstruction_lda));
		offset = verify_instr(func, func->first_instr->next->next->next, VMI_LDA, offset, sizeof(arInstruction_lda));
		verify_null(func->first_instr->next->next->next->next);

		arB_func_destroy(func);
	}

	void instr_offset_inject_start_multiple()
	{
		arString name;
		name.start = "test";
		name.end = name.start + 4;
		arB_func* const func = arB_func_new(&name);
		verify_not_null(func);
		arB_func_add_instr(func, arB_instr_lda(0));
		arB_func_add_instr(func, arB_instr_lda(0));
		arB_func_add_instr(func, arB_instr_lda(0));
		arB_func_inject_after(func, NULL, arB_instr_link(arB_instr_lda_a(0), arB_instr_stl(0)));

		verify_value(func->body_size, (arUint32)(sizeof(arInstruction_lda) * 3 + sizeof(arInstruction_lda_a) + sizeof(arInstruction_stl)));

		arInt32 offset = 0;
		offset = verify_instr(func, func->first_instr, VMI_LDA_A, offset, sizeof(arInstruction_lda_a));
		offset = verify_instr(func, func->first_instr->next, VMI_STL, offset, sizeof(arInstruction_stl));
		offset = verify_instr(func, func->first_instr->next->next, VMI_LDA, offset, sizeof(arInstruction_lda));
		offset = verify_instr(func, func->first_instr->next->next->next, VMI_LDA, offset, sizeof(arInstruction_lda));
		offset = verify_instr(func, func->first_instr->next->next->next->next, VMI_LDA, offset, sizeof(arInstruction_lda));
		verify_null(func->first_instr->next->next->next->next->next);

		arB_func_destroy(func);
	}

	void instr_offset_inject_end()
	{
		arString name;
		name.start = "test";
		name.end = name.start + 4;
		arB_func* const func = arB_func_new(&name);
		verify_not_null(func);
		arB_func_add_instr(func, arB_instr_lda(0));
		arB_func_add_instr(func, arB_instr_lda(0));
		arB_func_add_instr(func, arB_instr_lda(0));
		arB_func_inject_after(func, func->last_instr, arB_instr_lda_a(0));

		verify_value(func->body_size, (arUint32)(sizeof(arInstruction_lda) * 3 + sizeof(arInstruction_lda_a)));

		arInt32 offset = 0;
		offset = verify_instr(func, func->first_instr, VMI_LDA, offset, sizeof(arInstruction_lda));
		offset = verify_instr(func, func->first_instr->next, VMI_LDA, offset, sizeof(arInstruction_lda));
		offset = verify_instr(func, func->first_instr->next->next, VMI_LDA, offset, sizeof(arInstruction_lda));
		offset = verify_instr(func, func->first_instr->next->next->next, VMI_LDA_A, offset, sizeof(arInstruction_lda_a));
		verify_null(func->first_instr->next->next->next->next);

		arB_func_destroy(func);
	}

	void instr_offset_inject_end_multiple()
	{
		arString name;
		name.start = "test";
		name.end = name.start + 4;
		arB_func* const func = arB_func_new(&name);
		verify_not_null(func);
		arB_func_add_instr(func, arB_instr_lda(0));
		arB_func_add_instr(func, arB_instr_lda(0));
		arB_func_add_instr(func, arB_instr_lda(0));
		arB_func_inject_after(func, func->last_instr, arB_instr_link(arB_instr_lda_a(0), arB_instr_stl(0)));

		verify_value(func->body_size, (arUint32)(sizeof(arInstruction_lda) * 3 + sizeof(arInstruction_lda_a) + sizeof(arInstruction_stl)));

		arInt32 offset = 0;
		offset = verify_instr(func, func->first_instr, VMI_LDA, offset, sizeof(arInstruction_lda));
		offset = verify_instr(func, func->first_instr->next, VMI_LDA, offset, sizeof(arInstruction_lda));
		offset = verify_instr(func, func->first_instr->next->next, VMI_LDA, offset, sizeof(arInstruction_lda));
		offset = verify_instr(func, func->first_instr->next->next->next, VMI_LDA_A, offset, sizeof(arInstruction_lda_a));
		offset = verify_instr(func, func->first_instr->next->next->next->next, VMI_STL, offset, sizeof(arInstruction_stl));
		verify_null(func->first_instr->next->next->next->next->next);

		arB_func_destroy(func);
	}

	void instr_offset_inject_middle()
	{
		arString name;
		name.start = "test";
		name.end = name.start + 4;
		arB_func* const func = arB_func_new(&name);
		verify_not_null(func);
		arB_func_add_instr(func, arB_instr_lda(0));
		arB_func_add_instr(func, arB_instr_lda(0));
		arB_func_add_instr(func, arB_instr_lda(0));
		arB_func_inject_after(func, func->first_instr, arB_instr_lda_a(0));

		verify_value(func->body_size, (arUint32)(sizeof(arInstruction_lda) * 3 + sizeof(arInstruction_lda_a)));

		arInt32 offset = 0;
		offset = verify_instr(func, func->first_instr, VMI_LDA, offset, sizeof(arInstruction_lda));
		offset = verify_instr(func, func->first_instr->next, VMI_LDA_A, offset, sizeof(arInstruction_lda));
		offset = verify_instr(func, func->first_instr->next->next, VMI_LDA, offset, sizeof(arInstruction_lda));
		offset = verify_instr(func, func->first_instr->next->next->next, VMI_LDA, offset, sizeof(arInstruction_lda_a));
		verify_null(func->first_instr->next->next->next->next);

		arB_func_destroy(func);
	}

	void instr_offset_inject_middle_multiple()
	{
		arString name;
		name.start = "test";
		name.end = name.start + 4;
		arB_func* const func = arB_func_new(&name);
		verify_not_null(func);
		arB_func_add_instr(func, arB_instr_lda(0));
		arB_func_add_instr(func, arB_instr_lda(0));
		arB_func_add_instr(func, arB_instr_lda(0));
		arB_func_inject_after(func, func->first_instr, arB_instr_link(arB_instr_lda_a(0), arB_instr_stl(0)));

		verify_value(func->body_size, (arUint32)(sizeof(arInstruction_lda) * 3 + sizeof(arInstruction_lda_a) + sizeof(arInstruction_stl)));

		arInt32 offset = 0;
		offset = verify_instr(func, func->first_instr, VMI_LDA, offset, sizeof(arInstruction_lda));
		offset = verify_instr(func, func->first_instr->next, VMI_LDA_A, offset, sizeof(arInstruction_lda_a));
		offset = verify_instr(func, func->first_instr->next->next, VMI_STL, offset, sizeof(arInstruction_stl));
		offset = verify_instr(func, func->first_instr->next->next->next, VMI_LDA, offset, sizeof(arInstruction_lda));
		offset = verify_instr(func, func->first_instr->next->next->next->next, VMI_LDA, offset, sizeof(arInstruction_lda));
		verify_null(func->first_instr->next->next->next->next->next);

		arB_func_destroy(func);
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

struct suite_builder_neg : utils_vmp_vmi
{
	template<typename T>
	void neg_T(T value)
	{
		// Create the main package
		auto main_package = arB_package_newsz("main", 4);
		arBuilder_add_package(pipeline, main_package);

		// Create the Add function and add two integer types
		auto add = arB_func_newsz("Neg", 3);
		auto arg1 = arB_func_new_arg(add, get_type<T>());
		arB_arg_set_namesz(arg1, "lhs", 3);
		auto ret1 = arB_func_new_return(add, get_type<T>());
		arB_package_add_func(main_package, add);

		// {
		//	lda 0
		//	neg T
		//	ret
		// }
		arB_func_body_begin(add);
		arB_func_add_instr(add, arB_instr_lda(arg1));
		arB_func_add_instr(add, arB_instr_neg(get_type<T>()));
		arB_func_add_instr(add, arB_instr_ret());
		arB_func_body_end(add);

		compile();

		push_value<T>(value);
		invoke("Neg");

		verify_stack_size(sizeof(T) * 2);
		verify_value(pop_value<T>(), (T)(-value));
		verify_value(pop_value<T>(), (T)(value));
	}

	void operator()()
	{
		TEST(neg_T<arInt8>(1));
		TEST(neg_T<arUint8>(15));
		TEST(neg_T<arInt16>(10));
		TEST(neg_T<arUint16>(INT8_MAX));
		TEST(neg_T<arInt32>(INT16_MAX));
		TEST(neg_T<arUint32>((arUint32)INT16_MAX + 10));
		TEST(neg_T<arInt64>((arInt64)((arInt64)INT32_MAX + 5i64)));
		TEST(neg_T<arUint64>((arUint64)(INT32_MAX + 1000i64)));
		TEST(neg_T<arFloat32>(1.0f));
		TEST(neg_T<arFloat64>(-10.0));
	}
};


struct suite_builder_datatypes : utils_arlang
{
	template<typename LHS, typename RHS>
	void arPrimitiveValue_datatype_T(LHS lhs, RHS rhs, arPrimitive expected_data_type)
	{
		const arPrimitive lhs_dt = props1_of(lhs);
		const arPrimitive rhs_dt = props1_of(rhs);
		const auto data_type = arPrimitiveValue_datatype(lhs_dt, rhs_dt);
		assert_equals(data_type, expected_data_type);
		const arPrimitive cpp_data_type = props1_of(lhs + rhs);
		assert_equals(cpp_data_type, expected_data_type);
	}

	void operator()()
	{
		TEST(arPrimitiveValue_datatype_T((arInt8)1, true, ARLANG_PRIMITIVE_I32));
		TEST(arPrimitiveValue_datatype_T((arInt8)1, (arInt8)INT8_MAX, ARLANG_PRIMITIVE_I32));
		TEST(arPrimitiveValue_datatype_T((arInt8)1, (arUint8)INT8_MAX, ARLANG_PRIMITIVE_I32));
		TEST(arPrimitiveValue_datatype_T((arInt8)1, (arInt16)INT16_MAX, ARLANG_PRIMITIVE_I32));
		TEST(arPrimitiveValue_datatype_T((arInt8)1, (arUint16)INT16_MAX, ARLANG_PRIMITIVE_I32));
		TEST(arPrimitiveValue_datatype_T((arInt8)1, (arInt32)INT32_MAX, ARLANG_PRIMITIVE_I32));
		TEST(arPrimitiveValue_datatype_T((arInt8)1, (arUint32)INT32_MAX, ARLANG_PRIMITIVE_UI32));
		TEST(arPrimitiveValue_datatype_T((arInt8)1, (arInt64)INT64_MAX, ARLANG_PRIMITIVE_I64));
		TEST(arPrimitiveValue_datatype_T((arInt8)1, (arUint64)INT64_MAX, ARLANG_PRIMITIVE_UI64));
		TEST(arPrimitiveValue_datatype_T((arInt8)1, (arFloat32)1.0f, ARLANG_PRIMITIVE_F32));
		TEST(arPrimitiveValue_datatype_T((arInt8)1, (arFloat64)1.0, ARLANG_PRIMITIVE_F64));

		TEST(arPrimitiveValue_datatype_T((arUint8)1, true, ARLANG_PRIMITIVE_I32));
		TEST(arPrimitiveValue_datatype_T((arUint8)1, (arInt8)INT8_MAX, ARLANG_PRIMITIVE_I32));
		TEST(arPrimitiveValue_datatype_T((arUint8)1, (arUint8)INT8_MAX, ARLANG_PRIMITIVE_I32));
		TEST(arPrimitiveValue_datatype_T((arUint8)1, (arInt16)INT16_MAX, ARLANG_PRIMITIVE_I32));
		TEST(arPrimitiveValue_datatype_T((arUint8)1, (arUint16)INT16_MAX, ARLANG_PRIMITIVE_I32));
		TEST(arPrimitiveValue_datatype_T((arUint8)1, (arInt32)INT32_MAX, ARLANG_PRIMITIVE_I32));
		TEST(arPrimitiveValue_datatype_T((arUint8)1, (arUint32)INT32_MAX, ARLANG_PRIMITIVE_UI32));
		TEST(arPrimitiveValue_datatype_T((arUint8)1, (arInt64)INT64_MAX, ARLANG_PRIMITIVE_I64));
		TEST(arPrimitiveValue_datatype_T((arUint8)1, (arUint64)INT64_MAX, ARLANG_PRIMITIVE_UI64));
		TEST(arPrimitiveValue_datatype_T((arUint8)1, (arFloat32)1.0f, ARLANG_PRIMITIVE_F32));
		TEST(arPrimitiveValue_datatype_T((arUint8)1, (arFloat64)1.0, ARLANG_PRIMITIVE_F64));

		TEST(arPrimitiveValue_datatype_T((arInt16)1, true, ARLANG_PRIMITIVE_I32));
		TEST(arPrimitiveValue_datatype_T((arInt16)1, (arInt8)INT8_MAX, ARLANG_PRIMITIVE_I32));
		TEST(arPrimitiveValue_datatype_T((arInt16)1, (arUint8)INT8_MAX, ARLANG_PRIMITIVE_I32));
		TEST(arPrimitiveValue_datatype_T((arInt16)1, (arInt16)INT16_MAX, ARLANG_PRIMITIVE_I32));
		TEST(arPrimitiveValue_datatype_T((arInt16)1, (arUint16)INT16_MAX, ARLANG_PRIMITIVE_I32));
		TEST(arPrimitiveValue_datatype_T((arInt16)1, (arInt32)INT32_MAX, ARLANG_PRIMITIVE_I32));
		TEST(arPrimitiveValue_datatype_T((arInt16)1, (arUint32)INT32_MAX, ARLANG_PRIMITIVE_UI32));
		TEST(arPrimitiveValue_datatype_T((arInt16)1, (arInt64)INT64_MAX, ARLANG_PRIMITIVE_I64));
		TEST(arPrimitiveValue_datatype_T((arInt16)1, (arUint64)INT64_MAX, ARLANG_PRIMITIVE_UI64));
		TEST(arPrimitiveValue_datatype_T((arInt16)1, (arFloat32)1.0f, ARLANG_PRIMITIVE_F32));
		TEST(arPrimitiveValue_datatype_T((arInt16)1, (arFloat64)1.0, ARLANG_PRIMITIVE_F64));

		TEST(arPrimitiveValue_datatype_T((arUint16)1, true, ARLANG_PRIMITIVE_I32));
		TEST(arPrimitiveValue_datatype_T((arUint16)1, (arInt8)INT8_MAX, ARLANG_PRIMITIVE_I32));
		TEST(arPrimitiveValue_datatype_T((arUint16)1, (arUint8)INT8_MAX, ARLANG_PRIMITIVE_I32));
		TEST(arPrimitiveValue_datatype_T((arUint16)1, (arInt16)INT16_MAX, ARLANG_PRIMITIVE_I32));
		TEST(arPrimitiveValue_datatype_T((arUint16)1, (arUint16)INT16_MAX, ARLANG_PRIMITIVE_I32));
		TEST(arPrimitiveValue_datatype_T((arUint16)1, (arInt32)INT32_MAX, ARLANG_PRIMITIVE_I32));
		TEST(arPrimitiveValue_datatype_T((arUint16)1, (arUint32)INT32_MAX, ARLANG_PRIMITIVE_UI32));
		TEST(arPrimitiveValue_datatype_T((arUint16)1, (arInt64)INT64_MAX, ARLANG_PRIMITIVE_I64));
		TEST(arPrimitiveValue_datatype_T((arUint16)1, (arUint64)INT64_MAX, ARLANG_PRIMITIVE_UI64));
		TEST(arPrimitiveValue_datatype_T((arUint16)1, (arFloat32)1.0f, ARLANG_PRIMITIVE_F32));
		TEST(arPrimitiveValue_datatype_T((arUint16)1, (arFloat64)1.0, ARLANG_PRIMITIVE_F64));

		TEST(arPrimitiveValue_datatype_T((arInt32)1, true, ARLANG_PRIMITIVE_I32));
		TEST(arPrimitiveValue_datatype_T((arInt32)1, (arInt8)INT8_MAX, ARLANG_PRIMITIVE_I32));
		TEST(arPrimitiveValue_datatype_T((arInt32)1, (arUint8)INT8_MAX, ARLANG_PRIMITIVE_I32));
		TEST(arPrimitiveValue_datatype_T((arInt32)1, (arInt16)INT16_MAX, ARLANG_PRIMITIVE_I32));
		TEST(arPrimitiveValue_datatype_T((arInt32)1, (arUint16)INT16_MAX, ARLANG_PRIMITIVE_I32));
		TEST(arPrimitiveValue_datatype_T((arInt32)1, (arInt32)INT32_MAX, ARLANG_PRIMITIVE_I32));
		TEST(arPrimitiveValue_datatype_T((arInt32)1, (arUint32)INT32_MAX, ARLANG_PRIMITIVE_UI32));
		TEST(arPrimitiveValue_datatype_T((arInt32)1, (arInt64)INT64_MAX, ARLANG_PRIMITIVE_I64));
		TEST(arPrimitiveValue_datatype_T((arInt32)1, (arUint64)INT64_MAX, ARLANG_PRIMITIVE_UI64));
		TEST(arPrimitiveValue_datatype_T((arInt32)1, (arFloat32)1.0f, ARLANG_PRIMITIVE_F32));
		TEST(arPrimitiveValue_datatype_T((arInt32)1, (arFloat64)1.0, ARLANG_PRIMITIVE_F64));

		TEST(arPrimitiveValue_datatype_T((arUint32)1, true, ARLANG_PRIMITIVE_UI32));
		TEST(arPrimitiveValue_datatype_T((arUint32)1, (arInt8)INT8_MAX, ARLANG_PRIMITIVE_UI32));
		TEST(arPrimitiveValue_datatype_T((arUint32)1, (arUint8)INT8_MAX, ARLANG_PRIMITIVE_UI32));
		TEST(arPrimitiveValue_datatype_T((arUint32)1, (arInt16)INT16_MAX, ARLANG_PRIMITIVE_UI32));
		TEST(arPrimitiveValue_datatype_T((arUint32)1, (arUint16)INT16_MAX, ARLANG_PRIMITIVE_UI32));
		TEST(arPrimitiveValue_datatype_T((arUint32)1, (arInt32)INT32_MAX, ARLANG_PRIMITIVE_UI32));
		TEST(arPrimitiveValue_datatype_T((arUint32)1, (arUint32)INT32_MAX, ARLANG_PRIMITIVE_UI32));
		TEST(arPrimitiveValue_datatype_T((arUint32)1, (arInt64)INT64_MAX, ARLANG_PRIMITIVE_I64));
		TEST(arPrimitiveValue_datatype_T((arUint32)1, (arUint64)INT64_MAX, ARLANG_PRIMITIVE_UI64));
		TEST(arPrimitiveValue_datatype_T((arUint32)1, (arFloat32)1.0f, ARLANG_PRIMITIVE_F32));
		TEST(arPrimitiveValue_datatype_T((arUint32)1, (arFloat64)1.0, ARLANG_PRIMITIVE_F64));

		TEST(arPrimitiveValue_datatype_T((arInt64)1, true, ARLANG_PRIMITIVE_I64));
		TEST(arPrimitiveValue_datatype_T((arInt64)1, (arInt8)INT8_MAX, ARLANG_PRIMITIVE_I64));
		TEST(arPrimitiveValue_datatype_T((arInt64)1, (arUint8)INT8_MAX, ARLANG_PRIMITIVE_I64));
		TEST(arPrimitiveValue_datatype_T((arInt64)1, (arInt16)INT16_MAX, ARLANG_PRIMITIVE_I64));
		TEST(arPrimitiveValue_datatype_T((arInt64)1, (arUint16)INT16_MAX, ARLANG_PRIMITIVE_I64));
		TEST(arPrimitiveValue_datatype_T((arInt64)1, (arInt32)INT32_MAX, ARLANG_PRIMITIVE_I64));
		TEST(arPrimitiveValue_datatype_T((arInt64)1, (arUint32)INT32_MAX, ARLANG_PRIMITIVE_I64));
		TEST(arPrimitiveValue_datatype_T((arInt64)1, (arInt64)INT64_MAX, ARLANG_PRIMITIVE_I64));
		TEST(arPrimitiveValue_datatype_T((arInt64)1, (arUint64)INT64_MAX, ARLANG_PRIMITIVE_UI64));
		TEST(arPrimitiveValue_datatype_T((arInt64)1, (arFloat32)1.0f, ARLANG_PRIMITIVE_F32));
		TEST(arPrimitiveValue_datatype_T((arInt64)1, (arFloat64)1.0, ARLANG_PRIMITIVE_F64));

		TEST(arPrimitiveValue_datatype_T((arUint64)1, true, ARLANG_PRIMITIVE_UI64));
		TEST(arPrimitiveValue_datatype_T((arUint64)1, (arInt8)INT8_MAX, ARLANG_PRIMITIVE_UI64));
		TEST(arPrimitiveValue_datatype_T((arUint64)1, (arUint8)INT8_MAX, ARLANG_PRIMITIVE_UI64));
		TEST(arPrimitiveValue_datatype_T((arUint64)1, (arInt16)INT16_MAX, ARLANG_PRIMITIVE_UI64));
		TEST(arPrimitiveValue_datatype_T((arUint64)1, (arUint16)INT16_MAX, ARLANG_PRIMITIVE_UI64));
		TEST(arPrimitiveValue_datatype_T((arUint64)1, (arInt32)INT32_MAX, ARLANG_PRIMITIVE_UI64));
		TEST(arPrimitiveValue_datatype_T((arUint64)1, (arUint32)INT32_MAX, ARLANG_PRIMITIVE_UI64));
		TEST(arPrimitiveValue_datatype_T((arUint64)1, (arInt64)INT64_MAX, ARLANG_PRIMITIVE_UI64));
		TEST(arPrimitiveValue_datatype_T((arUint64)1, (arUint64)INT64_MAX, ARLANG_PRIMITIVE_UI64));
		TEST(arPrimitiveValue_datatype_T((arUint64)1, (arFloat32)1.0f, ARLANG_PRIMITIVE_F32));
		TEST(arPrimitiveValue_datatype_T((arUint64)1, (arFloat64)1.0, ARLANG_PRIMITIVE_F64));

		TEST(arPrimitiveValue_datatype_T((arFloat32)1, true, ARLANG_PRIMITIVE_F32));
		TEST(arPrimitiveValue_datatype_T((arFloat32)1, (arInt8)INT8_MAX, ARLANG_PRIMITIVE_F32));
		TEST(arPrimitiveValue_datatype_T((arFloat32)1, (arUint8)INT8_MAX, ARLANG_PRIMITIVE_F32));
		TEST(arPrimitiveValue_datatype_T((arFloat32)1, (arInt16)INT16_MAX, ARLANG_PRIMITIVE_F32));
		TEST(arPrimitiveValue_datatype_T((arFloat32)1, (arUint16)INT16_MAX, ARLANG_PRIMITIVE_F32));
		TEST(arPrimitiveValue_datatype_T((arFloat32)1, (arInt32)INT32_MAX, ARLANG_PRIMITIVE_F32));
		TEST(arPrimitiveValue_datatype_T((arFloat32)1, (arUint32)INT32_MAX, ARLANG_PRIMITIVE_F32));
		TEST(arPrimitiveValue_datatype_T((arFloat32)1, (arInt64)INT64_MAX, ARLANG_PRIMITIVE_F32));
		TEST(arPrimitiveValue_datatype_T((arFloat32)1, (arUint64)INT64_MAX, ARLANG_PRIMITIVE_F32));
		TEST(arPrimitiveValue_datatype_T((arFloat32)1, (arFloat32)1.0f, ARLANG_PRIMITIVE_F32));
		TEST(arPrimitiveValue_datatype_T((arFloat32)1, (arFloat64)1.0, ARLANG_PRIMITIVE_F64));

		TEST(arPrimitiveValue_datatype_T((arFloat64)1, true, ARLANG_PRIMITIVE_F64));
		TEST(arPrimitiveValue_datatype_T((arFloat64)1, (arInt8)INT8_MAX, ARLANG_PRIMITIVE_F64));
		TEST(arPrimitiveValue_datatype_T((arFloat64)1, (arUint8)INT8_MAX, ARLANG_PRIMITIVE_F64));
		TEST(arPrimitiveValue_datatype_T((arFloat64)1, (arInt16)INT16_MAX, ARLANG_PRIMITIVE_F64));
		TEST(arPrimitiveValue_datatype_T((arFloat64)1, (arUint16)INT16_MAX, ARLANG_PRIMITIVE_F64));
		TEST(arPrimitiveValue_datatype_T((arFloat64)1, (arInt32)INT32_MAX, ARLANG_PRIMITIVE_F64));
		TEST(arPrimitiveValue_datatype_T((arFloat64)1, (arUint32)INT32_MAX, ARLANG_PRIMITIVE_F64));
		TEST(arPrimitiveValue_datatype_T((arFloat64)1, (arInt64)INT64_MAX, ARLANG_PRIMITIVE_F64));
		TEST(arPrimitiveValue_datatype_T((arFloat64)1, (arUint64)INT64_MAX, ARLANG_PRIMITIVE_F64));
		TEST(arPrimitiveValue_datatype_T((arFloat64)1, (arFloat32)1.0f, ARLANG_PRIMITIVE_F64));
		TEST(arPrimitiveValue_datatype_T((arFloat64)1, (arFloat64)1.0, ARLANG_PRIMITIVE_F64));

		char* ptr = (char*)0x1;
		//TEST(arPrimitiveValue_datatype_T(ptr, true, 0));
		TEST(arPrimitiveValue_datatype_T(ptr, (arInt8)INT8_MAX, ARLANG_PRIMITIVE_PTR));
		TEST(arPrimitiveValue_datatype_T(ptr, (arUint8)INT8_MAX, ARLANG_PRIMITIVE_PTR));
		TEST(arPrimitiveValue_datatype_T(ptr, (arInt16)INT16_MAX, ARLANG_PRIMITIVE_PTR));
		TEST(arPrimitiveValue_datatype_T(ptr, (arUint16)INT16_MAX, ARLANG_PRIMITIVE_PTR));
		TEST(arPrimitiveValue_datatype_T(ptr, (arInt32)INT32_MAX, ARLANG_PRIMITIVE_PTR));
		TEST(arPrimitiveValue_datatype_T(ptr, (arUint32)INT32_MAX, ARLANG_PRIMITIVE_PTR));
		TEST(arPrimitiveValue_datatype_T(ptr, (arInt64)INT64_MAX, ARLANG_PRIMITIVE_PTR));
		TEST(arPrimitiveValue_datatype_T(ptr, (arUint64)INT64_MAX, ARLANG_PRIMITIVE_PTR));
		//TEST(arPrimitiveValue_datatype_T(ptr, (arFloat32)1.0f, 0));
		//TEST(arPrimitiveValue_datatype_T(ptr, (arFloat64)1.0, 0));
	}
};

struct suite_builder_primitives : utils_vmp_vmi
{
	template<typename T>
	void ldc_T(T value)
	{
		// Create the main package
		auto main_package = arB_package_newsz("main", 4);
		arBuilder_add_package(pipeline, main_package);

		auto primitive_type = get_type<T>();

		// Create the Get function that returns a constant value
		auto add = arB_func_newsz("Get", 3);
		auto ret1 = arB_func_new_return(add, primitive_type);
		arB_package_add_func(main_package, add);

		// {
		//	ldc_<T> $value
		//	ret
		// }
		arB_func_body_begin(add);
		arB_func_add_instr(add, arB_instr_ldc(primitive_type, to_const((T)value)));
		arB_func_add_instr(add, arB_instr_ret());
		arB_func_body_end(add);

		compile();

		invoke("Get");

		verify_stack_size(sizeof(T));
		verify_value(pop_value<T>(), (T)value);
	}

	template<typename LHS, typename RHS>
	void arPrimitiveValue_add_T(LHS lhs, RHS rhs)
	{
		arPrimitiveValue lhs_const = to_const(lhs);
		arPrimitiveValue rhs_const = to_const(rhs);
		arPrimitiveValue_add(&lhs_const, &rhs_const);

		auto expected = lhs + rhs;

		if (std::is_same<decltype(expected), arInt8>::value) {
			assert_equals(lhs_const.i8, (arInt8)expected);
		}
		else if (std::is_same<decltype(expected), arUint8>::value) {
			assert_equals(lhs_const.ui8, (arUint8)expected);
		}
		else if (std::is_same<decltype(expected), arInt16>::value) {
			assert_equals(lhs_const.i16, (arInt16)expected);
		}
		else if (std::is_same<decltype(expected), arUint16>::value) {
			assert_equals(lhs_const.ui16, (arUint16)expected);
		}
		else if (std::is_same<decltype(expected), arInt32>::value) {
			assert_equals(lhs_const.i32, (arInt32)expected);
		}
		else if (std::is_same<decltype(expected), arUint32>::value) {
			assert_equals(lhs_const.ui32, (arUint32)expected);
		}
		else if (std::is_same<decltype(expected), arInt64>::value) {
			assert_equals(lhs_const.i64, (arInt64)expected);
		}
		else if (std::is_same<decltype(expected), arUint64>::value) {
			assert_equals(lhs_const.ui64, (arUint64)expected);
		}
		else if (std::is_same<decltype(expected), arFloat32>::value) {
			assert_equals(lhs_const.f32, (arFloat32)expected);
		}
		else if (std::is_same<decltype(expected), arFloat64>::value) {
			assert_equals(lhs_const.f64, (arFloat64)expected);
		}

		assert_equals(lhs_const.type, (arInt32)props1_of(expected));
	}

	template<typename LHS, typename RHS>
	void arPrimitiveValue_sub_T(LHS lhs, RHS rhs)
	{
		arPrimitiveValue lhs_const = to_const(lhs);
		arPrimitiveValue rhs_const = to_const(rhs);
		arPrimitiveValue_sub(&lhs_const, &rhs_const);

		auto expected = lhs - rhs;

		if (std::is_same<decltype(expected), arInt8>::value) {
			assert_equals(lhs_const.i8, (arInt8)expected);
		}
		else if (std::is_same<decltype(expected), arUint8>::value) {
			assert_equals(lhs_const.ui8, (arUint8)expected);
		}
		else if (std::is_same<decltype(expected), arInt16>::value) {
			assert_equals(lhs_const.i16, (arInt16)expected);
		}
		else if (std::is_same<decltype(expected), arUint16>::value) {
			assert_equals(lhs_const.ui16, (arUint16)expected);
		}
		else if (std::is_same<decltype(expected), arInt32>::value) {
			assert_equals(lhs_const.i32, (arInt32)expected);
		}
		else if (std::is_same<decltype(expected), arUint32>::value) {
			assert_equals(lhs_const.ui32, (arUint32)expected);
		}
		else if (std::is_same<decltype(expected), arInt64>::value) {
			assert_equals(lhs_const.i64, (arInt64)expected);
		}
		else if (std::is_same<decltype(expected), arUint64>::value) {
			assert_equals(lhs_const.ui64, (arUint64)expected);
		}
		else if (std::is_same<decltype(expected), arFloat32>::value) {
			assert_equals(lhs_const.f32, (arFloat32)expected);
		}
		else if (std::is_same<decltype(expected), arFloat64>::value) {
			assert_equals(lhs_const.f64, (arFloat64)expected);
		}

		assert_equals(lhs_const.type, (arInt32)props1_of(expected));
	}

	template<typename LHS, typename RHS>
	void arPrimitiveValue_mult_T(LHS lhs, RHS rhs)
	{
		arPrimitiveValue lhs_const = to_const(lhs);
		arPrimitiveValue rhs_const = to_const(rhs);
		arPrimitiveValue_mult(&lhs_const, &rhs_const);

		auto expected = lhs * rhs;

		if (std::is_same<decltype(expected), arInt8>::value) {
			assert_equals(lhs_const.i8, (arInt8)expected);
		}
		else if (std::is_same<decltype(expected), arUint8>::value) {
			assert_equals(lhs_const.ui8, (arUint8)expected);
		}
		else if (std::is_same<decltype(expected), arInt16>::value) {
			assert_equals(lhs_const.i16, (arInt16)expected);
		}
		else if (std::is_same<decltype(expected), arUint16>::value) {
			assert_equals(lhs_const.ui16, (arUint16)expected);
		}
		else if (std::is_same<decltype(expected), arInt32>::value) {
			assert_equals(lhs_const.i32, (arInt32)expected);
		}
		else if (std::is_same<decltype(expected), arUint32>::value) {
			assert_equals(lhs_const.ui32, (arUint32)expected);
		}
		else if (std::is_same<decltype(expected), arInt64>::value) {
			assert_equals(lhs_const.i64, (arInt64)expected);
		}
		else if (std::is_same<decltype(expected), arUint64>::value) {
			assert_equals(lhs_const.ui64, (arUint64)expected);
		}
		else if (std::is_same<decltype(expected), arFloat32>::value) {
			assert_equals(lhs_const.f32, (arFloat32)expected);
		}
		else if (std::is_same<decltype(expected), arFloat64>::value) {
			assert_equals(lhs_const.f64, (arFloat64)expected);
		}

		assert_equals(lhs_const.type, (arInt32)props1_of(expected));
	}

	template<typename LHS, typename RHS>
	void arPrimitiveValue_div_T(LHS lhs, RHS rhs)
	{
		arPrimitiveValue lhs_const = to_const(lhs);
		arPrimitiveValue rhs_const = to_const(rhs);
		arPrimitiveValue_div(&lhs_const, &rhs_const);

		auto expected = lhs / rhs;

		if (std::is_same<decltype(expected), arInt8>::value) {
			assert_equals(lhs_const.i8, (arInt8)expected);
		}
		else if (std::is_same<decltype(expected), arUint8>::value) {
			assert_equals(lhs_const.ui8, (arUint8)expected);
		}
		else if (std::is_same<decltype(expected), arInt16>::value) {
			assert_equals(lhs_const.i16, (arInt16)expected);
		}
		else if (std::is_same<decltype(expected), arUint16>::value) {
			assert_equals(lhs_const.ui16, (arUint16)expected);
		}
		else if (std::is_same<decltype(expected), arInt32>::value) {
			assert_equals(lhs_const.i32, (arInt32)expected);
		}
		else if (std::is_same<decltype(expected), arUint32>::value) {
			assert_equals(lhs_const.ui32, (arUint32)expected);
		}
		else if (std::is_same<decltype(expected), arInt64>::value) {
			assert_equals(lhs_const.i64, (arInt64)expected);
		}
		else if (std::is_same<decltype(expected), arUint64>::value) {
			assert_equals(lhs_const.ui64, (arUint64)expected);
		}
		else if (std::is_same<decltype(expected), arFloat32>::value) {
			assert_equals(lhs_const.f32, (arFloat32)expected);
		}
		else if (std::is_same<decltype(expected), arFloat64>::value) {
			assert_equals(lhs_const.f64, (arFloat64)expected);
		}

		assert_equals(lhs_const.type, (arInt32)props1_of(expected));
	}

	void operator()()
	{
		TEST(ldc_T<arInt8>(12));
		TEST(ldc_T<arUint8>(UINT8_MAX - 10));
		TEST(ldc_T<arInt16>(INT16_MAX - 10));
		TEST(ldc_T<arUint16>(UINT16_MAX - 10));
		TEST(ldc_T<arInt32>(INT32_MAX - 1234));
		TEST(ldc_T<arUint32>(UINT32_MAX - 1234));
		TEST(ldc_T<arInt64>(INT64_MAX - 123456));
		TEST(ldc_T<arUint64>(UINT64_MAX - 123456));
		TEST(ldc_T<arFloat32>(123.456f));
		TEST(ldc_T<arFloat64>(12345.6789));

		TEST(arPrimitiveValue_add_T((arInt8)-123, (arInt8)54));
		TEST(arPrimitiveValue_add_T((arInt8)-123, (arUint8)244));
		TEST(arPrimitiveValue_add_T((arInt8)-123, (arInt16)1234));
		TEST(arPrimitiveValue_add_T((arInt8)-123, (arUint16)30000));
		TEST(arPrimitiveValue_add_T((arInt8)-123, (arInt32)34768));
		TEST(arPrimitiveValue_add_T((arInt8)-123, (arUint32)INT32_MAX + 100u));
		TEST(arPrimitiveValue_add_T((arInt8)-123, (arInt64)UINT32_MAX + 100ll));
		TEST(arPrimitiveValue_add_T((arInt8)-123, (arUint64)INT64_MAX + 100ull));
		TEST(arPrimitiveValue_add_T((arInt8)-123, 123.12f));
		TEST(arPrimitiveValue_add_T((arInt8)-123, 123.12));

		TEST(arPrimitiveValue_add_T((arUint8)129, (arInt8)-12));
		TEST(arPrimitiveValue_add_T((arUint8)129, (arUint8)244));
		TEST(arPrimitiveValue_add_T((arUint8)129, (arInt16)1234));
		TEST(arPrimitiveValue_add_T((arUint8)129, (arUint16)30000));
		TEST(arPrimitiveValue_add_T((arUint8)129, (arInt32)34768));
		TEST(arPrimitiveValue_add_T((arUint8)129, (arUint32)INT32_MAX + 100u));
		TEST(arPrimitiveValue_add_T((arUint8)129, (arInt64)UINT32_MAX + 100ll));
		TEST(arPrimitiveValue_add_T((arUint8)129, (arUint64)INT64_MAX + 100ull));
		TEST(arPrimitiveValue_add_T((arUint8)129, 123.12f));
		TEST(arPrimitiveValue_add_T((arInt8)129, 123.12));

		TEST(arPrimitiveValue_sub_T((arUint8)129, (arInt8)-12));
		TEST(arPrimitiveValue_sub_T((arUint8)129, (arUint8)244));
		TEST(arPrimitiveValue_sub_T((arUint8)129, (arInt16)1234));
		TEST(arPrimitiveValue_sub_T((arUint8)129, (arUint16)30000));
		TEST(arPrimitiveValue_sub_T((arUint8)129, (arInt32)34768));
		TEST(arPrimitiveValue_sub_T((arUint8)129, (arUint32)INT32_MAX + 100u));
		TEST(arPrimitiveValue_sub_T((arUint8)129, (arInt64)UINT32_MAX + 100ll));
		TEST(arPrimitiveValue_sub_T((arUint8)129, (arUint64)INT64_MAX + 100ull));
		TEST(arPrimitiveValue_sub_T((arUint8)129, 123.12f));
		TEST(arPrimitiveValue_sub_T((arInt8)129, 123.12));

		TEST(arPrimitiveValue_mult_T((arInt8)-2, (arInt8)26));
		TEST(arPrimitiveValue_mult_T((arInt8)-2, (arUint8)90));
		TEST(arPrimitiveValue_mult_T((arInt8)-2, (arInt16)1234));
		TEST(arPrimitiveValue_mult_T((arInt8)-2, (arUint16)10000));
		TEST(arPrimitiveValue_mult_T((arInt8)-2, (arInt32)24768));
		TEST(arPrimitiveValue_mult_T((arInt8)-2, (arUint32)INT32_MAX - 100u));
		TEST(arPrimitiveValue_mult_T((arInt8)-2, (arInt64)UINT32_MAX - 100ll));
		TEST(arPrimitiveValue_mult_T((arInt8)-2, (arUint64)INT64_MAX - 100ull));
		TEST(arPrimitiveValue_mult_T((arInt8)-2, 123.12f));
		TEST(arPrimitiveValue_mult_T((arInt8)-2, 123.12));

		TEST(arPrimitiveValue_div_T((arInt8)10, (arInt8)-2));
		TEST(arPrimitiveValue_div_T((arInt8)200, (arUint8)2));
		TEST(arPrimitiveValue_div_T((arInt8)50, (arInt16)5));
		TEST(arPrimitiveValue_div_T((arInt8)127, (arUint16)9));
		TEST(arPrimitiveValue_div_T((arInt8)40, (arInt32)24768));
		TEST(arPrimitiveValue_div_T((arInt8)70, (arUint32)30));
		TEST(arPrimitiveValue_div_T((arInt8)90, (arInt64)30ll));
		TEST(arPrimitiveValue_div_T((arInt8)-100, (arUint64)50ull));
		TEST(arPrimitiveValue_div_T((arInt8)-2, 50.0f));
		TEST(arPrimitiveValue_div_T((arInt8)50, -100.0f));

		TEST(arPrimitiveValue_add_T((arInt32)1234, (arInt32)5678));
	}
};

struct suite_builder_locals : utils_vmp_vmi
{
	void locals()
	{
		// Create the main package
		auto main_package = arB_package_newsz("main", 4);
		arBuilder_add_package(pipeline, main_package);

		// Create the Get function and add two integer types
		auto get = arB_func_newsz("Get", 3);
		auto get_ret1 = arB_func_new_return(get, get_type<arInt32>());
		auto get_local1 = arB_func_new_local(get, get_type<arInt32>());
		arB_local_set_namesz(get_local1, "i", 1);
		arB_package_add_func(main_package, get);

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
		arB_func_body_begin(get);
		arB_func_add_instr(get, arB_instr_ldc(get_type<arInt32>(), to_const((arInt32)100)));
		arB_func_add_instr(get, arB_instr_stl(get_local1));
		arB_func_add_instr(get, arB_instr_ldl(get_local1));
		arB_func_add_instr(get, arB_instr_ldc(get_type<arInt32>(), to_const((arInt32)1)));
		arB_func_add_instr(get, arB_instr_add(get_type<arInt32>()));
		arB_func_add_instr(get, arB_instr_stl(get_local1));
		arB_func_add_instr(get, arB_instr_ldl(get_local1));
		arB_func_add_instr(get, arB_instr_ret());
		arB_func_body_end(get);

		compile();

		invoke("Get");

		verify_stack_size(sizeof(arInt32));
		verify_value(pop_value<arInt32>(), 100 + 1);
	}

	template<typename T>
	void ldl_a_T(T value)
	{
		// Create the main package
		auto main_package = arB_package_newsz("main", 4);
		arBuilder_add_package(pipeline, main_package);

		// Create the Get function and add two integer types
		auto inner_get = arB_func_newsz("innerGet", 8);
		auto inner_get_arg1 = arB_func_new_arg(inner_get, get_type<T*>());
		arB_package_add_func(main_package, inner_get);

		// {
		//	lda 0
		//	ldc_T 100
		//	stunref int32
		//	ret
		// }
		arB_func_body_begin(inner_get);
		arB_func_add_instr(inner_get, arB_instr_lda(inner_get_arg1));
		arB_func_add_instr(inner_get, arB_instr_ldc(get_type<T>(), to_const((T)value)));
		arB_func_add_instr(inner_get, arB_instr_sturef(get_type<T>()));
		arB_func_add_instr(inner_get, arB_instr_ret());
		arB_func_body_end(inner_get);

		// Create the Get function and add two integer types
		auto get = arB_func_newsz("Get", 3);
		auto get_ret1 = arB_func_new_return(get, get_type<T>());
		auto get_local1 = arB_func_new_local(get, get_type<T>());
		arB_local_set_namesz(get_local1, "i", 1);
		arB_package_add_func(main_package, get);

		// {
		//	locals (i T)
		//	ldl_a 0
		//	call innerGet(*T)()
		//  frees *T
		//	ldl 0
		//	ret
		// }
		arB_func_body_begin(get);
		arB_func_add_instr(get, arB_instr_ldl_a(get_local1));
		arB_func_add_instr(get, arB_instr_call(inner_get));
		arB_func_add_instr(get, arB_instr_frees(get_type<T*>()));
		arB_func_add_instr(get, arB_instr_ldl(get_local1));
		arB_func_add_instr(get, arB_instr_ret());
		arB_func_body_end(get);

		compile();

		invoke("Get");

		verify_stack_size(sizeof(T));
		verify_value(pop_value<T>(), (T)value);
	}

	void operator()()
	{
		TEST(locals());

		TEST(ldl_a_T<arInt8>((arInt8)-120));
		TEST(ldl_a_T<arInt16>(INT16_MAX - 100));
		TEST(ldl_a_T<arInt32>((arInt32)(INT32_MAX * 0.98)));
		TEST(ldl_a_T<arInt64>(INT64_MAX - INT16_MAX));
		TEST(ldl_a_T<arFloat32>(123.45f));
		TEST(ldl_a_T<arFloat64>(1234.4567));
	}
};

struct suite_builder_globals : utils_vmp_vmi
{
	void globals()
	{
		// Create the main package
		auto main_package = arB_package_newsz("main", 4);
		arBuilder_add_package(pipeline, main_package);
		auto global = arB_global_new();
		arB_global_set_type(global, get_type<arInt32>());
		arB_package_add_global(main_package, global);

		// Create the Set function that sets the global variable
		auto set = arB_func_newsz("Set", 3);
		auto set_arg1 = arB_func_new_arg(set, get_type<arInt32>());
		arB_package_add_func(main_package, set);

		// {
		//	lda 0
		//	stg <var>
		//	ret
		// }
		arB_func_body_begin(set);
		arB_func_add_instr(set, arB_instr_lda(set_arg1));
		arB_func_add_instr(set, arB_instr_stg(global));
		arB_func_add_instr(set, arB_instr_ret());
		arB_func_body_end(set);

		// Create the Get function that returns the global variable
		auto get = arB_func_newsz("Get", 3);
		arB_func_new_return(get, get_type<arInt32>());
		arB_package_add_func(main_package, get);

		// {
		//	ldg <var>
		//	ret
		// }
		arB_func_body_begin(get);
		arB_func_add_instr(get, arB_instr_ldg(global));
		arB_func_add_instr(get, arB_instr_ret());
		arB_func_body_end(get);

		compile();

		push_value<arInt32>(123);
		invoke("Set");

		verify_stack_size(sizeof(arInt32));
		verify_value(pop_value<arInt32>(), 123);

		invoke("Get");
		verify_stack_size(sizeof(arInt32));
		verify_value(pop_value<arInt32>(), 123);
	}

	void operator()()
	{
		TEST(globals());
	}
};

struct suite_builder_convertions : utils_vmp_vmi
{
	template<typename FROM, typename TO>
	void conv_T_T(const FROM from, const TO to)
	{
		// Create the main package
		auto main_package = arB_package_newsz("main", 4);
		arBuilder_add_package(pipeline, main_package);

		// Create the Get function and add two integer types
		auto get = arB_func_newsz("Get", 3);
		auto get_arg1 = arB_func_new_arg(get, get_type<FROM>());
		arB_func_new_return(get, get_type<TO>());
		arB_package_add_func(main_package, get);

		// {
		//	lda 0
		//  conv_%FROM_%TO
		//	ret
		// }
		arB_func_body_begin(get);
		arB_func_add_instr(get, arB_instr_lda(get_arg1));
		arB_func_add_instr(get, arB_instr_conv(get_type<FROM>(), get_type<TO>()));
		arB_func_add_instr(get, arB_instr_ret());
		arB_func_body_end(get);

		compile();

		push_value<FROM>(from);
		invoke("Get");

		verify_stack_size(sizeof(FROM) + sizeof(TO));
		verify_value(pop_value<TO>(), (TO)to);
		verify_value(pop_value<FROM>(), (FROM)from);
	}

	void operator()()
	{
		TEST(conv_T_T((arInt32)INT32_MAX, (arInt16)INT32_MAX));
		TEST(conv_T_T((arInt16)INT16_MAX, (arInt32)INT16_MAX));
	}
};

struct suite_builder_add_sub_mult_div : utils_vmp_vmi
{
	template<typename T>
	void add_test(T lhs, T rhs)
	{
		// Create the main package
		auto main_package = arB_package_newsz("main", 4);
		arBuilder_add_package(pipeline, main_package);

		// Create the Add function and add two integer types
		auto add = arB_func_newsz("Add", 3);
		auto arg1 = arB_func_new_arg(add, get_type<T>());
		arB_arg_set_namesz(arg1, "lhs", 3);
		auto arg2 = arB_func_new_arg(add, get_type<T>());
		arB_arg_set_namesz(arg2, "rhs", 3);
		auto ret1 = arB_func_new_return(add, get_type<T>());
		arB_package_add_func(main_package, add);

		// {
		//	lda 0
		//	lda 1
		//	add T
		//	lda 1
		//	add T
		//	ret
		// }
		arB_func_body_begin(add);
		arB_func_add_instr(add, arB_instr_lda(arg1));
		arB_func_add_instr(add, arB_instr_lda(arg2));
		arB_func_add_instr(add, arB_instr_add(get_type<T>()));
		arB_func_add_instr(add, arB_instr_lda(arg2));
		arB_func_add_instr(add, arB_instr_add(get_type<T>()));
		arB_func_add_instr(add, arB_instr_ret());
		arB_func_body_end(add);

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
		auto main_package = arB_package_newsz("main", 4);
		arBuilder_add_package(pipeline, main_package);

		// Create the Add function and add two integer types
		auto add = arB_func_newsz("Add", 3);
		auto arg1 = arB_func_new_arg(add, get_type<T>());
		arB_arg_set_namesz(arg1, "lhs", 3);
		auto arg2 = arB_func_new_arg(add, get_type<T>());
		arB_arg_set_namesz(arg2, "rhs", 3);
		auto ret1 = arB_func_new_return(add, get_type<T>());
		arB_package_add_func(main_package, add);

		// {
		//	lda 0
		//	lda 1
		//	sub T
		//	lda 1
		//	sub T
		//	ret
		// }
		arB_func_body_begin(add);
		arB_func_add_instr(add, arB_instr_lda(arg1));
		arB_func_add_instr(add, arB_instr_lda(arg2));
		arB_func_add_instr(add, arB_instr_sub(get_type<T>()));
		arB_func_add_instr(add, arB_instr_lda(arg2));
		arB_func_add_instr(add, arB_instr_sub(get_type<T>()));
		arB_func_add_instr(add, arB_instr_ret());
		arB_func_body_end(add);

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
		auto main_package = arB_package_newsz("main", 4);
		arBuilder_add_package(pipeline, main_package);

		// Create the Add function and add two integer types
		auto func = arB_func_newsz("Mul", 3);
		auto arg1 = arB_func_new_arg(func, get_type<T>());
		arB_arg_set_namesz(arg1, "lhs", 3);
		auto arg2 = arB_func_new_arg(func, get_type<T>());
		arB_arg_set_namesz(arg2, "rhs", 3);
		auto ret1 = arB_func_new_return(func, get_type<T>());
		arB_package_add_func(main_package, func);

		// {
		//	lda 0
		//	lda 1
		//	mul T
		//	ret
		// }
		arB_func_body_begin(func);
		arB_func_add_instr(func, arB_instr_lda(arg1));
		arB_func_add_instr(func, arB_instr_lda(arg2));
		arB_func_add_instr(func, arB_instr_mul(get_type<T>()));
		arB_func_add_instr(func, arB_instr_ret());
		arB_func_body_end(func);

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
		auto main_package = arB_package_newsz("main", 4);
		arBuilder_add_package(pipeline, main_package);

		// Create the Add function and add two integer types
		auto func = arB_func_newsz("Div", 3);
		auto arg1 = arB_func_new_arg(func, get_type<T>());
		arB_arg_set_namesz(arg1, "lhs", 3);
		auto arg2 = arB_func_new_arg(func, get_type<T>());
		arB_arg_set_namesz(arg2, "rhs", 3);
		auto ret1 = arB_func_new_return(func, get_type<T>());
		arB_package_add_func(main_package, func);

		// {
		//	lda 0
		//	lda 1
		//	mul T
		//	ret
		// }
		arB_func_body_begin(func);
		arB_func_add_instr(func, arB_instr_lda(arg1));
		arB_func_add_instr(func, arB_instr_lda(arg2));
		arB_func_add_instr(func, arB_instr_div(get_type<T>()));
		arB_func_add_instr(func, arB_instr_ret());
		arB_func_body_end(func);

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
		TEST(add_test<arInt8>(1, 5));
		TEST(add_test<arUint8>(1, 5));
		TEST(add_test<arInt16>(10, INT8_MAX));
		TEST(add_test<arUint16>(10, INT8_MAX));
		TEST(add_test<arInt32>(INT16_MAX, 20));
		TEST(add_test<arUint32>(INT16_MAX, INT16_MAX));
		TEST(add_test<arInt64>(102, INT32_MAX));
		TEST(add_test<arUint64>(INT32_MAX, INT32_MAX));
		TEST(add_test<arFloat32>(1.0f, 20.0f));
		TEST(add_test<arFloat64>(-10.0, 203.0));

		TEST(sub_T<arInt8>(1, 5));
		TEST(sub_T<arUint8>(1, 5));
		TEST(sub_T<arInt16>(10, INT8_MAX));
		TEST(sub_T<arUint16>(10, INT8_MAX));
		TEST(sub_T<arInt32>(INT16_MAX, 20));
		TEST(sub_T<arUint32>(INT16_MAX, INT16_MAX));
		TEST(sub_T<arInt64>(102, INT32_MAX));
		TEST(sub_T<arUint64>(INT32_MAX, INT32_MAX));
		TEST(sub_T<arFloat32>(1.0f, 20.0f));
		TEST(sub_T<arFloat64>(-10.0, 203.0));

		TEST(mul_T<arInt8>(2, 5));
		TEST(mul_T<arUint8>(3, 5));
		TEST(mul_T<arInt16>(INT8_MAX, 10));
		TEST(mul_T<arUint16>(INT16_MAX, 2));
		TEST(mul_T<arInt32>(INT16_MAX, 3));
		TEST(mul_T<arUint32>(INT16_MAX, 4));
		TEST(mul_T<arInt64>(INT32_MAX, 2));
		TEST(mul_T<arUint64>(INT32_MAX, 4));
		TEST(mul_T<arFloat32>(1.0f, 20.0f));
		TEST(mul_T<arFloat64>(-10.0, 203.0));

		TEST(div_T<arInt8>(2, 5));
		TEST(div_T<arUint8>(3, 5));
		TEST(div_T<arInt16>(INT8_MAX, 10));
		TEST(div_T<arUint16>(INT16_MAX, 2));
		TEST(div_T<arInt32>(INT16_MAX, 3));
		TEST(div_T<arUint32>(INT16_MAX, 4));
		TEST(div_T<arInt64>(INT32_MAX, 2));
		TEST(div_T<arUint64>(INT32_MAX, 4));
		TEST(div_T<arFloat32>(1.0f, 20.0f));
		TEST(div_T<arFloat64>(-10.0, 203.0));
	}
};

struct suite_builder_bits : utils_vmp_vmi
{
	template<typename T>
	void bit_not_T(T value)
	{
		// Create the main package
		auto main_package = arB_package_newsz("main", 4);
		arBuilder_add_package(pipeline, main_package);

		// Create the Add function and add two integer types
		auto func = arB_func_newsz("BitNot", 6);
		auto arg1 = arB_func_new_arg(func, get_type<T>());
		arB_arg_set_namesz(arg1, "lhs", 3);
		auto ret1 = arB_func_new_return(func, get_type<T>());
		arB_package_add_func(main_package, func);

		// {
		//	lda 0
		//	bit not T
		//	ret
		// }
		arB_func_body_begin(func);
		arB_func_add_instr(func, arB_instr_lda(arg1));
		arB_func_add_instr(func, arB_instr_bit_not(get_type<T>()));
		arB_func_add_instr(func, arB_instr_ret());
		arB_func_body_end(func);

		compile();

		push_value<T>(value);
		invoke("BitNot");

		verify_stack_size(sizeof(T) * 2);
		verify_value(pop_value<T>(), (T)(~value));
		verify_value(pop_value<T>(), (T)(value));
	}

	void operator()()
	{
		TEST(bit_not_T<arInt8>(1));
		TEST(bit_not_T<arUint8>(15));
		TEST(bit_not_T<arInt16>(10));
		TEST(bit_not_T<arUint16>(INT8_MAX));
		TEST(bit_not_T<arInt32>(INT16_MAX));
		TEST(bit_not_T<arUint32>((arUint32)INT16_MAX + 10));
		TEST(bit_not_T<arInt64>((arInt64)((arInt64)INT32_MAX + 5i64)));
		TEST(bit_not_T<arUint64>((arUint64)(INT32_MAX + 1000i64)));
	}
};

struct suite_builder_memory : utils_vmp_vmi
{
	void allocs_const()
	{
		// Create the main package
		auto main_package = arB_package_newsz("main", 4);
		arBuilder_add_package(pipeline, main_package);

		// Create the Get function and add two integer types
		auto _do = arB_func_newsz("Do", 2);
		arB_package_add_func(main_package, _do);

		// {
		//	allocs 4
		//	frees 4
		//	ret
		// }
		arB_func_body_begin(_do);
		arB_func_add_instr(_do, arB_instr_allocs_const(4));
		arB_func_add_instr(_do, arB_instr_frees_const(4));
		arB_func_add_instr(_do, arB_instr_ret());
		arB_func_body_end(_do);

		compile();

		invoke("Do");

		verify_stack_size(0);
	}

	template<typename T>
	void allocs_T()
	{
		// Create the main package
		auto main_package = arB_package_newsz("main", 4);
		arBuilder_add_package(pipeline, main_package);

		auto type = get_type<T>();

		// Create the Get function and add two integer types
		auto _do = arB_func_newsz("Do", 2);
		arB_package_add_func(main_package, _do);

		// {
		//	allocs T
		//	frees T
		//	ret
		// }
		arB_func_body_begin(_do);
		arB_func_add_instr(_do, arB_instr_allocs(type));
		arB_func_add_instr(_do, arB_instr_frees(type));
		arB_func_add_instr(_do, arB_instr_ret());
		arB_func_body_end(_do);

		compile();

		invoke("Do");

		verify_stack_size(0);
	}

	void alloch_const()
	{
		// Create the main package
		auto main_package = arB_package_newsz("main", 4);
		arBuilder_add_package(pipeline, main_package);

		// Create the Get function and add two integer types
		auto _do = arB_func_newsz("Do", 2);
		arB_package_add_func(main_package, _do);

		// {
		//	alloch 4
		//	freeh
		//	ret
		// }
		arB_func_body_begin(_do);
		arB_func_add_instr(_do, arB_instr_alloch_const(4));
		arB_func_add_instr(_do, arB_instr_freeh());
		arB_func_add_instr(_do, arB_instr_ret());
		arB_func_body_end(_do);

		compile();

		invoke("Do");

		verify_stack_size(0);
	}

	template<typename T>
	void alloch_T()
	{
		// Create the main package
		auto main_package = arB_package_newsz("main", 4);
		arBuilder_add_package(pipeline, main_package);

		auto type = get_type<T>();

		// Create the Get function and add two integer types
		auto _do = arB_func_newsz("Do", 2);
		arB_package_add_func(main_package, _do);

		// {
		//	ldc_i4 sizeof(T)
		//	alloch T
		//	freeh T
		//	ret
		// }
		arB_func_body_begin(_do);
		arB_func_add_instr(_do, arB_instr_ldc(get_type<arInt32>(), to_const((arInt32)sizeof(T))));
		arB_func_add_instr(_do, arB_instr_alloch(get_type<arInt32>()));
		arB_func_add_instr(_do, arB_instr_freeh());
		arB_func_add_instr(_do, arB_instr_ret());
		arB_func_body_end(_do);

		compile();

		invoke("Do");

		verify_stack_size(0);
	}

	template<typename T>
	void alloch_type_T()
	{
		// Create the main package
		auto main_package = arB_package_newsz("main", 4);
		arBuilder_add_package(pipeline, main_package);

		auto type = get_type<T>();

		// Create the Get function and add two integer types
		auto _do = arB_func_newsz("Do", 2);
		arB_package_add_func(main_package, _do);

		// {
		//	alloch T
		//	freeh T
		//	ret
		// }
		arB_func_body_begin(_do);
		arB_func_add_instr(_do, arB_instr_alloch_type(type));
		arB_func_add_instr(_do, arB_instr_freeh());
		arB_func_add_instr(_do, arB_instr_ret());
		arB_func_body_end(_do);

		compile();

		invoke("Do");

		verify_stack_size(0);
	}

	void operator()()
	{
		TEST(allocs_const());

		TEST(allocs_T<arInt8>());
		TEST(allocs_T<arUint8>());
		TEST(allocs_T<arInt16>());
		TEST(allocs_T<arUint16>());
		TEST(allocs_T<arInt32>());
		TEST(allocs_T<arUint32>());
		TEST(allocs_T<arInt64>());
		TEST(allocs_T<arUint64>());
		TEST(allocs_T<arFloat32>());
		TEST(allocs_T<arFloat64>());

		TEST(alloch_const());

		TEST(alloch_T<arInt8>());
		TEST(alloch_T<arUint8>());
		TEST(alloch_T<arInt16>());
		TEST(alloch_T<arUint16>());
		TEST(alloch_T<arInt32>());
		TEST(alloch_T<arUint32>());
		TEST(alloch_T<arInt64>());
		TEST(alloch_T<arUint64>());
		TEST(alloch_T<arFloat32>());
		TEST(alloch_T<arFloat64>());

		TEST(alloch_type_T<arInt8>());
		TEST(alloch_type_T<arUint8>());
		TEST(alloch_type_T<arInt16>());
		TEST(alloch_type_T<arUint16>());
		TEST(alloch_type_T<arInt32>());
		TEST(alloch_type_T<arUint32>());
		TEST(alloch_type_T<arInt64>());
		TEST(alloch_type_T<arUint64>());
		TEST(alloch_type_T<arFloat32>());
		TEST(alloch_type_T<arFloat64>());
	}
};

struct suite_builder_element : utils_vmp_vmi
{
	template<typename T>
	void stelem_T(const array<T, 2>& values)
	{
		// Create the main package
		auto main_package = arB_package_newsz("main", 4);
		arBuilder_add_package(pipeline, main_package);

		const auto array_type_name = string("[2]") + string(name<T>());

		// Create array type
		auto array_type = arB_type_from_props(
			arB_type_props_get(
				arBuilder_get_string(pipeline, array_type_name.c_str(), (arInt32)array_type_name.length()),
				sizeof(T[2]),
				arB_TYPE_FLAGS_ARRAY,
				props1<T>(),
				get_type<T>()
			)
		);
		arB_package_add_type(vm_package(), array_type);

		// Create the Get function and add two integer types
		auto get = arB_func_newsz("Get", 3);
		arB_func_new_return(get, array_type);
		auto get_local_1 = arB_func_new_local(get, array_type);
		arB_package_add_func(main_package, get);

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
		arB_func_body_begin(get);
		arB_func_add_instr(get, arB_instr_ldl_a(get_local_1));
		arB_func_add_instr(get, arB_instr_ldc(get_type<arInt32>(), to_const(0)));
		arB_func_add_instr(get, arB_instr_ldc(get_type<T>(), to_const((T)values[0])));
		arB_func_add_instr(get, arB_instr_stelem(array_type));
		arB_func_add_instr(get, arB_instr_ldl_a(get_local_1));
		arB_func_add_instr(get, arB_instr_ldc(get_type<arInt32>(), to_const(1)));
		arB_func_add_instr(get, arB_instr_ldc(get_type<T>(), to_const((T)values[1])));
		arB_func_add_instr(get, arB_instr_stelem(array_type));
		arB_func_add_instr(get, arB_instr_ldl(get_local_1));
		arB_func_add_instr(get, arB_instr_ret());
		arB_func_body_end(get);

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
		auto main_package = arB_package_newsz("main", 4);
		arBuilder_add_package(pipeline, main_package);

		// Create array type
		auto ptr_type = get_type(string(ptr<T>()));

		// Create the Get function and add two integer types
		auto get = arB_func_newsz("Get", 3);
		auto get_arg1 = arB_func_new_arg(get, ptr_type);
		arB_package_add_func(main_package, get);

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
		arB_func_body_begin(get);
		arB_func_add_instr(get, arB_instr_lda(get_arg1));
		arB_func_add_instr(get, arB_instr_ldc(get_type<arInt32>(), to_const(0)));
		arB_func_add_instr(get, arB_instr_ldc(get_type<T>(), to_const((T)values[0])));
		arB_func_add_instr(get, arB_instr_stelem(ptr_type));
		arB_func_add_instr(get, arB_instr_lda(get_arg1));
		arB_func_add_instr(get, arB_instr_ldc(get_type<arInt32>(), to_const(1)));
		arB_func_add_instr(get, arB_instr_ldc(get_type<T>(), to_const((T)values[1])));
		arB_func_add_instr(get, arB_instr_stelem(ptr_type));
		arB_func_add_instr(get, arB_instr_ret());
		arB_func_body_end(get);

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
		auto main_package = arB_package_newsz("main", 4);
		arBuilder_add_package(pipeline, main_package);

		const auto array_type_name = string("[2]") + string(name<T>());

		// Create array type
		auto array_type = arB_type_from_props(
			arB_type_props_get(
				arBuilder_get_string(pipeline, array_type_name.c_str(), (arInt32)array_type_name.length()),
				sizeof(T[2]),
				arB_TYPE_FLAGS_ARRAY,
				props1<T>(),
				get_type<T>()
			)
		);
		arB_package_add_type(vm_package(), array_type);

		// Create the Get function and add two integer types
		auto get = arB_func_newsz("Get", 3);
		auto get_arg1 = arB_func_new_arg(get, array_type);
		auto get_ret1 = arB_func_new_return(get, get_type<T>());
		auto get_ret2 = arB_func_new_return(get, get_type<T>());
		arB_package_add_func(main_package, get);

		// {
		//	lda_a 0
		//	ldc_i4 0
		//	ldelem [2]T
		//	lda_a 0
		//	ldc_i4 1
		//	ldelem [2]T
		//	ret
		// }
		arB_func_body_begin(get);
		arB_func_add_instr(get, arB_instr_lda_a(get_arg1));
		arB_func_add_instr(get, arB_instr_ldc(get_type<arInt32>(), to_const(0)));
		arB_func_add_instr(get, arB_instr_ldelem(array_type));
		arB_func_add_instr(get, arB_instr_lda_a(get_arg1));
		arB_func_add_instr(get, arB_instr_ldc(get_type<arInt32>(), to_const(1)));
		arB_func_add_instr(get, arB_instr_ldelem(array_type));
		arB_func_add_instr(get, arB_instr_ret());
		arB_func_body_end(get);

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
		auto main_package = arB_package_newsz("main", 4);
		arBuilder_add_package(pipeline, main_package);

		// Create array type
		auto ptr_type = get_type(string(ptr<T>()));

		// Create the Get function and add two integer types
		auto get = arB_func_newsz("Get", 3);
		auto get_arg1 = arB_func_new_arg(get, ptr_type);
		auto get_ret1 = arB_func_new_return(get, get_type<T>());
		auto get_ret2 = arB_func_new_return(get, get_type<T>());
		arB_package_add_func(main_package, get);

		// {
		//	lda 0
		//	ldc_i4 0
		//	ldelem *T
		//	lda 0
		//	ldc_i4 1
		//	ldelem *T
		//	ret
		// }
		arB_func_body_begin(get);
		arB_func_add_instr(get, arB_instr_lda(get_arg1));
		arB_func_add_instr(get, arB_instr_ldc(get_type<arInt32>(), to_const(0)));
		arB_func_add_instr(get, arB_instr_ldelem(ptr_type));
		arB_func_add_instr(get, arB_instr_lda(get_arg1));
		arB_func_add_instr(get, arB_instr_ldc(get_type<arInt32>(), to_const(1)));
		arB_func_add_instr(get, arB_instr_ldelem(ptr_type));
		arB_func_add_instr(get, arB_instr_ret());
		arB_func_body_end(get);

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
		TEST(stelem_T<arInt8>({ 1, -1 }));
		TEST(stelem_T<arUint8>({ INT8_MAX + 1, UINT8_MAX - 1 }));
		TEST(stelem_T<arInt16>({ UINT8_MAX + 1, INT16_MAX - 100 }));
		TEST(stelem_T<arUint16>({ INT16_MAX + 1, UINT16_MAX - 1 }));
		TEST(stelem_T<arInt32>({ UINT16_MAX + 1, INT32_MAX - 100 }));
		TEST(stelem_T<arUint32>({ INT32_MAX + 1u, UINT32_MAX - 100u }));
		TEST(stelem_T<arInt64>({ UINT32_MAX + 1ll, INT64_MAX - 100i64 }));
		TEST(stelem_T<arUint64>({ (arUint64)INT64_MAX + 1i64, (arUint64)UINT64_MAX - 100i64 }));
		TEST(stelem_T<arFloat32>({ 123.43f, 657.123f }));
		TEST(stelem_T<arFloat64>({ 123.4345, 657.12312 }));

		TEST(stelem_ptr_T<arInt8>({ 1, -1 }));
		TEST(stelem_ptr_T<arUint8>({ INT8_MAX + 1, UINT8_MAX - 1 }));
		TEST(stelem_ptr_T<arInt16>({ UINT8_MAX + 1, INT16_MAX - 100 }));
		TEST(stelem_ptr_T<arUint16>({ INT16_MAX + 1, UINT16_MAX - 1 }));
		TEST(stelem_ptr_T<arInt32>({ UINT16_MAX + 1, INT32_MAX - 100 }));
		TEST(stelem_ptr_T<arUint32>({ INT32_MAX + 1u, UINT32_MAX - 100u }));
		TEST(stelem_ptr_T<arInt64>({ UINT32_MAX + 1ll, INT64_MAX - 100ll }));
		TEST(stelem_ptr_T<arUint64>({ (arUint64)INT64_MAX + 1i64, (arUint64)UINT64_MAX - 100i64 }));
		TEST(stelem_ptr_T<arFloat32>({ 123.43f, 657.123f }));
		TEST(stelem_ptr_T<arFloat64>({ 123.4345, 657.12312 }));

		TEST(ldelem_T<arInt8>({ 1, -1 }));
		TEST(ldelem_T<arUint8>({ INT8_MAX + 1, UINT8_MAX - 1 }));
		TEST(ldelem_T<arInt16>({ UINT8_MAX + 1, INT16_MAX - 100 }));
		TEST(ldelem_T<arUint16>({ INT16_MAX + 1, UINT16_MAX - 1 }));
		TEST(ldelem_T<arInt32>({ UINT16_MAX + 1, INT32_MAX - 100 }));
		TEST(ldelem_T<arUint32>({ INT32_MAX + 1u, UINT32_MAX - 100u }));
		TEST(ldelem_T<arInt64>({ UINT32_MAX + 1ll, INT64_MAX - 100ll }));
		TEST(ldelem_T<arUint64>({ (arUint64)INT64_MAX + (arUint64)1, (arUint64)UINT64_MAX - (arUint64)100 }));
		TEST(ldelem_T<arFloat32>({ 123.43f, 657.123f }));
		TEST(ldelem_T<arFloat64>({ 123.4345, 657.12312 }));

		TEST(ldelem_ptr_T<arInt8>({ 1, -1 }));
		TEST(ldelem_ptr_T<arUint8>({ INT8_MAX + 1, UINT8_MAX - 1 }));
		TEST(ldelem_ptr_T<arInt16>({ UINT8_MAX + 1, INT16_MAX - 100 }));
		TEST(ldelem_ptr_T<arUint16>({ INT16_MAX + 1, UINT16_MAX - 1 }));
		TEST(ldelem_ptr_T<arInt32>({ UINT16_MAX + 1, INT32_MAX - 100 }));
		TEST(ldelem_ptr_T<arUint32>({ INT32_MAX + 1u, UINT32_MAX - 100u }));
		TEST(ldelem_ptr_T<arInt64>({ UINT32_MAX + 1ll, INT64_MAX - 100ll }));
		TEST(ldelem_ptr_T<arUint64>({ (arUint64)INT64_MAX + (arUint64)1, (arUint64)UINT64_MAX - (arUint64)100 }));
		TEST(ldelem_ptr_T<arFloat32>({ 123.43f, 657.123f }));
		TEST(ldelem_ptr_T<arFloat64>({ 123.4345, 657.12312 }));
	}
};

struct suite_builder_compares : utils_vmp_vmi
{
	void cgt(const arInt32 const_val)
	{
		// Create the main package
		auto main_package = arB_package_newsz("main", 4);
		arBuilder_add_package(pipeline, main_package);

		// Create the Get function and add two integer types
		auto get = arB_func_newsz("Get", 3);
		auto get_arg1 = arB_func_new_arg(get, get_type<arInt32>());
		arB_func_new_return(get, get_type<arInt32>());
		arB_package_add_func(main_package, get);

		// {
		//	lda 0
		//	ldc_i4 10
		//	cgt
		//	ret
		// }
		arB_func_body_begin(get);
		arB_func_add_instr(get, arB_instr_lda(get_arg1));
		arB_func_add_instr(get, arB_instr_ldc(get_type<arInt32>(), to_const(10)));
		arB_func_add_instr(get, arB_instr_cgt(get_type<arInt32>()));
		arB_func_add_instr(get, arB_instr_ret());
		arB_func_body_end(get);

		compile();

		push_value<arInt32>(const_val);
		invoke("Get");

		verify_stack_size(sizeof(arInt32) * 2);
		verify_value(pop_value<arInt32>(), const_val > 10 ? TRUE : FALSE);
		verify_value(pop_value<arInt32>(), const_val);
	}

	void clt(const arInt32 const_val)
	{
		// Create the main package
		auto main_package = arB_package_newsz("main", 4);
		arBuilder_add_package(pipeline, main_package);

		// Create the Get function and add two integer types
		auto get = arB_func_newsz("Get", 3);
		auto get_arg1 = arB_func_new_arg(get, get_type<arInt32>());
		arB_func_new_return(get, get_type<arInt32>());
		arB_package_add_func(main_package, get);

		// {
		//	lda 0
		//	ldc_i4 10
		//	clt
		//	ret
		// }
		arB_func_body_begin(get);
		arB_func_add_instr(get, arB_instr_lda(get_arg1));
		arB_func_add_instr(get, arB_instr_ldc(get_type<arInt32>(), to_const(10)));
		arB_func_add_instr(get, arB_instr_clt(get_type<arInt32>()));
		arB_func_add_instr(get, arB_instr_ret());
		arB_func_body_end(get);

		compile();

		push_value<arInt32>(const_val);
		invoke("Get");

		verify_stack_size(sizeof(arInt32) * 2);
		verify_value(pop_value<arInt32>(), const_val < 10 ? TRUE : FALSE);
		verify_value(pop_value<arInt32>(), const_val);
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

struct suite_builder_jumps : utils_vmp_vmi
{
	void jmpt(const arInt32 const_val)
	{
		// Create the main package
		auto main_package = arB_package_newsz("main", 4);
		arBuilder_add_package(pipeline, main_package);

		// Create the Get function and add two integer types
		auto get = arB_func_newsz("Get", 3);
		auto get_arg1 = arB_func_new_arg(get, get_type<arInt32>());
		arB_func_new_return(get, get_type<arInt32>());
		arB_package_add_func(main_package, get);

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
		arB_func_body_begin(get);
		auto marker = arB_func_new_marker(get);
		arB_func_add_instr(get, arB_instr_lda(get_arg1));
		arB_func_add_instr(get, arB_instr_ldc(get_type<arInt32>(), to_const(10)));
		arB_func_add_instr(get, arB_instr_clt(get_type<arInt32>()));
		arB_func_add_instr(get, arB_instr_jmpt(marker));
		arB_func_add_instr(get, arB_instr_ldc(get_type<arInt32>(), to_const(50)));
		arB_func_add_instr(get, arB_instr_ret());
		arB_marker_set_instr(marker,
			arB_func_add_instr(get, arB_instr_ldc(get_type<arInt32>(), to_const(150)))
		);
		arB_func_add_instr(get, arB_instr_ret());
		arB_func_body_end(get);

		compile();

		push_value<arInt32>(const_val);
		invoke("Get");

		verify_stack_size(sizeof(arInt32) * 2);
		verify_value(pop_value<arInt32>(), const_val < 10 ? 150 : 50);
		verify_value(pop_value<arInt32>(), const_val);
	}

	void jmpf(const arInt32 const_val)
	{
		// Create the main package
		auto main_package = arB_package_newsz("main", 4);
		arBuilder_add_package(pipeline, main_package);

		// Create the Get function and add two integer types
		auto get = arB_func_newsz("Get", 3);
		auto get_arg1 = arB_func_new_arg(get, get_type<arInt32>());
		arB_func_new_return(get, get_type<arInt32>());
		arB_package_add_func(main_package, get);

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
		arB_func_body_begin(get);
		auto marker = arB_func_new_marker(get);
		arB_func_add_instr(get, arB_instr_lda(get_arg1));
		arB_func_add_instr(get, arB_instr_ldc(get_type<arInt32>(), to_const(10)));
		arB_func_add_instr(get, arB_instr_clt(get_type<arInt32>()));
		arB_func_add_instr(get, arB_instr_jmpf(marker));
		arB_func_add_instr(get, arB_instr_ldc(get_type<arInt32>(), to_const(50)));
		arB_func_add_instr(get, arB_instr_ret());
		arB_marker_set_instr(marker,
			arB_func_add_instr(get, arB_instr_ldc(get_type<arInt32>(), to_const(150)))
		);
		arB_func_add_instr(get, arB_instr_ret());
		arB_func_body_end(get);

		compile();

		push_value<arInt32>(const_val);
		invoke("Get");

		verify_stack_size(sizeof(arInt32) * 2);
		verify_value(pop_value<arInt32>(), !(const_val < 10) ? 150 : 50);
		verify_value(pop_value<arInt32>(), const_val);
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

struct suite_builder_calls : utils_vmp_vmi
{
	void call()
	{
		// Create the main package
		auto main_package = arB_package_newsz("main", 4);
		arBuilder_add_package(pipeline, main_package);

		// Create the Add function and add two integer types
		auto add = arB_func_newsz("Add", 3);
		auto add_arg1 = arB_func_new_arg(add, get_type<arInt32>());
		arB_arg_set_namesz(add_arg1, "lhs", 3);
		auto add_arg2 = arB_func_new_arg(add, get_type<arInt32>());
		arB_arg_set_namesz(add_arg2, "rhs", 3);
		arB_func_new_return(add, get_type<arInt32>());
		arB_package_add_func(main_package, add);

		// {
		//	lda 0
		//	lda 1
		//	add int32
		//	ret
		// }
		arB_func_body_begin(add);
		arB_func_add_instr(add, arB_instr_lda(add_arg1));
		arB_func_add_instr(add, arB_instr_lda(add_arg2));
		arB_func_add_instr(add, arB_instr_add(get_type<arInt32>()));
		arB_func_add_instr(add, arB_instr_ret());
		arB_func_body_end(add);

		// Create the Get function and add two integer types
		auto get = arB_func_newsz("Get", 3);
		arB_func_new_return(get, get_type<arInt32>());
		auto get_local_1 = arB_func_new_local(get, get_type<arInt32>());
		arB_package_add_func(main_package, get);

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
		arB_func_body_begin(get);
		arB_func_add_instr(get, arB_instr_ldc(get_type<arInt32>(), to_const(const_val1)));
		arB_func_add_instr(get, arB_instr_ldc(get_type<arInt32>(), to_const(const_val2)));
		arB_func_add_instr(get, arB_instr_call(add));
		arB_func_add_instr(get, arB_instr_stl(get_local_1));
		arB_func_add_instr(get, arB_instr_frees(get_type<arInt32>()));
		arB_func_add_instr(get, arB_instr_frees(get_type<arInt32>()));
		arB_func_add_instr(get, arB_instr_ldl(get_local_1));
		arB_func_add_instr(get, arB_instr_ret());
		arB_func_body_end(get);

		compile();

		invoke("Get");

		verify_stack_size(sizeof(arInt32));
		verify_value(pop_value<arInt32>(), (arInt32)(const_val1 + const_val2));
	}

	static int callnative_add(arThread* t)
	{
		const auto rhs = *(arInt32*)arThread_getArg(t, 0);
		const auto lhs = *(arInt32*)arThread_getArg(t, 4);
		*(arInt32*)arThread_pushStack(t, sizeof(arInt32)) = lhs + rhs;
		return 0;
	}

	void callnative()
	{
		// Create the main package
		auto main_package = arB_package_newsz("main", 4);
		arBuilder_add_package(pipeline, main_package);

		// Create the Add function and add two integer types
		auto add = arB_func_newsz("Add", 3);
		auto add_arg1 = arB_func_new_arg(add, get_type<arInt32>());
		arB_arg_set_namesz(add_arg1, "lhs", 3);
		auto add_arg2 = arB_func_new_arg(add, get_type<arInt32>());
		arB_arg_set_namesz(add_arg2, "rhs", 3);
		arB_func_new_return(add, get_type<arInt32>());
		arB_func_set_native(add, &callnative_add);
		arB_package_add_func(main_package, add);

		// Create the Get function and add two integer types
		auto get = arB_func_newsz("Get", 3);
		arB_func_new_return(get, get_type<arInt32>());
		auto get_local_1 = arB_func_new_local(get, get_type<arInt32>());
		arB_package_add_func(main_package, get);

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
		arB_func_body_begin(get);
		arB_func_add_instr(get, arB_instr_ldc(get_type<arInt32>(), to_const(const_val1)));
		arB_func_add_instr(get, arB_instr_ldc(get_type<arInt32>(), to_const(const_val2)));
		arB_func_add_instr(get, arB_instr_callnative(add));
		arB_func_add_instr(get, arB_instr_stl(get_local_1));
		arB_func_add_instr(get, arB_instr_frees(get_type<arInt32>()));
		arB_func_add_instr(get, arB_instr_frees(get_type<arInt32>()));
		arB_func_add_instr(get, arB_instr_ldl(get_local_1));
		arB_func_add_instr(get, arB_instr_ret());
		arB_func_body_end(get);

		compile();

		invoke("Get");

		verify_stack_size(sizeof(arInt32));
		verify_value(pop_value<arInt32>(), (arInt32)(const_val1 + const_val2));
	}

	void calluref()
	{
		// Create the main package
		auto main_package = arB_package_newsz("main", 4);
		arBuilder_add_package(pipeline, main_package);

		// Create the Add function and add two integer types
		auto add = arB_func_newsz("Add", 3);
		auto add_arg1 = arB_func_new_arg(add, get_type<arInt32>());
		arB_arg_set_namesz(add_arg1, "lhs", 3);
		auto add_arg2 = arB_func_new_arg(add, get_type<arInt32>());
		arB_arg_set_namesz(add_arg2, "rhs", 3);
		arB_func_new_return(add, get_type<arInt32>());
		arB_package_add_func(main_package, add);

		// {
		//	lda 0
		//	lda 1
		//	add int32
		//	ret
		// }
		arB_func_body_begin(add);
		arB_func_add_instr(add, arB_instr_lda(add_arg1));
		arB_func_add_instr(add, arB_instr_lda(add_arg2));
		arB_func_add_instr(add, arB_instr_add(get_type<arInt32>()));
		arB_func_add_instr(add, arB_instr_ret());
		arB_func_body_end(add);

		// Create the Get function and add two integer types
		auto get = arB_func_newsz("Get", 3);
		arB_func_new_return(get, get_type<arInt32>());
		auto get_local_1 = arB_func_new_local(get, get_type<arInt32>());
		arB_package_add_func(main_package, get);

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
		arB_func_body_begin(get);
		arB_func_add_instr(get, arB_instr_ldc(get_type<arInt32>(), to_const(const_val1)));
		arB_func_add_instr(get, arB_instr_ldc(get_type<arInt32>(), to_const(const_val2)));
		arB_func_add_instr(get, arB_instr_ldf(add));
		arB_func_add_instr(get, arB_instr_calluref(add));
		arB_func_add_instr(get, arB_instr_stl(get_local_1));
		arB_func_add_instr(get, arB_instr_frees(get_type<arInt32>()));
		arB_func_add_instr(get, arB_instr_frees(get_type<arInt32>()));
		arB_func_add_instr(get, arB_instr_ldl(get_local_1));
		arB_func_add_instr(get, arB_instr_ret());
		arB_func_body_end(get);

		compile();

		invoke("Get");

		verify_stack_size(sizeof(arInt32));
		verify_value(pop_value<arInt32>(), (arInt32)(const_val1 + const_val2));
	}

	void operator()()
	{
		TEST(call());
		TEST(callnative());
		TEST(calluref());
	}
};

void suite_builder()
{
	SUITE(suite_builder_tests);
	SUITE(suite_builder_primitives);
	SUITE(suite_builder_datatypes);
	SUITE(suite_builder_locals);
	SUITE(suite_builder_globals);
	SUITE(suite_builder_add_sub_mult_div);
	SUITE(suite_builder_bits);
	SUITE(suite_builder_neg);
	SUITE(suite_builder_convertions);
	SUITE(suite_builder_memory);
	SUITE(suite_builder_element);
	SUITE(suite_builder_compares);
	SUITE(suite_builder_jumps);
	SUITE(suite_builder_calls);
}
