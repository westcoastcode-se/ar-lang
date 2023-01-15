#pragma once

#include "SyntaxTreeNode.h"

namespace WestCoastCode::Compilation
{
	// Class that contains multiple return types
	class SyntaxTreeNodeMultiType : public ISyntaxTreeNodeMultiType
	{
	public:
		SyntaxTreeNodeMultiType(SourceCodeView sourceCode);

		~SyntaxTreeNodeMultiType() final;

		// Inherited via ISyntaxTreeNodeTypeRef
		const ID& GetID() const final { return _id; }
		ReadOnlyString GetName() const final { return ReadOnlyString(); }
		void ToString(StringStream& s, int indent) const final;
		ISyntaxTree* GetSyntaxTree() const final;
		ISyntaxTreeNode* GetRootNode() final;
		ISyntaxTreeNode* GetParent() const final { return _parent; }
		void SetParent(ISyntaxTreeNode* parent) final;
		ReadOnlyArray<ISyntaxTreeNode*> GetChildren() const final { return _children; }
		ReadOnlyArray<ISyntaxTreeNodeType*> GetTypes() const final { return _children; }
		const SourceCodeView* GetSourceCode() const final { return &_sourceCode; }
		ReadOnlyArray<ISyntaxTreeNode*> GetDefinitions() const final { return _definitions; }
		void ResolveReferences() final;

	public:
		// Add a new node
		void AddType(ISyntaxTreeNodeType* type);

		// Parse one or more types
		static SyntaxTreeNodeMultiType* Parse(const ParserState*);

	private:
		const ID _id;
		ISyntaxTreeNode* _parent;
		SourceCodeView _sourceCode;
		Vector<ISyntaxTreeNodeType*> _children;
		Vector<ISyntaxTreeNode*> _definitions;
	};
}
