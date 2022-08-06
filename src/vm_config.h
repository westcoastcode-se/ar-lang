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
typedef unsigned int vm_bits32;
typedef unsigned int vm_hash32;
typedef long long vm_int64;
typedef unsigned long long vm_uint64;
typedef int (*vm_nativefunc)(struct vmi_thread*);

#ifndef VM_VERSION
#	define VM_VERSION 1
#endif

#if defined(_WIN64)
#	define VM_64BIT
#elif defined(_WIN32)
#	define VM_32BIT
#elif defined(__MACOSX__)
#	define VM_64BIT
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

// Enable runtime stack validation. This is enabled by default when building in debug mode
#if !defined(VM_STACK_DEBUG)
#	if !defined(NDEBUG)
#		define VM_STACK_DEBUG 1
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
#	define BOOL vm_int32
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

#endif

