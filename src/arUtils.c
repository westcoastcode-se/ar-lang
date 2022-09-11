#include "arUtils.h"

BOOL arStrcmp(const char* s1, int l1, const char* s2, int l2)
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

char* arStrcpy(char* dest, const char* src, int len)
{
	int i = 0;
	for (; i < len; ++i) {
		*dest++ = *src++;
	}
	return dest;
}

char* arStrcpy_s(char* dest, int* bytes_left, const char* src, int len)
{
	if (*bytes_left < len)
		len = *bytes_left;
	*bytes_left -= len;
	return arStrcpy(dest, src, len);
}

int arStrlen(const char* str)
{
	const char* s = str;
	while (*++s != 0);
	return (s - str);
}

arByte* arMemcpy(arByte* dest, const arByte* src, arInt32 size)
{
	const arByte* const end = dest + size;
	for (; dest != end; ++dest, ++src) {
		*dest = *src;
	}
	return dest;
}

arByte* arMemcpy32(arInt32* dest, const arInt32* src, arInt32 count)
{
	const arInt32* const end = dest + count;
	for (; dest != end; ++dest, ++src) {
		*dest = *src;
	}
	return (arByte*)dest;
}

arByte* arMemcpy64(arInt64* dest, const arInt64* src, arInt32 count)
{
	const arInt64* const end = dest + count;
	for (; dest != end; ++dest, ++src) {
		*dest = *src;
	}
	return (arByte*)dest;
}

arHash32 arHash32_init()
{
	return 5381;
}

arHash32 arHash32_next(arHash32 hash, char c)
{
	// Based on http://www.cse.yorku.ca/~oz/hash.html
	return ((hash << 5) + hash) + c; /* hash * 33 + c */
}

arHash32 arHash32_from_string(const char* s, const char* end)
{
	arHash32 hash = arHash32_init();
	for (; s != end; ++s)
		hash = arHash32_next(hash, *s);
	return hash;
}

arUint64 arStrtou64(const char* str, int len)
{
	arUint64 ret = 0;
	for (int i = 0; i < len; ++i)
		ret = (ret * (arUint64)10) + (arUint64)(str[i] - '0');
	return ret;
}

arInt64 arStrtoi64(const char* str, int len)
{
	arInt64 ret = 0;

	// Special handling for a negative values
	arInt64 multiplier = 1;
	if (len > 0 && *str == '-')
	{
		multiplier = -1;
		len--;
		str++;
	}

	for (int i = 0; i < len; ++i)
		ret = (ret * (arInt64)10) + (arInt64)(str[i] - '0');
	return ret * multiplier;
}

arInt32 arGetAlignSize2(arInt32 size, arInt32 align, arInt32 ao)
{
	const arInt32 padding = (align - (size & ao)) & ao;
	return size + padding;
}

arInt32 arGetAlignSize(arInt32 size, arInt32 align)
{
	return arGetAlignSize2(size, align, align - 1);
}

// Convert a hex key into an integer value
arInt32 arHexKeyToInt(const char c)
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

arInt64 arHextoi64(const char* hex, int len)
{
	arInt64 ret = 0;
	const char* end = hex + len;
	if (len < 3)
		return 0;
	hex += 2;
	for (; hex != end; ++hex)
		ret = (ret * 16) + (arInt64)arHexKeyToInt(*hex);
	return ret;
}

arUint64 arHextou64(const char* hex, int len)
{
	arUint64 ret = 0;
	const char* end = hex + len;
	if (len < 3)
		return 0;
	hex += 2;
	for (; hex != end; ++hex)
		ret = (ret * 16) + (arUint64)arHexKeyToInt(*hex);
	return ret;
}
