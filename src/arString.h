#ifndef _VMC_STRING_H_
#define _VMC_STRING_H_

#include "arconf.h"

// Represents a string
typedef struct arString
{
	const char* start;
	const char* end;
} arString;

// Initialize the supplied string with the soruce string
ARLANG_API void arString_setsz(arString* s, const char* src, int len);

// Initialize the supplied string to zero
ARLANG_API void arString_zero(arString* s);

// Compare two strings
ARLANG_API BOOL arString_cmpsz(const arString* s, const char* other_string, int length);

// Compare two strings
ARLANG_API BOOL arString_cmp(const arString* s1, const arString* s2);

// Compare a string with a character
ARLANG_API BOOL arString_cmp_char(const arString* s, char c);

// Check to see if the string starts with the supplied value
ARLANG_API BOOL arString_starts_with(const arString* s, const char* other_string, int length);

// Check to see if the string ends with the supplied character
ARLANG_API BOOL arString_ends_with(const arString* s, char c);

// Length of the supplied string
ARLANG_API int arString_length(const arString* s);

// Get the content of the string and put it into the destination
ARLANG_API char* arString_get(const arString* src, char* dest, int* len);

// Copy the string content into the supplied destination buffer
ARLANG_API char* arString_cpy_s(char* dest, int* bytes_left, const arString* src);

// Copy the string content into the supplied destination buffer
ARLANG_API char* arString_cpy(char* dest, const arString* src);

// Define a constant arString
#define CONST_VM_STRING(ns, name, value, len) \
const arString* ##ns##_##name() { \
	static const char c[] = value; \
	static const arString str = { c, c + len }; \
	return &str; \
}

// Get a constant string
#define GET_CONST_VM_STRING(ns, name) ns##_##name()

#endif
