#include "test.hpp"

bool _tests_success = true;

// Verify that you can build bytecode using the builder interface
extern void suite_vmp();

int main()
{
	// Suites
	suite_vmp();

	// Verify result
	if (!_tests_success)
	{
		cerr << "One or more tests failed" << endl;
		return 1;
	}
	cout << "All tests succeeeded " << sizeof(float) << " " << sizeof(double) << endl;
	return 0;
}