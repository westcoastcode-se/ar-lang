#pragma once

#include <exception>
#include <cstdarg>
#include "../Common.h"

namespace WestCoastCode::Compilation
{
	// Base class for errors raised by the interpreter
	class CompilationError : public std::exception
	{
	public:
		CompilationError();

		// std::exception
		char const* what() const final;

		/// @brief Print the error to stderr
		virtual void PrintToStderr() const = 0;

	protected:
		// Set the error message
		void SetErrorf(const char* fmt, ...);

		// Set the error message
		void SetError(const String& error);

	private:
		String _error;
	};
}
