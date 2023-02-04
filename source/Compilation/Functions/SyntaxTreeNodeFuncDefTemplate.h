#pragma once

#include "../SyntaxTreeNodeFunc.h"

namespace WestCoastCode::Compilation
{
	/// @brief Represents a definition of a function template
	///
	/// Template Functions can be defined by:
	/// 
	/// @code
	/// func <T> Pow2(T value) T { 
	///		return value * value 
	/// }
	/// @endcode
	class ARLANG_API SyntaxTreeNodeFuncDefTemplate : public SyntaxTreeNodeFunc
	{
	public:
	};
}
