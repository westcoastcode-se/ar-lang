#pragma once

#include "SyntaxTreeNode.h"

namespace WestCoastCode::Compilation
{
	// Class that contains one or many type definitions. Example on a use-case where this is used
	// is for functions returning more than one value
	class SyntaxTreeNodeTypes : public ISyntaxTreeNodeTypes
	{
	public:
		SyntaxTreeNodeTypes(SourceCodeView sourceCode);

		~SyntaxTreeNodeTypes() final;

		// Inherited via ISyntaxTreeNodeTypeRef
		const ID& GetID() const final { return _id; }
		void ToString(StringStream& s, int indent) const final;
		ISyntaxTree* GetSyntaxTree() const final;
		ISyntaxTreeNode* GetRootNode() final;
		ISyntaxTreeNode* GetParent() const final { return _parent; }
		void SetParent(ISyntaxTreeNode* parent) final;
		ReadOnlyArray<ISyntaxTreeNode*> GetChildren() const final { return _children; }
		const SourceCodeView* GetSourceCode() const final { return &_sourceCode; }
		ReadOnlyArray<ISyntaxTreeNode*> GetDefinitions() const final { return _definitions; }
		ISyntaxTreeNode* GetStackType() final { return this; }
		void ResolveReferences() final;

	public:
		// Add a new node
		void AddNode(ISyntaxTreeNode* node);

		// Parse one or more types
		static SyntaxTreeNodeTypes* Parse(const ParserState*);

	private:
		const ID _id;
		ISyntaxTreeNode* _parent;
		SourceCodeView _sourceCode;
		Vector<ISyntaxTreeNode*> _children;
		Vector<ISyntaxTreeNode*> _definitions;
	};
}
