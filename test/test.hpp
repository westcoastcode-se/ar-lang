#pragma once

#include <vm.h>
#include <iostream>
#include <stdexcept>
#include <cstdarg>
#include <sstream>
#include <array>
#include <vector>
#include <unordered_map>

using namespace std;

extern bool _tests_success;
extern int _tests_count;
extern int _tests_failed;

struct error_string_stream : public std::stringstream
{
	void throw_() const
	{
		throw std::runtime_error(str().c_str());
	}
};

struct test_utils
{
	static inline error_string_stream error()
	{
		return error_string_stream();
	}

	static inline void throw_(const error_string_stream& stream)
	{
		stream.throw_();
	}

	static inline void throw_(const char* format, ...)
	{
		char memory[4096];
		va_list argptr;
		va_start(argptr, format);
		vsprintf(memory, format, argptr);
		va_end(argptr);
		throw std::runtime_error(memory);
	}
};

#define SUITE(s) \
	cout << "Suite '" << #s << "'" << endl; \
	s _##s; _##s();

#define TEST(s) \
	_tests_count++;\
	cout << "\tTest '" << #s << "'"; \
	try {\
		beforeEach(); \
		s; \
		afterEach(); \
		cout << " - OK" << endl; \
	}\
	catch (const std::exception& e) { \
		cerr << " - ERROR: " << e.what() << endl; \
		_tests_success = false; \
		_tests_failed++;\
		afterEach(); \
	}
