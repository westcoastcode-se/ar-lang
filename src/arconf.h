#ifndef _ARLANG_CONFIG_H_
#define _ARLANG_CONFIG_H_

#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>

#define ARLANG_MAJOR_VERSION 0
#define ARLANG_MINOR_VERSION 0
#define ARLANG_PATCH_VERSION 1

typedef float arFloat32;
typedef double arFloat64;
typedef char arInt8;
typedef char arByte;
typedef unsigned char arUint8;
typedef short arInt16;
typedef unsigned short arUint16;
typedef int arInt32;
typedef unsigned int arUint32;
typedef unsigned int arBits32;
typedef unsigned int arHash32;
typedef long long arInt64;
typedef unsigned long long arUint64;
typedef int (*arB_native_func)(struct arThread*);
typedef arInt32 arBool;

// Instruction Pointer
typedef const arByte* arIP;

// A code processed by the process. You can use this if you want to create specialized functions
// for handling instructions in the virtual machine
typedef arInt32 arOpcode;

// Represents an instruction
typedef arUint8 arIncode;

// Define that helps us define an arlang function.
#if defined(ARLANG_DYNAMIC_LIB)
#	if definde(ARLANG_BUILD_DYNAMIC_LIB)
#		define ARLANG_API __declspec(dllexport)
#	else
#		define ARLANG_API __declspec(dllimport)
#	endif
#else
#	define ARLANG_API extern
#endif

#ifndef VM_VERSION
#	define VM_VERSION 1
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

// Enable memory tracking. This is enabled by default when compiling in debug mode
#if !defined(VM_MEMORY_TRACKING)
#	if !defined(NDEBUG)
#		define VM_MEMORY_TRACKING 1
#	else
#		define VM_MEMORY_TRACKING 0
#	endif
#endif

// Enable runtime stack validation. This is only used if ARLANG_INSTRUCTION_DEBUG is enabled.
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

#ifndef NULL
#	define NULL 0
#endif

#ifndef BIT_ISSET
#	define BIT_ISSET(val, bit) ((val & bit) == bit)
#endif

#ifndef BIT
#	define BIT(B) (1 << B)
#endif

#ifndef BOOL
#	define BOOL arInt32
#endif

#ifndef TRUE
#	define TRUE (1)
#endif

#ifndef FALSE
#	define FALSE (0)
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

#endif

