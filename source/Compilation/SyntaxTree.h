#pragma once

#include "SyntaxTreeNode.h"

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
		virtual void Visit(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor, VisitFlags flags) = 0;

		// Get the root package where primitives and built-in functions are located
		virtual ISyntaxTreeNodePackage* GetRootNode() = 0;
	};
}
