#ifndef _ARLANG_UTILS_H_
#define _ARLANG_UTILS_H_

#include "arconf.h"

// Compare two strings
ARLANG_API BOOL arStrcmp(const char* s1, int l1, const char* s2, int l2);

// Copy the content of a string into a new memory location
ARLANG_API char* arStrcpy(char* dest, const char* src, int len);

// Calculate the length of a NULL terminated string
ARLANG_API int arStrlen(const char* str);

// Copy the bytes found in the source memory location and put the result into the destination location
ARLANG_API arByte* arMemcpy(arByte* dest, const arByte* src, arInt32 size);

// Copy the bytes found in the source memory location and put the result into the destination location
ARLANG_API arByte* arMemcpy32(arInt32* dest, const arInt32* src, arInt32 count);

// Copy the bytes found in the source memory location and put the result into the destination location
ARLANG_API arByte* arMemcpy64(arInt64* dest, const arInt64* src, arInt32 count);

// Give an initialized hash value
ARLANG_API arHash32 arHash32_init();

// Increment the hash value using a character
ARLANG_API arHash32 arHash32_next(arHash32 hash, char c);

// Calculate a hash based on the supplied string
ARLANG_API arHash32 arHash32_from_string(const char* s, const char* end);

// Convert the supplied c-string into a 64 bit unsigned integer. The size of the string is limited to the supplied length 
ARLANG_API arUint64 arStrtou64(const char* str, int len);

// Convert the supplied c-string into a 64 bit integer. The size of the string is limited to the supplied length 
ARLANG_API arInt64 arStrtoi64(const char* str, int len);

// Figure out the size if we want a fixed memory alignment
ARLANG_API arInt32 arGetAlignSize2(arInt32 size, arInt32 align, arInt32 ao);

// Figure out the size if we want a fixed memory alignment. For example, if the supplied size is 2 and align 4, then this will return 4
ARLANG_API arInt32 arGetAlignSize(arInt32 size, arInt32 align);

// Convert a hex value into a 64 bit integer
ARLANG_API arInt64 arHextoi64(const char* hex, int len);

// Convert a hex value into a 64 bit unsigned integer
ARLANG_API arUint64 arHextou64(const char* hex, int len);

#endif
