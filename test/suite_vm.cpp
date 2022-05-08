#include "test.hpp"

// Base class for all vm tests
struct suite_vm_utils : test_utils
{
	void verify_compiler(vmc_compiler* c)
	{
		if (!vmc_compiler_success(c)) {
			error_string_stream e;
			e << "could not compile source code: [";
			auto message = c->messages_first;
			while (message != nullptr) {
				e << "\n" << message->message;
				message = message->next;
			}
			e << "\n]";
			throw_(e);
		}
	}

	void verify_stack(vmi_thread* t, vm_int32 offset, vm_int32 value)
	{
		const vm_int32* byte = (vm_int32*)(t->stack.blocks + offset);
		if (*byte != value) {
			throw_(error() << "expected stack value at " << offset << " to be " << value << " but was " << *byte);
		}
	}

	vmc_compiler* compile(const vm_byte* src)
	{
		auto const compiler = vmc_compiler_new(NULL);
		if (!vmc_compiler_compile(compiler, src))
			throw_(error() << "failed to compile source code");
		verify_compiler(compiler);
		return compiler;
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

	void invoke(vmi_process* p, vmi_thread* t, const char* entry_point)
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
	void calculate_two_i32()
	{
/*
fn Add(lhs int32, rhs int32) (int32) {
	return lhs + rhs
}

fn AddTwoInts() (int32) {
	return Add(10, 20)
}
*/	
		const auto source = R"(
fn Add (lhs int32, rhs int32) (int32) {
	load_a 0	// Push first arg (4 bytes) to the stack (esp + 0)
	load_a 1	// Push second arg (4 bytes) to the stack (esp + 4)
	add int32	// Pop the two top-most i32 values on the stack and push the sum of those values to the stack
	save_r 0	// Pop the top stack value and put it into the first return value
	ret			// Return to the caller address (assume return value is on the top of the stack)
}
/*
fn AddTwoInts() (int32) {
	ldc int32 10	// Argument 1
	ldc int32 20	// Argument 2
	alloc_s 4		// Return value sizeof(int32)
	call Add(int32, int32) (int32)
	save_l 			// Pop the top most value and store it into the first local memory block
	free_s 8		// Release 8 bytes from the stack
	ret
}
*/
)";
		auto c = compile(source);
		auto p = process(c);
		auto t = thread(p);

		// begin_
		vmi_thread_push_i32(t, 10);
		vmi_thread_push_i32(t, 20);
		vmi_thread_push_i32(t, 99); // return value here (can be done by the API)
		
		// Stack:
		// 10, 20, _ret1
		//		Add
		//			10, 20, _ret1, <IP>
		// Stack:
		// 10, 20, 30

		invoke(p, t, "Add(int32,int32)(int32)");

		if (vmi_stack_count(&t->stack) != 12) {
			throw_(error() << "expected stack size 12 but was " << vmi_stack_count(&t->stack));
		}
		verify_stack(t, 0, 10);
		verify_stack(t, 4, 20);
		verify_stack(t, 8, 30);

		destroy(t);
		destroy(p);
		destroy(c);

	}

	void instantly_return()
	{
// Script Source Code:
/*
package stdio

extern fn Printf(value string) (void)

package main

import "stdio"

fn Calc(s int16, lhs int32) (int64) {
	return (int64)(s + lhs)
}

fn Main() int {
	stdio.Printf("Hello World!")
	var i2 = 10
	stdio.Printf(string(Calc(32766, i2))
	return 0
}
*/
		const auto source = R"(
# package main
.pkg stdio
.extern fn Print (str) void
.fn Printf (str) void
	

.pkg main
.import "stdio"

.fn Stuff (i16, i32) (i64)
	ldarg 0			# Push first arg (2 bytes) to the stack
	conv i32		# Convert it into a 4 byte integer
	ldarg 1			# Push second arg (4 bytes) to the stack
	add i32			# Pop the two top-most i32 values and push the sum of those values to the stack
	conv i64		# Convert the top-most stack value into 8 bytes (64 bit integer)
	ret				# Jump back to caller

# fn Main() int
.fn Main () (i32)
	var (i32) # Prepare stack for "i2" value (4 bytes)

	#stdio.Printf("Hello World!")
	ldc str "Hello World!"
	invoke stdio.Printf(string) (void)
	
	# i2 = 10
	ldc i32 10
	stl 0

	# stdio.Printf(string(Stuff(32766, i2))
	ldc i16 32766				# Push 16 bit integer value
	ldl 0						# Push the value found in local variable container 0 (32 bits)
	invoke Stuff(i16,i32) (i64)	# Invoke function "Stuff"
	invoke String(i64) (string) # Invoke "String", which converts the 64 bit integer into a string
	invoke stdio.Printf(string) (void)

	const 0
	ret
)";
/*
	Instructions:
	ldc <type> <value> = load constant
	ldl <index> = load local value
	stl <index> = store local value (figures out the stack size based on type on index)

*/
		/*auto c = compile(source);
		auto p = process(c);
		auto t = thread(p);

		invoke(p, t);

		destroy(t);
		destroy(p);
		destroy(c);*/
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
		TEST(calculate_two_i32);
		//TEST(instantly_return);
		//TEST(reserve_var_memory);
	}
};

void suite_vm()
{
	SUITE(suite_vm_tests);
}
