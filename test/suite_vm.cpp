#include "test.hpp"

// Base class for all vm tests
struct suite_vm_utils : test_utils
{
};

// All test functions
struct suite_vm_tests : suite_vm_utils
{
	void create_thread()
	{
	}

	void operator()()
	{
		TEST(create_thread);
	}
};

void suite_vm()
{
	SUITE(suite_vm_tests);
}
