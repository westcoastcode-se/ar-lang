#pragma once

#include <exception>
#include <cstdarg>
#include "../Common.h"

namespace WestCoastCode::Builder
{
	// Error raised when 
	class LinkError : public std::exception
	{
	public:
		char const* what() const;

	protected:
		// Set the error message
		void SetErrorf(const char* fmt, ...);

		// Set the error message
		void SetError(const String& error);

	private:
		String _error;
	};

	class LinkErrorInvalidBytecodeSize : public LinkError
	{
	public:
		char const* what() const final { return "invalid bytecode size generation"; }
	};
}