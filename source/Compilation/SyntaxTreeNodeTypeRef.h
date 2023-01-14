#pragma once

#include "SyntaxTreeNode.h"

namespace WestCoastCode::Compilation
{
	// Base class references that must point to a type
	class SyntaxTreeNodeTypeRef : public ISyntaxTreeNodeTypeRef
	{
	public:
		SyntaxTreeNodeTypeRef(SourceCodeView sourceCode);

		~SyntaxTreeNodeTypeRef() final;

		// Inherited via ISyntaxTreeNodeTypeRef
		const ID& GetID() const final { return _id; }
		void ToString(StringStream& s, int indent) const final;
		ISyntaxTree* GetSyntaxTree() const final;
		ISyntaxTreeNode* GetRootNode() final;
		ISyntaxTreeNode* GetParent() const final { return _parent; }
		void SetParent(ISyntaxTreeNode* parent) final;
		ReadOnlyArray<ISyntaxTreeNode*> GetChildren() const final { return _children; }
		const SourceCodeView* GetSourceCode() const final { return &_sourceCode; }
		ReadOnlyString GetName() const final { return _fullName; }
		ReadOnlyArray<ISyntaxTreeNode*> GetDefinitions() const final { return _definitions; }

	public:
		// Add a node to this type reference
		void AddNode(ISyntaxTreeNode* node);

		// Parse
		static SyntaxTreeNodeTypeRef* Parse(const ParserState* state);

	private:
		const ID _id;
		ISyntaxTreeNode* _parent;
		SourceCodeView _sourceCode;
		Vector<ISyntaxTreeNode*> _children;
		ReadOnlyString _fullName;
		Vector<ISyntaxTreeNode*> _definitions;
	};
}
