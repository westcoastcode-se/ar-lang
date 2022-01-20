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
var (i8,i8,i32)			# i8 = 4 bytes, i32 = 4 bytes
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
