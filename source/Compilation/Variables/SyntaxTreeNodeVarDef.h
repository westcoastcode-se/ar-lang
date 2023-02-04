#pragma once

#include "../SyntaxTreeNodeVar.h"

namespace WestCoastCode::Compilation
{
	/// @brief A variable definition
	class ARLANG_API SyntaxTreeNodeVarDef : public SyntaxTreeNodeVar
	{
	public:
		SyntaxTreeNodeVarDef(SourceCodeView sourceCode, ReadOnlyString name)
			: SyntaxTreeNodeVar(sourceCode, name) {}
	};
}
