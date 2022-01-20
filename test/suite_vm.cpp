#include "test.hpp"

// Base class for all vm tests
struct suite_vm_utils : test_utils
{
};

// All test functions
struct suite_vm_tests : suite_vm_utils
{
	void reserve_var_memory()
	{
		const auto source = R"(
var (i8,i8,i32,addr)		# The total amount of memory is 4 * 8 = 32 bytes because each block is 8 bytes
ldc i32 100					# Load constant i32 value 100 onto the stack
conv i8						# Convert the value so that it fits into i8
store $0					# Set variable $0 to 100
ldc i8 127					# Load constant i8 value of 127 onto the stack
store $1
)";

	}

	void operator()()
	{
		TEST(reserve_var_memory);
	}
};

void suite_vm()
{
	SUITE(suite_vm_tests);
}
