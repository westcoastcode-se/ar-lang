#pragma once

#include "SyntaxTree.h"

namespace WestCoastCode::Compilation
{
	class SyntaxTreeNodeFuncDef;

	// An return node for a function
	class SyntaxTreeNodeFuncRet : public ISyntaxTreeNodeFuncRet
	{
	public:
		SyntaxTreeNodeFuncRet(SourceCodeView sourceCode);

		~SyntaxTreeNodeFuncRet() final;

		// Inherited via ISyntaxTreeNodeFuncRet
		void ToString(StringStream& s, int indent) const final;
		ISyntaxTree* GetSyntaxTree() const final;
		ISyntaxTreeNode* GetRootNode() final;
		ISyntaxTreeNode* GetParent() const final { return _parent; }
		void SetParent(ISyntaxTreeNode* parent) final;
		ReadOnlyArray<ISyntaxTreeNode*> GetChildren() const final { return _children; }
		bool Visit(ISyntaxTreeNodeVisitor<const ISyntaxTreeNode>* visitor) const final;
		bool Visit(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor) final;
		bool Query(ISyntaxTreeNodeVisitor<ISyntaxTreeNode>* visitor) final;
		const SourceCodeView* GetSourceCode() const final { return &_sourceCode; }
		ISyntaxTreeNodeTypeRef* GetReturnType() const final { return _returnType; }

	public:
		// Add a child node
		void AddNode(ISyntaxTreeNode* node);

		// Set the return type for this return statement
		void SetReturnType(ISyntaxTreeNodeTypeRef* type);

		// Parse
		static SyntaxTreeNodeFuncRet* Parse(ParserState* state);

	private:
		ISyntaxTreeNode* _parent;
		SourceCodeView _sourceCode;
		Vector<ISyntaxTreeNode*> _children;
		ISyntaxTreeNodeTypeRef* _returnType;
	};
}
