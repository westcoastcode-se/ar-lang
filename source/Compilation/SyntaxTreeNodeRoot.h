#pragma once

#include "SyntaxTreeNodePackage.h"

namespace WestCoastCode::Compilation
{
	// A package node
	class SyntaxTreeNodeRoot : public SyntaxTreeNodePackage
	{
	public:
		~SyntaxTreeNodeRoot() final;

		// Get the supplied primitive
		ISyntaxTreeNodePrimitive* FindPrimitive(ReadOnlyString name);

	public:
		ISyntaxTree* GetSyntaxTree() const final { return _syntaxTree; }

	private:
		friend class SyntaxTree;

		// Create a new syntax tree root
		static SyntaxTreeNodeRoot* Create();

		// Add the supplied node
		void AddPrimitive(ISyntaxTreeNodePrimitive* primitive);

		// set which syntax tree this node is part of
		void SetSyntaxTree(ISyntaxTree* syntaxTree) { _syntaxTree = syntaxTree; }

	private:
		SyntaxTreeNodeRoot();

		Map<ReadOnlyString, ISyntaxTreeNodePrimitive*> _primitives;
		ISyntaxTree* _syntaxTree;
	};
}
