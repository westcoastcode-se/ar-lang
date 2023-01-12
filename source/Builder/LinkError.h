#pragma once

#include <exception>
#include <cstdarg>

namespace WestCoastCode::Builder
{
	// Error raised when 
	class LinkError : public std::exception
	{
	};
}
