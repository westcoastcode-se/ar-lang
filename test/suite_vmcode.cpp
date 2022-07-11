#include "utils.hpp"

struct utils_vmcode : utils_vm
{
	vmcode* vmcd;
	vmi_process* process;

	void begin()
	{
		// Create a new pipeline to put bytecode into
		vmcd = vmcode_new();
		process = NULL;
	}

	void end()
	{
		if (vmcd != NULL) {
			vmcode_destroy(vmcd);
			vmcd = NULL;
		}

		if (vmp_memory_test_bytes_left() == FALSE) {
			throw_(error() << "not all memory was released");
		}
	}

	void compile(const vm_byte* source_code)
	{
		if (!vmcode_parse(vmcd, source_code)) {
			error_string_stream e;
			e << "could not parse source code: [";
			auto message = vmcd->messages.first;
			while (message != nullptr) {
				e << "\n" << message->message;
				message = message->next;
			}
			if (vmcd->panic_error_message.code != 0) {
				e << "\n" << vmcd->panic_error_message.message;
			}
			e << "\n]";
			end();
			throw_(e);
		}

		if (!vmcode_link(vmcd)) {
			error_string_stream e;
			e << "could not link source code: [";
			auto message = vmcd->messages.first;
			while (message != nullptr) {
				e << "\n" << message->message;
				message = message->next;
			}
			if (vmcd->panic_error_message.code != 0) {
				e << "\n" << vmcd->panic_error_message.message;
			}
			e << "\n]";
			end();
			throw_(e);
		}

		process = vmi_process_new();
		if (process == nullptr) {
			end();
			throw_(error() << "could not create a new VM process");
		}

		const auto result = vmi_process_load(process, vmcode_get_bytecode(vmcd));
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
};

struct suite_vmcode_tests : utils_vmcode
{
	void get_local()
	{
		static const auto source = R"(
package main

func Get() int32 {
	ret := 12345
	return ret
}
)";
		begin();
		compile(source);

		auto t = thread();

		invoke(t, "Get");

		verify_stack_size(t, sizeof(vm_int32));
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(t, sizeof(vm_int32));
		verify_value(ret, 12345);

		destroy(t);
		end();
	}

	void plus2()
	{
		static const auto source = R"(
package main

func Plus2(value int32) int32 {
	ret := value + 2
	return ret
}
)";
		begin();
		compile(source);

		auto t = thread();

		static constexpr auto value = 10000;
		*(vm_int32*)vmi_thread_push_stack(t, sizeof(vm_int32)) = value;
		
		invoke(t, "Plus2");

		verify_stack_size(t, sizeof(vm_int32) * 2);
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(t, sizeof(vm_int32));
		verify_value(ret, value + 2);
		const auto in = *(vm_int32*)vmi_thread_pop_stack(t, sizeof(vm_int32));
		verify_value(in, value);
		
		destroy(t);
		end();
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
		begin();
		compile(source);

		auto t = thread();

		static constexpr auto value = 123;
		*(vm_int32*)vmi_thread_push_stack(t, sizeof(vm_int32)) = value;

		invoke(t, "Complex");

		verify_stack_size(t, sizeof(vm_int32) * 2);
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(t, sizeof(vm_int32));
		verify_value(ret, c_complex1(value));
		const auto in = *(vm_int32*)vmi_thread_pop_stack(t, sizeof(vm_int32));
		verify_value(in, value);

		destroy(t);
		end();
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
		begin();
		compile(source);

		auto t = thread();

		static constexpr auto value = 123;
		*(vm_int32*)vmi_thread_push_stack(t, sizeof(vm_int32)) = value;

		invoke(t, "Complex");

		verify_stack_size(t, sizeof(vm_int32) * 2);
		const auto ret = *(vm_int32*)vmi_thread_pop_stack(t, sizeof(vm_int32));
		verify_value(ret, c_complex2(value));
		const auto in = *(vm_int32*)vmi_thread_pop_stack(t, sizeof(vm_int32));
		verify_value(in, value);

		destroy(t);
		end();
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
		begin();
		compile(source);
		
		auto t = thread();

		static constexpr auto COUNT = 10000;

		// 10000 values to sort
		auto arr = new vm_int32[COUNT];
		srand(0);
		for (int i = 0; i < COUNT; ++i) {
			arr[i] = rand() % 1000;
		}
		*(vm_int32**)vmi_thread_push_stack(t, sizeof(vm_int32*)) = arr;
		*(vm_int32*)vmi_thread_push_stack(t, sizeof(vm_int32)) = 0;
		*(vm_int32*)vmi_thread_push_stack(t, sizeof(vm_int32)) = COUNT - 1;
		invoke(t, "QuickSort");
		destroy(t);
		vm_int32 tmp = 0;
		for (int i = 0; i < COUNT; ++i) {
			if (arr[i] < tmp) {
				throw_(error() << "failed to quicksort values");
			}
			tmp = arr[i];
		}
		delete[] arr;
		verify_stack_size(t, sizeof(vm_int32*) + sizeof(vm_int32) + sizeof(vm_int32));
		end();
	}

	void operator()()
	{
		TEST(get_local);
		TEST(plus2);
		//TEST(quicksort);
	}
};

void suite_vmcode()
{
	SUITE(suite_vmcode_tests);
}
