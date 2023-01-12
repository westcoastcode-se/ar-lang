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
		const List<ISyntaxTreeNode*>& GetChildren() const final { return _children; }
		bool Visit(ISyntaxTreeNodeVisitor<const ISyntaxTreeNode>* visitor) const final;
		bool Visit(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor) final;
		bool Query(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor) final;
		const SourceCodeView* GetSourceCode() const final { return &_sourceCode; }

	private:
		ISyntaxTreeNodePackage* _parent;
		List<ISyntaxTreeNode*> _children;
		SourceCodeView _sourceCode;
	};
}
