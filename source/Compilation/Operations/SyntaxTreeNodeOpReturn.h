#pragma once

#include "../SyntaxTreeNodeOp.h"

namespace WestCoastCode::Compilation
{
	/// @brief Represents a return statement
	class SyntaxTreeNodeOpReturn : public SyntaxTreeNodeOp
	{
	public:
		SyntaxTreeNodeOpReturn(SourceCodeView view, SyntaxTreeNodeFuncDefBody* body);

#pragma region SyntaxTreeNodeOp
		virtual void ToString(StringStream& s, int indent) const override;
		void Compile(Builder::Linker* linker, Builder::Instructions& target) final;
		SyntaxTreeNodeType* GetType() final;
		void Resolve() final;
#pragma endregion
	};
}
