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
		SyntaxTreeNodeImport(SourceCodeView sourceCode, SyntaxTreeNodePackage* import);

		~SyntaxTreeNodeImport() final;

		// Inherited via ISyntaxTreeNodeImport
		const ID& GetID() const final { return _id; }
		void ToString(StringStream& s, int indent) const final;
		ISyntaxTree* GetSyntaxTree() const final;
		ISyntaxTreeNode* GetRootNode() final;
		ISyntaxTreeNode* GetParent() const final;
		void SetParent(ISyntaxTreeNode* parent) final;
		ReadOnlyArray<ISyntaxTreeNode*> GetChildren() const final { return _children; }
		const SourceCodeView* GetSourceCode() const final { return &_sourceCode; }
		VisitResult Query(ISyntaxTreeNodeVisitor* visitor, QuerySearchFlags flags) final;

	private:
		const ID _id;
		ISyntaxTreeNodePackage* _parent;
		SyntaxTreeNodePackage* _import;
		Vector<ISyntaxTreeNode*> _children;
		SourceCodeView _sourceCode;
	};
}
