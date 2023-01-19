#pragma once

#include <iostream>
#include <stdexcept>
#include <cstdarg>
#include <sstream>
#include <array>
#include <vector>
#include <unordered_map>
#include "../Arlang.h"

using namespace WestCoastCode;
using namespace WestCoastCode::Builder;
using namespace WestCoastCode::Compilation;
using namespace WestCoastCode::Interpreter;

using namespace std;

extern bool _tests_success;
extern int _tests_count;
extern int _tests_failed;

struct ErrorStringStream : public std::stringstream
{
	void Throw() const
	{
		throw std::runtime_error(str().c_str());
	}
};

#define SUITE(s) \
	cout << "Suite '" << #s << "'" << endl; \
	s _##s; _##s();

#define SUITE_RANGE(s, start, end) {\
	cout << "Suite '" << #s << endl; \
	for(int i = start; i < end; ++i) { \
	cout << "Suite '" << #s << "' (" << i << ")" << endl; \
		s _##s; _##s(i); \
	} \
}

#define TEST(s) \
	_tests_count++;\
	cout << "\tTest '" << #s << "'"; \
	try {\
		BeforeEach(); \
		s; \
		AfterEach(nullptr); \
		cout << " - OK" << endl; \
	}\
	catch (const std::exception& e) { \
		cerr << " - ERROR: " << e.what() << endl; \
		_tests_success = false; \
		_tests_failed++;\
		try { \
			AfterEach(&e); \
		} catch (const std::exception& e) { \
			cerr << " - ERROR: " << e.what() << endl; \
		} \
	}
