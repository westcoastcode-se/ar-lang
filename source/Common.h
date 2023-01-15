#pragma once

#include <vector>
#include <list>
#include <stack>
#include <functional>
#include <string>
#include <string_view>
#include <sstream>
#include <cassert>
#include <unordered_map>
#include "Config.h"

namespace WestCoastCode
{
	template<typename T>
	using Stack = std::stack<T>;

	typedef std::string String;

	using ReadOnlyString = std::string_view;
	typedef char Char;

	typedef std::stringstream StringStream;

	template<typename Key, typename Val>
	using Map = std::unordered_map<Key, Val>;

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

		// Switch the guarded value with a new one
		void SwitchWith(T* newValue) { ptr = newValue; }
		
		MemoryGuard<T>& operator=(MemoryGuard<T>& rhs) { ptr = rhs.Done(); }

		template<class H>
		MemoryGuard<T>& operator=(const MemoryGuard<H>& rhs) { 
			ptr = const_cast<MemoryGuard<H>*>(&rhs)->Done(); 
			return *this;
		}
	};
}

#define BIT_ISSET(val, bit) ((val & ((I32)bit)) == ((I32)bit))
#define BIT(B) (1 << B)
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define ENUM_STRING_START(op) switch(op) {
#define ENUM_STRING(op) case op: { static ReadOnlyString s(STRINGIFY(op)); return s;}
#define ENUM_STRING_END() default:{ static ReadOnlyString s("Unknown"); return s; } }

