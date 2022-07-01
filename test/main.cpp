#include "test.hpp"

bool _tests_success = true;

// Verify virtual machine
extern void suite_vm();
// Verify lexer
extern void suite_lexer();
// Verify that you can build bytecode using the builder interface
extern void suite_vmp();

int main()
{
	// Suites
	suite_lexer();
	suite_vm();
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