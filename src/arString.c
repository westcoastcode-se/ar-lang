#include "arString.h"
#include "arUtils.h"

void arString_setsz(arString* s, const char* src, int len)
{
	s->start = src;
	s->end = src + len;
}

void arString_zero(arString* s)
{
	s->start = s->end = NULL;
}

BOOL arString_cmpsz(const arString* s, const char* other_string, int length)
{
	return arStrcmp(s->start, arString_length(s), other_string, length);
}

BOOL arString_cmp(const arString* s1, const arString* s2)
{
	return arStrcmp(s1->start, arString_length(s1), s2->start, arString_length(s2));
}

BOOL arString_cmp_char(const arString* s, char c)
{
	if (arString_length(s) != 1)
		return FALSE;
	return *s->start == c;
}

BOOL arString_starts_with(const arString* s, const char* other_string, int length)
{
	int i;
	if (arString_length(s) < length)
		return FALSE;
	for (i = 0; i < length; ++i)
	{
		if (s->start[i] != other_string[i])
			return FALSE;
	}
	return TRUE;
}

BOOL arString_ends_with(const arString* s, char c)
{
	if (s->start == s->end)
		return FALSE;
	return *(s->end - 1) == c;
}

arInt32 arString_length(const arString* s)
{
	return (arInt32)(s->end - s->start);
}

char* arString_get(const arString* src, char* dest, int* len)
{
	*len = arString_length(src);
	return arStrcpy(dest, src->start, *len);
}

char* arString_cpy_s(char* dest, int* bytes_left, const arString* src)
{
	return arStrcpy_s(dest, bytes_left, src->start, arString_length(src));
}
