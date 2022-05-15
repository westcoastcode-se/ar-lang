#ifndef _VMC_STRING_H_
#define _VMC_STRING_H_

#include "vm_config.h"

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

// Initialize the supplied string with the soruce string
extern void vm_string_setsz(vm_string* s, const char* src, int len);

// Initialize the supplied string to zero
extern void vm_string_zero(vm_string* s);

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

// Define a constant vm_string
#define VM_STRING_CONST(name, value, len) \
	const char _const_C##name[] = value; \
	const vm_string _const_S##name = { \
		_const_C##name, \
		_const_C##name + len \
	}

// Get the address for a constant vm_string
#define VM_STRING_CONST_GET(name) (&_const_S##name)

#endif
