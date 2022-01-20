#include "test.hpp"

bool _tests_success = true;

// Verify virtual machine operators
extern void suite_vm();

int main()
{
	// Suites
	suite_vm();

	// Verify result
	if (!_tests_success)
	{
		cerr << "One or more tests failed" << endl;
		return 1;
	}
	cout << "All tests succeeeded" << endl;
	return 0;
}