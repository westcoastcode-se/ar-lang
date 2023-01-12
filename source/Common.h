#pragma once

#include <vector>
#include <list>
#include <stack>
#include <functional>
#include <string>
#include <string_view>
#include <sstream>
#include <cassert>
#include "Config.h"

namespace WestCoastCode
{
	template<typename T>
	using List = std::list<T>;

	template<typename T>
	using Vector = std::vector<T>;

	template<typename T>
	using Stack = std::stack<T>;

	typedef std::string String;

	using ReadOnlyString = std::string_view;
	typedef char Char;

	typedef std::stringstream StringStream;

	// A memory guard that will ensure that memory is cleared if exceptions are thrown
	template<class T>
	struct MemoryGuard
	{
		T* ptr;

		MemoryGuard(T* ptr) : ptr(ptr) {}
		template<class H>
		MemoryGuard(H* ptr) : ptr(ptr) {}
		MemoryGuard(MemoryGuard& g) : ptr(g.ptr) { g.ptr = nullptr; }

		~MemoryGuard() {
			if (ptr)
				delete ptr;
		}

		// Guard is now considered done and should not delete any memory
		T* Done() { T* tmp = ptr; ptr = nullptr; return tmp; }
		
		MemoryGuard<T>& operator=(MemoryGuard<T>& rhs) { ptr = rhs.Done(); }

		template<class H>
		MemoryGuard<T>& operator=(const MemoryGuard<H>& rhs) { 
			ptr = const_cast<MemoryGuard<H>*>(&rhs)->Done(); 
			return *this;
		}
	};

	// Helper struct managing range validation
	template<typename T>
	struct Range
	{
		T start;
		T end;

		Range(T start, T end) : start(start), end(end) {}
		Range(const Range<T>& rhs) : start(rhs.start), end(rhs.end) {}
		
		// Check to see if the supplied value is inside range
		bool Inside(T val) const noexcept { return val >= start && val <= end; }
	};
}

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define ENUM_STRING_START(op) switch(op) {
#define ENUM_STRING(op) case op: { static ReadOnlyString s(STRINGIFY(op)); return s;}
#define ENUM_STRING_END() default:{ static ReadOnlyString s("Unknown"); return s; } }

