#pragma once

#include <exception>
#include "../Common.h"

namespace WestCoastCode::Interpreter
{
	// Base class for errors raised by the interpreter
	class InterpreterError : public std::exception
	{
	public:
		InterpreterError();

		// std::exception
		char const* what() const final;

	protected:
		// Set the error message
		void SetErrorf(const char* fmt, ...);

		// Set the error message
		void SetError(const String& error);

	private:
		String _error;
	};
}
