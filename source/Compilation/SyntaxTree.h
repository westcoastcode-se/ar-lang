#pragma once

#include "SyntaxTreeNode.h"
#include "SyntaxTreeNodeRoot.h"

namespace WestCoastCode::Compilation
{
	class ISyntaxTree;
	class ISyntaxTreeNode;

	// Interface for the syntax tree
	class ISyntaxTree
	{
	public:
		virtual ~ISyntaxTree() {}

		// Stringify this syntax tree node
		virtual void ToString(StringStream& s) const = 0;

		// Visit all children in the entire tree
		virtual void Visit(ISyntaxTreeNodeVisitor* visitor, VisitFlags flags) = 0;

		// Get the root package where primitives and built-in functions are located
		virtual SyntaxTreeNodeRoot* GetRootNode() = 0;
	};
}
