#pragma once

#include "SyntaxTreeNode.h"

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
		VisitResult Query(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor, QuerySearchFlags flags) final {
			return VisitResult::Continue;
		}
		const SourceCodeView* GetSourceCode() const final { return &_sourceCode; }

	private:
		ISyntaxTreeNodePackage* _parent;
		Vector<ISyntaxTreeNode*> _children;
		SourceCodeView _sourceCode;
	};
}
