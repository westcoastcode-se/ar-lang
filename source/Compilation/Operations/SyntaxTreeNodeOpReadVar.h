#pragma once

#include "../SyntaxTreeNodeOp.h"
#include "../Variables/SyntaxTreeNodeVarRef.h"

namespace WestCoastCode::Compilation
{
	/// @brief Read a variable
	class ARLANG_API SyntaxTreeNodeOpReadVar : public SyntaxTreeNodeOp
	{
	public:
		SyntaxTreeNodeOpReadVar(SourceCodeView view, SyntaxTreeNodeFuncDefBody* body);

		/// @return The variable we are reading from
		SyntaxTreeNodeVarRef* GetVariable();

#pragma region SyntaxTreeNodeOp
		SyntaxTreeNodeType* GetType() final;
		void ToString(StringStream& s, int indent) const final;
		void Compile(Builder::Linker* linker, Builder::Instructions& target) final;
#pragma endregion

	};

}
