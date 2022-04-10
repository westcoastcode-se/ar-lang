#ifndef _VM_LEXER_MATH_H_
#define _VM_LEXER_MATH_H_

#include "../vm_config.h"

// Convert the supplied c-string into a 64 bit unsigned integer. The size of the string is limited to the supplied length 
static inline vm_uint64 strtou64(const char* str, int len)
{
	vm_uint64 ret = 0;
	for (int i = 0; i < len; ++i)
		ret = (ret * (vm_uint64)10) + (vm_uint64)(str[i] - '0');
	return ret;
}

// Convert the supplied c-string into a 64 bit integer. The size of the string is limited to the supplied length 
static inline vm_int64 strtoi64(const char* str, int len)
{
	vm_int64 ret = 0;

	// Special handling for a negative values
	vm_int64 multiplier = 1;
	if (len > 0 && *str == '-')
	{
		multiplier = -1;
		len--;
		str++;
	}

	for (int i = 0; i < len; ++i)
		ret = (ret * (vm_int64)10) + (vm_int64)(str[i] - '0');
	return ret * multiplier;
}

// Figure out the size if we want a fixed memory alignment
static inline vm_int32 get_align_size2(vm_int32 size, vm_int32 align, vm_int32 ao)
{
	const vm_int32 padding = (align - (size & ao)) & ao;
	return size + padding;
}

// Figure out the size if we want a fixed memory alignment
static inline vm_int32 get_align_size(vm_int32 size, vm_int32 align)
{
	return get_align_size2(size, align, align - 1);
}

// Convert a hex key into an integer value
static inline vm_int32 hex_key_to_int(const char c)
{
	switch (c)
	{
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		return c - '0';
	case 'a':
	case 'b':
	case 'c':
	case 'd':
	case 'e':
	case 'f':
		return c - 'a' + 10;
	case 'A':
	case 'B':
	case 'C':
	case 'D':
	case 'E':
	case 'F':
		return c - 'A' + 10;
	default:
		return 0;
	}
}

// Convert a hex value into a 64 bit integer
static inline vm_int64 hextoi64(const char* hex, int len)
{
	vm_int64 ret = 0;
	const char* end = hex + len;
	if (len < 3)
		return 0;
	hex += 2;
	for (; hex != end; ++hex)
		ret = (ret * 16) + (vm_int64)hex_key_to_int(*hex);
	return ret;
}

// Convert a hex value into a 64 bit unsigned integer
static inline vm_uint64 hextou64(const char* hex, int len)
{
	vm_uint64 ret = 0;
	const char* end = hex + len;
	if (len < 3)
		return 0;
	hex += 2;
	for (; hex != end; ++hex)
		ret = (ret * 16) + (vm_uint64)hex_key_to_int(*hex);
	return ret;
}

#endif
