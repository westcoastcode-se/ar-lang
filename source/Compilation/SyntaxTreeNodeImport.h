#pragma once

#include "SyntaxTree.h"

namespace WestCoastCode::Compilation
{
	class SyntaxTreeNodePackage;

	// Represents an import statement. 
	class SyntaxTreeNodeImport : public ISyntaxTreeNodeImport
	{
	public:
		// Import the supplied package
		SyntaxTreeNodeImport(SourceCodeView sourceCode, SyntaxTreeNodePackage* package);

		~SyntaxTreeNodeImport() final;

		// Inherited via ISyntaxTreeNodeImport
		void ToString(StringStream& s, int indent) const final;
		ISyntaxTree* GetSyntaxTree() const final;
		ISyntaxTreeNode* GetRootNode() final;
		ISyntaxTreeNode* GetParent() const final;
		void SetParent(ISyntaxTreeNode* parent) final;
		ReadOnlyArray<ISyntaxTreeNode*> GetChildren() const final { return _children; }
		VisitResult Visit(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor, VisitFlags flags) final {
			return VisitResult::Continue;
		}
		bool Query(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor, QuerySearchFlags flags) final {
			return false;
		}
		const SourceCodeView* GetSourceCode() const final { return &_sourceCode; }

	private:
		ISyntaxTreeNodePackage* _parent;
		Vector<ISyntaxTreeNode*> _children;
		SourceCodeView _sourceCode;
	};
}
