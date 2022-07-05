#include "test.hpp"

bool _tests_success = true;

// Verify that you can build bytecode using the builder interface
extern void suite_vmp();
// Verify that you can build bytecode using the scripting language
extern void suite_vmcode();

int main()
{
	// Suites
	suite_vmp();
	suite_vmcode();

	// Verify result
	if (!_tests_success)
	{
		cerr << "One or more tests failed" << endl;
		return 1;
	}
	cout << "All tests succeeeded " << sizeof(float) << " " << sizeof(double) << endl;
	return 0;
}