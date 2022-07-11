#include "vm_string.h"

void vm_string_setsz(vm_string* s, const char* src, int len)
{
	s->start = src;
	s->end = src + len;
}

void vm_string_zero(vm_string* s)
{
	s->start = s->end = NULL;
}

void vm_mutable_string_malloc(vm_mutable_string* dest, const char* str, int len)
{
	dest->start = malloc(len);
	vm_str_cpy(dest->start, str, len);
	dest->end = dest->start + len;
}

void vm_mutable_string_free(vm_mutable_string* m)
{
	if (m->start)
	{
		free(m->start);
		m->start = m->end = NULL;
	}
}

BOOL vm_string_cmpsz(const vm_string* s, const char* other_string, int length)
{
	return vm_str_cmp(s->start, vm_string_length(s), other_string, length);
}

BOOL vm_string_cmp(const vm_string* s1, const vm_string* s2)
{
	return vm_str_cmp(s1->start, vm_string_length(s1), s2->start, vm_string_length(s2));
}

BOOL vm_string_cmp_char(const vm_string* s, char c)
{
	if (vm_string_length(s) != 1)
		return FALSE;
	return *s->start == c;
}

BOOL vm_string_starts_with(const vm_string* s, const char* other_string, int length)
{
	int i;
	if (vm_string_length(s) < length)
		return FALSE;
	for (i = 0; i < length; ++i)
	{
		if (s->start[i] != other_string[i])
			return FALSE;
	}
	return TRUE;
}

BOOL vm_string_ends_with(const vm_string* s, char c)
{
	if (s->start == s->end)
		return FALSE;
	return *(s->end - 1) == c;
}

vm_int32 vm_string_length(const vm_string* s)
{
	return (vm_int32)(s->end - s->start);
}
