#pragma once

#include "../SyntaxTreeNodeOp.h"

namespace WestCoastCode::Compilation
{
	/// @brief Represents a scope inside a function body
	class SyntaxTreeNodeOpScope : public SyntaxTreeNodeOp
	{
	public:
		SyntaxTreeNodeOpScope(SourceCodeView view, SyntaxTreeNodeFuncBody* body)
			: SyntaxTreeNodeOp(view, body) {}

#pragma region SyntaxTreeNodeOp
		void ToString(StringStream& s, int indent) const final;
		SyntaxTreeNodeType* GetType() final { return nullptr; }
#pragma endregion
	};
}

