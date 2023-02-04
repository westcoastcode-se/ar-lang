#pragma once

#include "SyntaxTreeNode.h"
#include "SyntaxTreeNodeType.h"

namespace WestCoastCode::Compilation
{
	/// @brief A variable
	class ARLANG_API SyntaxTreeNodeVar : public SyntaxTreeNode
	{
	public:
		SyntaxTreeNodeVar(SourceCodeView view, ReadOnlyString name);

		/// @return The name of this function
		inline ReadOnlyString GetName() const { return _name; }

		/// @return The type of this variable, for example an int32 primitive
		virtual SyntaxTreeNodeType* GetType() = 0;

	private:
		ReadOnlyString _name;
	};
}
