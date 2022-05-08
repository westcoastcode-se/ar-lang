#include "test.hpp"

bool _tests_success = true;

// Verify virtual machine
extern void suite_vm();
// Verify lexer
extern void suite_lexer();

int main()
{
	// Suites
	suite_vm();
	suite_lexer();

	// Verify result
	if (!_tests_success)
	{
		cerr << "One or more tests failed" << endl;
		return 1;
	}
	cout << "All tests succeeeded " << sizeof(float) << " " << sizeof(double) << endl;
	return 0;
}