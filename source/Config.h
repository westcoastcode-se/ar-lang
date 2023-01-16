#pragma once

#include <cinttypes>

#define ARLANG_MAJOR_VERSION 0
#define ARLANG_MINOR_VERSION 0
#define ARLANG_PATCH_VERSION 1
#define ARLANG_INTERPRETER_VERSION 1

// Define that helps us define an arlang function.
#if defined(ARLANG_DYNAMIC_LIB)
#	if definde(ARLANG_BUILD_DYNAMIC_LIB)
#		define ARLANG_API __declspec(dllexport)
#	else
#		define ARLANG_API __declspec(dllimport)
#	endif
#else
#	define ARLANG_API
#endif

// Define which indicates if compiled in 32 or 64-bit mode
#if defined(_WIN64)
#	define ARLANG_64BIT
#elif defined(_WIN32)
#	define ARLANG_64BIT
#elif defined(__MACOSX__)
#	define ARLANG_64BIT
#else
#	error "No valid CPU target is set"
#endif

// Default the initial stack stack size in bytes
#if !defined(VM_STACK_DEFAULT_SIZE)
#	define VM_STACK_DEFAULT_SIZE (32768)
#endif

// How many functions are allowed to be called recursivelly
#if !defined(VM_RECURSIVE_FN_CALLFRAMES)
#	define VM_RECURSIVE_FN_CALLFRAMES (128)
#endif

// How many local blocks should be allowed
#if !defined(VM_LOCALS_BLOCK_COUNT)
#	define VM_LOCALS_BLOCK_COUNT (32)
#endif

// The minimum size of a local variable
#if !defined(VM_LOCALS_MIN_SIZE)
#	define VM_LOCALS_MIN_SIZE (32768)
#endif

// Size of the memory pool
#if !defined(ARLANG_MEMORY_POOL_SIZE)
#	define ARLANG_MEMORY_POOL_SIZE (32768)
#endif

// Enable memory tracking. This is enabled by default when compiling in debug mode
#if !defined(ARLANG_MEMORY_TRACKING)
#	if !defined(NDEBUG)
#		define ARLANG_MEMORY_TRACKING 1
#	else
#		define ARLANG_MEMORY_TRACKING 0
#	endif
#endif

// Enable runtime stack validation.
// This is enabled by default when building in debug mode. 
#if !defined(VM_STACK_DEBUG)
#	if !defined(NDEBUG)
#		define VM_STACK_DEBUG 1
#	endif
#endif

// If instruction debugging should be enabled.
// This is enabled by default when building in debug mode. 
#if !defined(ARLANG_INSTRUCTION_DEBUG)
#	if !defined(NDEBUG)
#		define ARLANG_INSTRUCTION_DEBUG 1
#	endif
#endif

// Enable runtime locals validation. This is enabled by default when building in debug mode
#if !defined(VM_LOCALS_DEBUG)
#	if !defined(NDEBUG)
#		define VM_LOCALS_DEBUG 1
#	endif
#endif

#ifndef OFFSETOF
#	define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
#endif

#ifndef ASSERT_NOT_NULL
#	define ASSERT_NOT_NULL(a) assert(a != NULL && "expected '" #a "' to be set")
#endif

#ifndef ASSERT_NULL
#	define ASSERT_NULL(a) assert(a == NULL && "expected '" #a "' to be NULL")
#endif

#ifndef ASSERT_NOT_IMPLEMENTED
#	define ASSERT_NOT_IMPLEMENTED() assert(false && "Not implemented");
#endif

namespace WestCoastCode
{
	typedef char Byte;
	typedef Byte* Bytes;

	typedef int8_t I8;
	typedef uint8_t U8;
	typedef int16_t I16;
	typedef uint16_t U16;
	typedef int32_t I32;
	typedef uint32_t U32;
	typedef int64_t I64;
	typedef uint64_t U64;

	typedef float F32;
	typedef double F64;

	typedef I32 IB;
	
	static_assert(sizeof(I8) == 1, "Expected 1 byte");
	static_assert(sizeof(U8) == 1, "Expected 1 byte");
	static_assert(sizeof(I16) == 2, "Expected 2 byte");
	static_assert(sizeof(U16) == 2, "Expected 2 byte");
	static_assert(sizeof(I32) == 4, "Expected 4 byte");
	static_assert(sizeof(U32) == 4, "Expected 4 byte");
	static_assert(sizeof(I64) == 8, "Expected 8 byte");
	static_assert(sizeof(U64) == 8, "Expected 8 byte");
	static_assert(sizeof(F32) == 4, "Expected 4 byte");
	static_assert(sizeof(F64) == 8, "Expected 8 byte");
	static_assert(sizeof(IB) == 4, "Expected 4 byte");
}
