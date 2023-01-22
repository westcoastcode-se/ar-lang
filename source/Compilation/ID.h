#pragma once

#include "../Common.h"

namespace WestCoastCode
{
	namespace Compilation
	{
		/// @brief Represents a runtime-unique ID. The value itself is automatically generated
		struct ARLANG_API ID
		{
			/// @brief The unique id
			const I32 value;

			ID();

			ID(const ID& rhs) : value(rhs.value) {}
		};

	}
}

inline static std::ostream& operator<< (std::ostream& out, WestCoastCode::Compilation::ID const& t)
{
	return out << "ID(" << t.value << ")";
}

