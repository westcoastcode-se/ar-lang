#pragma once

#include <exception>

namespace WestCoastCode::Interpreter
{
	// Base class for errors raised by a thread
	class ThreadError : public std::exception
	{
	public:
	};
}
