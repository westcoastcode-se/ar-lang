#include "test.hpp"

bool _tests_success = true;
int _tests_count = 0;
int _tests_failed = 0;

// Verify that you can build bytecode using the builder interface
extern void suite_builder();
// Verify that ar-lang source can be compiled
extern void suite_compiler();

int main()
{
	// Suites
	suite_builder();
	suite_compiler();

	// Verify result
	if (!_tests_success)
	{
		cerr << _tests_failed << " test failed of " << _tests_count << " tests" << endl;
		return 1;
	}
	cout << "All " << _tests_count << " tests succeeeded" << endl;
	return 0;
}