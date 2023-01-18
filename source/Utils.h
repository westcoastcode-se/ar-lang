#pragma once

#include "Common.h"

namespace WestCoastCode
{
	inline static bool Strcmp(const char* s1, int l1, const char* s2, int l2)
	{
		int i;
		if (l1 != l2)
			return false;
		for (i = 0; i < l1; ++i)
		{
			if (s1[i] != s2[i])
				return false;
		}
		return true;
	}

	inline static U64 Strtou64(const char* str, int len)
	{
		U64 ret = 0;
		for (int i = 0; i < len; ++i) {
			const auto v = str[i];
			if (v == 'u')
				break;
			ret = (ret * (U64)10) + (U64)(v - '0');
		}
		return ret;
	}

	inline static I64 Strtoi64(const char* str, int len)
	{
		I64 ret = 0;

		// Special handling for a negative values
		I64 multiplier = 1;
		if (len > 0 && *str == '-')
		{
			multiplier = -1;
			len--;
			str++;
		}

		for (int i = 0; i < len; ++i)
			ret = (ret * (I64)10) + (I64)(str[i] - '0');
		return ret * multiplier;
	}

}
