#ifndef _VM_CONFIG_H_
#define _VM_CONFIG_H_

#include <memory.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

typedef float vm_float32;
typedef double vm_float64;
typedef char vm_int8;
typedef char vm_byte;
typedef unsigned char vm_uint8;
typedef short vm_int16;
typedef unsigned short vm_uint16;
typedef int vm_int32;
typedef unsigned int vm_uint32;
typedef long long vm_int64;
typedef unsigned long long vm_uint64;

#ifndef VM_VERSION
#	define VM_VERSION 1
#endif

// Default the initial stack size is 32kb
#if !defined(VM_STACK_DEFAULT_SIZE)
#	define VM_STACK_DEFAULT_SIZE (32768)
#endif

// Enable memory tracking. This is enabled by default when compiling in debug mode
#if !defined(VM_MEMORY_TRACKING)
#	if !defined(NDEBUG)
#		define VM_MEMORY_TRACKING 1
#	else
#		define VM_MEMORY_TRACKING 0
#	endif
#endif

#if defined(_WIN64)
#	define VM_64BIT
#elif defined(_WIN32)
#	define VM_32BIT
#else
#	error "No valid CPU target is set"
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
#	define BOOL int
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

#endif

