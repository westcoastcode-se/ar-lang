#ifndef _VMC_STRING_H_
#define _VMC_STRING_H_

#include "vm_config.h"

// Represents a string that we are allowed to write to
struct vm_mutable_string
{
	char* start;
	const char* end;
};
typedef struct vm_mutable_string vm_mutable_string;

// Represents a string
struct vm_string
{
	const char* start;
	const char* end;
};
typedef struct vm_string vm_string;

// Compare two strings
static inline BOOL vm_str_cmp(const char* s1, int l1, const char* s2, int l2)
{
	int i;
	if (l1 != l2)
		return FALSE;
	for (i = 0; i < l1; ++i)
	{
		if (s1[i] != s2[i])
			return FALSE;
	}
	return TRUE;
}

// Copy the content of a string into a new memory location
static inline char* vm_str_cpysz(char* dest, const char* src, int len)
{
	int i = 0;
	for (; i < len; ++i) {
		*dest++ = *src++;
	}
	return dest;
}

// Calculate the length of a null terminated string
static inline int vm_str_len(const char* str)
{
	int len = 0;
	while (*str++ != 0)
		++len;
	return len;
}

// Initialize the supplied string with the soruce string
extern void vm_string_setsz(vm_string* s, const char* src, int len);

// Initialize the supplied string to zero
extern void vm_string_zero(vm_string* s);

// Allocate memory and copy the supplied source code
extern void vm_mutable_string_malloc(vm_mutable_string* dest, const char* str, int len);

// Free the internal memory
extern void vm_mutable_string_free(vm_mutable_string* m);

// Compare two strings
extern BOOL vm_string_cmpsz(const vm_string* s, const char* other_string, int length);

// Compare two strings
extern BOOL vm_string_cmp(const vm_string* s1, const vm_string* s2);

// Compare a string with a character
extern BOOL vm_string_cmp_char(const vm_string* s, char c);

// Check to see if the string starts with the supplied value
extern BOOL vm_string_starts_with(const vm_string* s, const char* other_string, int length);

// Check to see if the string ends with the supplied character
extern BOOL vm_string_ends_with(const vm_string* s, char c);

// Length of the supplied string
extern int vm_string_length(const vm_string* s);

// Copy the content of a string into a new memory location
static inline char* vm_str_cpy(char* dest, const vm_string* str)
{
	return vm_str_cpysz(dest, str->start, vm_string_length(str));
}

// Define a constant vm_string
#define VM_STRING_CONST(name, value, len) \
	static const char _const_C##name[] = value; \
	static const vm_string _const_S##name = { \
		_const_C##name, \
		_const_C##name + len \
	}

// Get the address for a constant vm_string
#define VM_STRING_CONST_GET(name) (&_const_S##name)

#endif
