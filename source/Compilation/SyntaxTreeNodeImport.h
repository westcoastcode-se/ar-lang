#pragma once

#include "SyntaxTreeNode.h"

namespace WestCoastCode::Compilation
{
	class SyntaxTreeNodePackage;

	/// @brief Represents an import statement
	class SyntaxTreeNodeImport : public SyntaxTreeNode
	{
	public:
		SyntaxTreeNodeImport(SourceCodeView view, SyntaxTreeNodePackage* import);

#pragma region SyntaxTreeNode
		void ToString(StringStream& s, int indent) const final;
		VisitResult Query(ISyntaxTreeNodeVisitor* visitor, QuerySearchFlags flags) final;
#pragma endregion

	private:
		SyntaxTreeNodePackage* const _import;
	};
}
