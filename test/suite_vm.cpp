#include "test.hpp"

// Base class for all vm tests
struct suite_vm_utils : test_utils
{

	void verify_compiler(vmc_compiler* c)
	{
		if (!vmc_compiler_success(c)) {
			error_string_stream e;
			e << "could not compile source code: [";
			auto message = c->messages;
			while (message != nullptr) {
				e << "\n" << message->message;
				message = message->next;
			}
			e << "\n]";
			throw_(e);
		}
	}

	vmc_compiler* compile(const vm_byte* src)
	{
		auto const compiled = vmc_compiler_compile_string(src);
		verify_compiler(compiled);
		return compiled;
	}

	void destroy(vmc_compiler* c)
	{
		vmc_compiler_destroy(c);
	}

	void destroy(vmi_process* p)
	{
		vmi_process_destroy(p);
	}

	void destroy(vmi_thread* p)
	{
		vmi_thread_destroy(p);
	}

	void invoke(vmi_process* p, vmi_thread* t)
	{
		const auto result = vmi_process_exec(p, t);
		if (result != 0)
			throw_(error() << "error occurred when executing thread: " << result << ". Message: " << t->exit_reason);
	}

	vmi_process* process(vmc_compiler* c)
	{
		auto const p = vmi_process_new();
		if (p == nullptr)
			throw_(error() << "could not create a new VM process");
		const auto result = vmi_process_load(p, vmc_compiler_bytecode(c));
		if (result != 0)
			throw_(error() << "failed to load bytecode because: " << result);
		return p;
	}

	vmi_thread* thread(vmi_process* p)
	{
		auto const t = vmi_thread_new(p);
		if (t == nullptr)
			throw_(error() << "could not spawn a new VM thread");
		return t;
	}
};

// All test functions
struct suite_vm_tests : suite_vm_utils
{
	void instantly_return()
	{
		const auto source = R"(
.fn 
	ret # Jump back to caller instruction
)";
		auto c = compile(source);
		auto p = process(c);
		auto t = thread(p);

		invoke(p, t);

		destroy(t);
		destroy(p);
		destroy(c);
	}

	void reserve_var_memory()
	{
		const auto source = R"(
.fn name (i8,i32) (i8):
	ldarg $0			# Push 1 byte
	conv i32			# Pop 1 byte and push 4 bytes
	ldarg $1			# Push 4 bytes
	add i32				# Add two i32
	conv i8				# Pop 4 bytes and push 1 byte
	ret					# Jump back to caller instruction

.fn main () ():
	var (i8)			# Allocate 1 byte on the stack; S=1
	ldc i8 10			# Push 1 byte					S=2
	ldc i32 -100		# Push 4 bytes					S=6
	invoke name			# Call "name"					S=7
	pop 5												S=2
	starg $0			# Pop 1 byte to the first var	S=1
	ret

package game

type GameObject struct {
	ID int32
}

type Player struct {
	GameObject
	Identity string
}

func (p Player) Hello(name string) {
	stdio.Println(string(p.ID) + " Hello " + name)
}

func hello(name string, age int32) string {
	return name + string(age)
}
)";
	}

	void operator()()
	{
		TEST(instantly_return);
		TEST(reserve_var_memory);
	}
};

void suite_vm()
{
	SUITE(suite_vm_tests);
}
